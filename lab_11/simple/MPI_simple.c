#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mpi.h"

int main(int argc, char** argv)
{ 
    int rank, ranksent, size;
    char hostname[64];
    char received_hostname[64];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Get host's name
    gethostname(hostname, sizeof(hostname));

    if (size > 1)
    {
        if (rank != 0)
        { 
            int dest = 0;

            // Send process's rank
            int tag = 0;
            MPI_Send(&rank, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);

            // Send process' hostname
            tag = 1;
            MPI_Send(hostname, strlen(hostname) + 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
        } 
        else
        {
            printf("/* Proces 0 odbiera dane */\n");
            for (int current_rank = 1; current_rank < size; current_rank++) 
            {
                // Receive host's rank
                MPI_Recv(&ranksent, 1, MPI_INT, current_rank, 0, MPI_COMM_WORLD, &status);

                // Receive host's name
                MPI_Recv(received_hostname, sizeof(received_hostname), MPI_CHAR, current_rank, 1, MPI_COMM_WORLD, &status);

                // Print data
                printf("Dane od procesu: %d\n", status.MPI_SOURCE);
                printf("\tRanga: %d\n", ranksent);
                printf("\tNazwa hosta: %s\n", received_hostname);
            }
        }
    }
    else
    {
        printf("Pojedynczy proces o randze %d na hoście %s (brak komunikatów)\n", rank, hostname);
    }

    MPI_Finalize();
    return 0;
}
