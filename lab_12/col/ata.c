#include <stdio.h>
#include "mpi.h"

#define ROOT 0
#define NUMBERS 10

int main(int argc, char** argv)
{
    /* Initilization */
    MPI_Init(&argc, &argv);

    // Gather Comm data
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // printf("Rank: %d\tSize: %d\n", rank, size);

    // Initialize A and B arrays
    int A[NUMBERS] = {}, B[NUMBERS] = {};
    for (int i = 0; i < NUMBERS; i++)
    {
        A[i] = (rank + i) * 2;
        B[i] = i;
    }

    // Debug: Verify array initialization
    /*if (rank == ROOT)
    {
        for (int i = 0; i < NUMBERS; i += 1)
            printf ("A[%d] = %d\n", i, A[i]);

        for (int i = 0; i < NUMBERS; i += 1)
            printf ("B[%d] = %d\n", i, B[i]);
    }*/

    /* Task */
    
    // Group distribution function call
    // MPI_Alltoall(&A[1], 2, MPI_INT, &B[0], 2, MPI_INT, MPI_COMM_WORLD);
    int rc = MPI_Allgather(&A[1], 2, MPI_INT, B, 2, MPI_INT, MPI_COMM_WORLD);

    // Display the result for each process in synchronised manner
    for (int i = 0; i < size; i++)
    {
        if (rank == i)
        {
            if (rank == 0)
                printf ("--------- Results ----------\n");

            printf ("Process: %d\n", i);
            // for (int j = 0; j < 10; j++)
               // printf("\tA%d[%d] = %d\n", i, j, A[j]);
            
            for (int j = 0; j < 6; j++)
                printf("\tB%d[%d] = %d\n", i, j, B[j]);

            // Ensure buffer is clear before handing to next process
            fflush(stdout);
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }


    /* Finalization */
    MPI_Finalize();
}