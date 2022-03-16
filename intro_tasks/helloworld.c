#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int my_rank, commsize;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    printf("\n");
    printf("Hello World, my rank is %d, total number of executors is %d\n", my_rank, commsize); 
    MPI_Finalize();
    return 0;
}