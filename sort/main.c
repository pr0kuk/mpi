#define MAX_DIGITS 6
#define SWAP(arr, i, l) do{c = *(arr+i), *(arr+i) = *(arr+l), *(arr+l) = c;}while(0)
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include "mpi.h"
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int print_arr(int* arr, int n)
{
    int i = 0;
    for (i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int main(int argc, char* argv[])
{
    int n = atoi(argv[1]), i = 0, n2 = 0, j =0, k =0, c = 0, l = 0, d = 0;
    int my_rank, commsize;
    int* marr = (int*)malloc((n+1)*sizeof(int));
    for (i = 0; i < n; i++) {
        marr[i] = n - i;
    }
    print_arr(marr, n);
    //MPI_Init(&argc, &argv);
    //MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    //MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    commsize = 4;
    for (my_rank = 0; my_rank < commsize; my_rank++) {
        printf("rank is %d\n", my_rank);
        int np = n / commsize;
        if (my_rank == commsize)
            np+=n%commsize;
        int* narr = marr + my_rank*np;
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
    }
    print_arr(marr, n);
    //reduce
    for (k = k /2; k <= n; k *= 2) {
        for (j=k>>1; j>0; j=j>>1) {
            for (i = 0; i < n; i++) {
                l = i^j;
                if (l > i) {
                    if ((i&k) == 0 && marr[i]> marr[l])
                        SWAP(marr, i, l);   
                    if ((i&k) != 0 && marr[i] < marr[l])
                        SWAP(marr, i, l);
                }
            }
        }
    }
    print_arr(marr, n);
    //MPI_FINALIZE();
}