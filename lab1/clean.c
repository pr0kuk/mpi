#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
long double phi(long double x) {
    return (x*x*x) / (12 * (0.6*0.6));
}


long double analytical(long double x, long double t) {
    long double c = 0.6
    return (x+c*t)*((x+c*t)*(x+c*t)/3-(x-c*t)*(x-c*t))/8/c/c+(x-c*t)*(x-c*t)*(x-c*t)/6/c/c;
}
long double psi(long double t) {
    return 0.6 * (t*t*t) / 12;
}

int init_grid(long double* grid, int n, int m, long double h, long double tao) {
    int i = 0, j = 0, nm = n * m;
    for (i = 0; i < n; i++) {
        for(j = 0; j < m; j++) {
            *(grid+i*m+j) = 0;}}
    for (i = 0; i < m; i++)
        *(grid + i) = phi(i*h);
    for (i = 0; i < n; i++)
        *(grid+i*m) = psi(i*tao);
}

int print_grid(long double* grid, int n, int m) {
    int i = 0, j = 0;
    for (i = n-1; i >= 0; i--) {
        for(j = 0; j < m; j++)
            printf("%Lf ", *(grid+i*m+j));
        printf("\n");
    }
}

int print_str(long double* grid, int m) {
    int i = 0;
    for (i = 0; i < m; i++)
        printf("%Lf ", *(grid+i));
    printf("\n");
}

int gnuplotformat(long double* grid, int n, int m, long double tao, long double h) {
    int i =0, j = 0;
    for (i =0; i < n; i ++) {
        for (j = 0;j < m; j++)
            printf("%Lf %Lf %Lf\n", i*tao, j*h, *(grid+i*m+j));
        printf("\n");
    }
}

int fkm(long double t, long double x) {
    return t*x;
}

int krest2(long double* stroo, long double * stro, long double * strn, long double h, long double tao, int t, int x, int j) {
    strn[j] = fkm(t*tao,x*h) * 2 * tao - 0.6*tao*(stro[j+1] - stro[j-1]) / (long double)h + stroo[j]; 
}

int krest(long double * grid, int n, int m, long double h, long double tao, int t, int x) {
    *(grid+(t+1)*m+x) = fkm(t*tao,x*h) * 2 * tao - 0.6*tao*(*(grid+t*m+x+1) - *(grid+t*m+x-1)) / (long double)h + *(grid+(t-1)*m+x); 
}

int ugolok(long double * grid, int n, int m, long double h, long double tao, int t, int x) {
    *(grid+(t+1)*m+x) = fkm(t*tao,x*h) * tao - 0.6*tao*(*(grid+t*m+x) - *(grid+t*m+x-1)) / (long double)h + *(grid+(t)*m+x); 
}

int ugolok2(long double * str, long double u1, long double u2, long double h, long double tao, int t, int x, int size) {
    str[size-2] = fkm(t*tao,x*h) * tao - 0.6*tao*(u2 - u1) / (long double)h + u2; 
}

int init_sizes(int* sizes, int* startpos, int commsize, int m) {
    int i = 0;
    startpos[0] = 0;
    startpos[commsize] = m - 1;
    int t = m % commsize;
    int mp = m / commsize;
    for (i = 0; i < commsize; i++) {
        sizes[i] = mp;
        if (t > 0) {
            sizes[i]++;
            t--;
        }
        if (i > 0)
            startpos[i] = startpos[i-1] + sizes[i-1];
    }
    //sizes[0]--;
    return 0;
}
int copy_str(long double* sn, long double* so, int size)
{
    int i =0;
    for(; i < size; i++)
        sn[i] = so[i];
}

int change_str(long double* soo, long double* so, long double* sn, int size) {
    int i = 0;
    for (; i < size; i++) {
        soo[i] = so[i];
        so[i] = sn[i];
    }
}
int main(int argc, char* argv[])
{
    int n = atoi(argv[1]), m = n, i = 0, j = 0, *sizes, *startpos, commsize, my_rank;
    long double tao = 0.01, h = 0.01, /** grid = (long double*)calloc(n*m, sizeof(long double)),*/ *stroo, *stro, *strn, *Result, *temp;
    double t_start = 0, t_finish = 0;
    Result = (long double*)calloc(m, sizeof(long double));
    temp = (long double*)calloc(m*2, sizeof(long double));

    init_grid(temp, 2, m, h, tao);

    for (i = 1; i < m; i++)
        ugolok(temp, 2, m, h, tao, 0, i);
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    sizes = (int*)calloc(commsize, sizeof(int));
    startpos = (int*)calloc(commsize + 1, sizeof(int));
    init_sizes(sizes, startpos, commsize, m);
    stroo = (long double*)calloc(sizes[my_rank] + 2, sizeof(long double));
    stro = (long double*)calloc(sizes[my_rank] + 2, sizeof(long double));
    strn = (long double*)calloc(sizes[my_rank] + 2, sizeof(long double));


    if (my_rank == 0) {
        copy_str(stroo+1, temp, sizes[my_rank]+1);
        copy_str(stro+1, temp+m, sizes[my_rank]+1);
    }
    if (my_rank == commsize - 1) {
            copy_str(stroo, temp+startpos[my_rank] - 1, sizes[my_rank]+1);
            copy_str(stro, temp+startpos[my_rank] - 1 + m, sizes[my_rank]+1);
    }
    if (my_rank !=0 && my_rank != commsize - 1) {
            copy_str(stroo, temp+startpos[my_rank] - 1, sizes[my_rank]+2);
            copy_str(stro, temp+startpos[my_rank] - 1 + m, sizes[my_rank]+2);
    }
    /*if (!my_rank) {
        for (i = 0; i < commsize; i++) {
            printf("sizes[%d]=%d ", i, sizes[i]);
        }
        printf("\n");
        for (i = 0; i < commsize; i++) {
            printf("startpos[%d]=%d ", i, startpos[i]);
        }
        printf("\n");
    }*/
    t_start = MPI_Wtime();
    for (i = 1; i < (n-1); i++) {
        for (j = 1; j < sizes[my_rank] + 2; j++) {
            if (my_rank == 0 && j == 1)
                continue;
            if ((j ==1 ) && (my_rank != 0) && (i != 1)) {
                //printf("revs rank:%d, i:%d, j:%d\n", my_rank, i, j);
                MPI_Recv(stro, 1, MPI_LONG_DOUBLE, (my_rank + commsize-1)%commsize, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //printf("revf rank:%d, i:%d, j:%d, u:%Lf\n", my_rank, i, j, *(stro));
        }
            if ((my_rank != commsize - 1) && (j == sizes[my_rank]) && (i != 1)){
                //printf("revs rank:%d, i:%d, j:%d\n", my_rank, i, j);
                MPI_Recv(stro+sizes[my_rank] + 1, 1, MPI_LONG_DOUBLE, (my_rank+1)%commsize, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //printf("revf rank:%d, i:%d, j:%d, u:%Lf\n", my_rank, i, j, *(stro+sizes[my_rank]+1));
    }
            krest2(stroo, stro, strn, h, tao, i, j+startpos[my_rank] - 1, j);
            if ((my_rank != 0) && (j == 1)){
                //printf("sent rank:%d, i:%d, j:%d, u:%Lf\n", my_rank, i, j, *(strn+1));
                MPI_Send(strn + 1, 1, MPI_LONG_DOUBLE, (my_rank+commsize-1)%commsize, 0, MPI_COMM_WORLD);}
            if ((my_rank != commsize - 1) && (j == sizes[my_rank])) {
                //printf("sent rank:%d, i:%d, j:%d, u:%Lf\n", my_rank, i, j, *(strn+sizes[my_rank]));
                MPI_Send(strn+sizes[my_rank], 1, MPI_LONG_DOUBLE, (my_rank+1)%commsize, 0, MPI_COMM_WORLD);}
            if (j + startpos[my_rank] == m) {
                ugolok2(strn, stro[sizes[my_rank]-1], stro[sizes[my_rank]], h, tao, i, m-1, sizes[my_rank]+2);}
        }

        if(my_rank == 0)
            strn[1] = psi((i+1)*tao);
        change_str(stroo, stro, strn, sizes[my_rank]+2);
    }
    if (my_rank)
        MPI_Send(strn+1, sizes[my_rank], MPI_LONG_DOUBLE, 0, 1, MPI_COMM_WORLD);
    else {
        for (i = 1; i < commsize; i++)
            MPI_Recv(Result+startpos[i], sizes[i], MPI_LONG_DOUBLE, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        copy_str(Result, strn+1, sizes[my_rank]);
        t_finish = MPI_Wtime();
        //print_str(Result,m);
        printf("ANALITYCAL: %Lf, %Lf\n", analytical(n*h, m*tao));
        printf("CALC: %Lf\n", Result[m-1]);
        //print_str(Result, m);
        printf("NP:%d, n:%d, TIME %lf\n", commsize, n, t_finish-t_start);
    }
    MPI_Finalize();
}
