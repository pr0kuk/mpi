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

int print_ans(int*arr, int t, int n)
{
    int i = 0;
    for(i = t-n; i < t; i++)
        printf("%d ", arr[i]);
    printf("\n");
    return 0;
}

int main(int argc, char* argv[])
{
    int min = 1 << 31;
    int n = atoi(argv[1]), i = 0, n2 = 0, j =0, k =0, c = 0, l = 0, d = 0;
    int my_rank, commsize, np, t;
    //MPI_Init(&argc, &argv);
    //MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    //MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    commsize = 3;
    np = n / commsize;
    if (n % commsize != 0)
        np++;
    for (t = 1; t < np; t*=2);
    np = t;
    for (;t<commsize*np; t*=2);
    int* marr = (int*)malloc(t*sizeof(int));
    for(i = 0; i < t; marr[i++] = min);
    for (i = 0; i < n; i++)
        marr[i] = n - i;
    marr[5] = 228;
    marr[11] = 1;
    marr[3] = 47;
    marr[9] = 2;
    for (my_rank = 0; my_rank < commsize; my_rank++) {
        int* narr = marr + my_rank*np;
        for (k = 2; k <= np; k *= 2) {
            for (j=k>>1; j>0; j=j>>1) {
                for (i = 0; i < np; i++) {
                    l = i^j;
                    if (l > i) {
                        if (l >= np)
                            l = np - 1;
                        if ((i&k) == 0 && narr[i] > narr[l])
                            SWAP(narr, i, l);
                        if ((i&k) != 0 && narr[i] < narr[l])
                            SWAP(narr, i, l);
                    }
                }
            }
        }
    }
    for (k = k /2; k <= t; k *= 2) {
        for (j=k>>1; j>0; j=j>>1) {
            for (i = 0; i < t; i++) {
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
    print_ans(marr, t, n);
    //MPI_FINALIZE();
}