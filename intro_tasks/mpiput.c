#define MAX_PROC 6
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("n is not defined\n");
        exit(-1);
    }
    int my_rank = 0, commsize = 0, i = 0, j = 0, n = atoi(argv[1]);
    long double summ = 0, ans = 0, Result[MAX_PROC];
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
    MPI_Win window;
    MPI_Win_create(&Result, sizeof(long double)*MAX_PROC, sizeof(long double), MPI_INFO_NULL, MPI_COMM_WORLD, &window);
    MPI_Win_fence(0, window);

    for (i = my_rank + 1; i <= n; i += commsize)
        summ += 1/(long double)i;

    MPI_Put(&summ, 1, MPI_LONG_DOUBLE, 0, my_rank, 1, MPI_LONG_DOUBLE, window);
    MPI_Win_fence(0, window);

    if (!my_rank) {
        for(j = 0; j < commsize; ans += Result[j++]);
        printf("\nanswer is %Lf\n", ans);
    }
    MPI_Finalize();
    return 0;
}