#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
long double phi(long double x) {
    return (x*x*x) / (12 * (0.6*0.6));
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

int print_str(long double* grid, int n, int m) {
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

int krest(long double * grid, int n, int m, long double h, long double tao, int t, int x) {
    *(grid+(t+1)*m+x) = fkm(t*tao,x*h) * 2 * tao - 0.6*tao*(*(grid+t*m+x+1) - *(grid+t*m+x-1)) / (long double)h + *(grid+(t-1)*m+x); 
}

int ugolok(long double * grid, int n, int m, long double h, long double tao, int t, int x) {
    *(grid+(t+1)*m+x) = fkm(t*tao,x*h) * tao - 0.6*tao*(*(grid+t*m+x) - *(grid+t*m+x-1)) / (long double)h + *(grid+(t)*m+x); 
}


int main(int argc, char* argv[])
{
    int n = atoi(argv[1]), m = n;
    long double tao = 0.01, h = 0.01;
    double t_start = 0, t_finish = 0;
    int i = 0, j = 0;
    int commsize, my_rank;
    long double* grid = (long double*)calloc(n*m, sizeof(long double));
    if (grid == NULL) {
        perror("MALLOC: ");
        return -1;
    }
    init_grid(grid, n, m, h, tao);
    for (i = 1; i < m; i++)
        ugolok(grid, n, m, h, tao, 0, i);
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    t_start = MPI_Wtime();
    int mp = (m + commsize - 1) / commsize;
    for (i = 1; i < (n-1); i++) {
        for (j = my_rank * mp + 1; (j < (m-1)) && (j < my_rank * mp + mp + 1); j++) {
            if ((j != 1) && (j % mp == 1) && (i != 1))
                MPI_Recv(grid+i*m+j-1, 1, MPI_LONG_DOUBLE, (my_rank + commsize-1)%commsize, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if ((j != m-2) && (j % mp == 0) && (i != 1))
                MPI_Recv(grid+i*m+j+1, 1, MPI_LONG_DOUBLE, (my_rank+1)%commsize, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            krest(grid, n, m, h, tao, i, j);
            if ((j != 1) && (j % mp == 1))
                MPI_Send(grid+(i+1)*m+j, 1, MPI_LONG_DOUBLE, (my_rank+commsize-1)%commsize, 0, MPI_COMM_WORLD);
            if ((j != (m-2)) && ((j) % mp == 0))
                MPI_Send(grid+(i+1)*m+j, 1, MPI_LONG_DOUBLE, (my_rank+1)%commsize, 0, MPI_COMM_WORLD);
            if (j + 1 + commsize * mp > (m-1))
                ugolok(grid, n, m, h, tao, i, m-1);
        }
    }
    if (my_rank)
        for (i = 1; i < n; i++)
            MPI_Send(grid+i*m +my_rank*mp+1, (my_rank == commsize-1 && m%mp != 0) ? m%mp : mp, MPI_LONG_DOUBLE, 0, 1, MPI_COMM_WORLD);
    else {
        for (i = 1; i < commsize; i++)
            for (j = 1; j < n; j++)
                MPI_Recv(grid+i*mp+1+j*m,(i == commsize-1 && m%mp != 0) ? m%mp : mp, MPI_LONG_DOUBLE, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        t_finish = MPI_Wtime();
        //printf("TIME %lf\n", t_finish-t_start);
        print_grid(grid, n, m);
        //gnuplotformat(grid, n, m, tao, h);
    }
    MPI_Finalize();
}