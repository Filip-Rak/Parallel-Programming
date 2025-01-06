#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

void verify_status(int return_code, int id)
{
    if (return_code != MPI_SUCCESS)
        printf ("FAILURE: Return code is: %d for ID: %d\n", return_code, id);
}

int main(int argc, char** argv)
{
    /* Process's properties */
    int id;
    int size;

    // Used only for return code
    int rc = 0;

    /* Initialization */
    rc = MPI_Init(&argc, &argv);
    verify_status(rc, 1);

    // Receive id and number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Task */
    // Proprties of the task
    MPI_Status status;
    int source = id -1;
    int destination = id + 1;
    int data = 0;

    // Await data
    if (id != 0)    // Ignored by process 0
    {
        rc = MPI_Recv(&data, 1, MPI_INT, source, 0, MPI_COMM_WORLD, &status);
        verify_status(rc, 3);
        
        // Output data to console
        printf("RECEIVING\n\tFrom id: %d\n\tTo id: %d\n\tData: %d\n", status.MPI_SOURCE, id, data);
    }

    // Increment the value
    data += 1;

    // Send data
    if (id != size - 1) // Ignored by last process
    {
        // State intent in console
        printf("SENDING\n\tFrom id: %d\n\tTo id: %d\n\tData: %d\n", id, destination, data);

        rc = MPI_Send(&data, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);
        verify_status(rc, 5);
    }


    /* Finalization */
    rc = MPI_Finalize();
    verify_status(rc, 2);
}