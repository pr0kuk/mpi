#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int my_rank, commsize, j = 0, i = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    for (j = 0; j < 10; j++) {
        if (j == 0 && my_rank == 0)
            MPI_Send(&i, 1, MPI_INT, (my_rank+1)%commsize, 0, MPI_COMM_WORLD);
        MPI_Recv(&i, 1, MPI_INT, (my_rank-1)%commsize, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        i++;
        MPI_Send(&i, 1, MPI_INT, (my_rank+1)%commsize, 0, MPI_COMM_WORLD);
        printf("inc i is %d, my_rank is %d, commsize is %d\n", i, my_rank, commsize);
    }
    MPI_Finalize();
    return 0;
}