#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

typedef struct 
{
    int id;
    float value;
    char name[100];
} CustomType;

int main(int argc, char** argv) 
{
    /* Initialize */
    MPI_Init(&argc, &argv);

    int id, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (id == 0) // Process 0 sends data
    {
        // Prepare data
        CustomType data = {1, 3.14, "Gorilla"};
        int buffer_size = 0, temp_size, position = 0;

        // Calculate buffer size
        MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &temp_size);
        buffer_size += temp_size;
        MPI_Pack_size(1, MPI_FLOAT, MPI_COMM_WORLD, &temp_size);
        buffer_size += temp_size;
        MPI_Pack_size(100, MPI_CHAR, MPI_COMM_WORLD, &temp_size);
        buffer_size += temp_size;

        // Allocate buffer
        char* buffer = malloc(buffer_size);

        // Pack data
        MPI_Pack(&data.id, 1, MPI_INT, buffer, buffer_size, &position, MPI_COMM_WORLD);
        MPI_Pack(&data.value, 1, MPI_FLOAT, buffer, buffer_size, &position, MPI_COMM_WORLD);
        MPI_Pack(data.name, 100, MPI_CHAR, buffer, buffer_size, &position, MPI_COMM_WORLD);

        // Send the buffer
        MPI_Send(buffer, position, MPI_PACKED, 1, 0, MPI_COMM_WORLD);

        free(buffer);
    } 
    else if (id == 1) // Process 1 receives data
    {
        // Probe for message size
        MPI_Status status;
        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);

        int buffer_size;
        MPI_Get_count(&status, MPI_PACKED, &buffer_size);

        // Allocate buffer
        char* buffer = malloc(buffer_size);

        // Receive buffer
        MPI_Recv(buffer, buffer_size, MPI_PACKED, 0, 0, MPI_COMM_WORLD, &status);

        // Unpack data
        CustomType data;
        int position = 0;
        MPI_Unpack(buffer, buffer_size, &position, &data.id, 1, MPI_INT, MPI_COMM_WORLD);
        MPI_Unpack(buffer, buffer_size, &position, &data.value, 1, MPI_FLOAT, MPI_COMM_WORLD);
        MPI_Unpack(buffer, buffer_size, &position, data.name, 100, MPI_CHAR, MPI_COMM_WORLD);

        // Print received data
        printf("======== Process %d ========\n", id);
        printf("Data received:\n");
        printf("\tID: %d\n", data.id);
        printf("\tValue: %.2f\n", data.value);
        printf("\tName: %s\n", data.name);

        free(buffer);
    }

    /* Finalize */
    MPI_Finalize();
    return 0;
}
