#define SWAP(arr, i, l) do{c = *(arr+i), *(arr+i) = *(arr+l), *(arr+l) = c;}while(0)
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int print_arr(int* arr, int n)
{
    int i = 0, min = 1 << 31;
    for (i = 0; i < n; i++) {
        if (arr[i] == min)
            printf("MIN ");
        else
            printf("%d ", arr[i]);
    }
    printf("\n");
}

int print_ans(int*arr, int t, int n)
{
    int i = 0;
    for(i = t-n; i < t; i++)
        printf("%d ", arr[i]);
    printf("\n");
    return 0;
}

int file_arr(int* marr, int n, char* filename) {
    FILE * fd = fopen(filename, "r");
    int i = 0;
    for(i = 0; i < n; i++)
        fscanf(fd, "%d", marr+i);
    fclose(fd);
}

int init_arr(int* marr, int* rarr,int n, int t) {
    int i = 0, min = 1 << 31;
    for(i = 0; i < t; marr[i++] = min);
    for(i = 0; i < t; rarr[i++] = min);
}

int get_np(int n, int commsize, int* np, int* t) {
    *np = n / commsize;
    if (n % commsize != 0)
        (*np)++;
    for (*t = 1; *t < *np; (*t)*=2);
    *np = *t;
    for (;*t<commsize*(*np); (*t)*=2);
}

int bitonic_sort(int* narr, int k, int np) {
    int j = 0, l = 0, i = 0, c = 0;
    for (;k <= np; k *= 2) {
        for (j = k >> 1; j > 0; j = j >> 1) {
            for (i = 0; i < np; i++) {
                l = i ^ j;
                if (l > i) {
                    if ((i & k) == 0 && narr[i] > narr[l])
                        SWAP(narr, i, l);
                    if ((i & k) != 0 && narr[i] < narr[l])
                        SWAP(narr, i, l);
                }
            }
        }
    }
    return k;
}

int main(int argc, char* argv[])
{
    int n = atoi(argv[1]), k = 0, my_rank = 0, commsize = 0, np = 0, t = 0;
    char* filename = argv[2];
    double t_start = 0, t_finish = 0;
    int * marr, *rarr, *narr;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    
    get_np(n, commsize, &np, &t);
    marr = (int*)malloc(t*sizeof(int));
    rarr = (int*)malloc(t*sizeof(int));
    init_arr(marr, rarr, n ,t);
    file_arr(marr, n, filename);

    if (my_rank == 0) {
        printf("\n");
        //print_arr(marr, t);
    }
    t_start = MPI_Wtime();
    narr = marr + my_rank * np;
    k = bitonic_sort(narr, 2, np);
    MPI_Gather(marr+my_rank*np, np, MPI_INT, rarr, np, MPI_INT, 0, MPI_COMM_WORLD);
    if (my_rank == 0) {
        bitonic_sort(rarr, k/2, t);
        t_finish = MPI_Wtime();
        print_ans(rarr, t, n);
        printf("TIME %lf\n", t_finish-t_start);
    }
    MPI_Finalize();
    return 0;
}   