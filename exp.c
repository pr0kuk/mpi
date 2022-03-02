#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

double Fact(int n)
{
    if (n == 0)
        return 1;
    else 
        return n * Fact(n - 1);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("n is not defined\n");
        exit(-1);
    }
    int n = atoi(argv[1]), my_rank = 0, commsize = 0, i = 0;
    long double c_k = 0, sumc_k = 0, Result = 0, sum = 0;
    double startwtime = 0, endwtime = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
 
    if (my_rank == 0)
        startwtime = MPI_Wtime();

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    for (i = my_rank; i <= n; i += commsize)
        c_k = 1/Fact(i), sumc_k += c_k;

    MPI_Reduce(&sumc_k, &Result, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    if (my_rank == 0) {   
        printf("\nResult %.70Lf\n", Result);
        endwtime = MPI_Wtime();
        printf("Time %f\n", endwtime-startwtime);  
    }

    MPI_Finalize();
    return 0;
}
