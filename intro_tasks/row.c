#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"


int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("n is not defined\n");
        exit(-1);
    }

    int my_rank = 0, commsize = 0, i = 0, n = atoi(argv[1]);
    long double summ = 0, Result = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    for (i = my_rank + 1; i <= n; i += commsize)
        summ += 1/(long double)i;
    MPI_Reduce(&summ, &Result, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (my_rank == 0)
        printf("\nanswer is %Lf\n", Result);
    MPI_Finalize();
    return 0;
}