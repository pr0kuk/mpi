#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int my_rank, commsize, j = 0, i = 0, n = atoi(argv[1]);
    double t_start = 0, t_finish = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    if (my_rank == 0)
        printf("\n");
    t_start = MPI_Wtime();
    for (j = 0; j < n; j++) {
        if (j == 0 && my_rank == 0)
            MPI_Send(&i, 1, MPI_INT, (my_rank+1)%commsize, 0, MPI_COMM_WORLD);
        MPI_Recv(&i, 1, MPI_INT, (my_rank + commsize -1)%commsize, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        i++;
        MPI_Send(&i, 1, MPI_INT, (my_rank+1)%commsize, 0, MPI_COMM_WORLD);
    }
    t_finish = MPI_Wtime();
    printf("TIME %lf\n", (t_finish - t_start)/(n*commsize));
    MPI_Finalize();
    return 0;
}