#define MAX_DIGITS 6
#define SWAP(arr, i, l) do{c = *(arr+i), *(arr+i) = *(arr+l), *(arr+l) = c;}while(0)
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "mpi.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

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

int init_arr(int* marr, int* rarr,int n, int t) {
    int i = 0, min = 1 << 31;
    for(i = 0; i < t; marr[i++] = min);
    for(i = 0; i < t; rarr[i++] = min);
    for (i = 0; i < n; i++)
        marr[i] = n - i;
    marr[5] = 228;
    marr[11] = 1;
    marr[3] = 47;
    marr[9] = 2;
}

int get_np(int n, int commsize, int* np, int* t) {
    *np = n / commsize;
    if (n % commsize != 0)
        (*np)++;
    for (*t = 1; *t < *np; (*t)*=2);
    *np = *t;
    for (;*t<commsize*(*np); (*t)*=2);
}

int main(int argc, char* argv[])
{
    int min = 1 << 31;
    int n = atoi(argv[1]), i = 0, n2 = 0, j =0, k =0, c = 0, l = 0, d = 0;
    int my_rank, commsize, np, t;
    double t_start, t_finish;
    int * marr, *rarr, *narr;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    get_np(n, commsize, &np, &t);
    marr = (int*)malloc(t*sizeof(int));
    rarr = (int*)malloc(t*sizeof(int));
    init_arr(marr, rarr, n ,t);
    if (my_rank == 0) {
        //printf("\n");
        print_arr(marr, t);
        //printf("np is %d, t is %d\n", np, t);
    }
    t_start = MPI_Wtime();
    narr = marr + my_rank*np;
    for (k = 2; k <= np; k *= 2) {
        for (j=k>>1; j>0; j=j>>1) {
            for (i = 0; i < np; i++) {
                l = i^j;
                if (l > i) {
                    if ((i&k) == 0 && narr[i] > narr[l])
                        SWAP(narr, i, l);
                    if ((i&k) != 0 && narr[i] < narr[l])
                        SWAP(narr, i, l);
                }
            }
        }
    }
    MPI_Gather(marr+my_rank*np, np, MPI_INT, rarr, np, MPI_INT, 0, MPI_COMM_WORLD);
    if (my_rank == 0) {
        for (k = k /2; k <= t; k *= 2) {
            for (j=k>>1; j>0; j=j>>1) {
                for (i = 0; i < t; i++) {
                    l = i^j;
                    if (l > i) {
                        if ((i&k) == 0 && rarr[i]> rarr[l])
                            SWAP(rarr, i, l);   
                        if ((i&k) != 0 && rarr[i] < rarr[l])
                            SWAP(rarr, i, l);
                    }
                }
            }
        }
        t_finish = MPI_Wtime();
        print_ans(rarr, t, n);
        printf("TIME %lf\n", t_finish-t_start);
    }
    MPI_Finalize();
    return 0;
}   