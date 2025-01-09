#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#define process_number 4
#define name_length 100

typedef struct 
{
    int id;
    float value;
    char name[name_length];
} CustomType;

int main(int argc, char** argv) 
{
    /* Initialize */
    MPI_Init(&argc, &argv);

    int id, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Task */
    if (id == 0) // Process 0 sends data
    {
        // Prepare data
        CustomType data = {1, 3.14, "Gorilla"};
        int buffer_size = 0, temp_size;

        // Calculate buffer size
        MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &temp_size);
        buffer_size += temp_size;
        MPI_Pack_size(1, MPI_FLOAT, MPI_COMM_WORLD, &temp_size);
        buffer_size += temp_size;
        MPI_Pack_size(name_length, MPI_CHAR, MPI_COMM_WORLD, &temp_size);
        buffer_size += temp_size;

        // Allocate buffer
        char* buffer = malloc(buffer_size);

        // Pack data
        int position = 0;
        MPI_Pack(&data.id, 1, MPI_INT, buffer, buffer_size, &position, MPI_COMM_WORLD);
        MPI_Pack(&data.value, 1, MPI_FLOAT, buffer, buffer_size, &position, MPI_COMM_WORLD);
        MPI_Pack(data.name, name_length, MPI_CHAR, buffer, buffer_size, &position, MPI_COMM_WORLD);

        // Send the buffer to each process
        for (int i = 1; i < process_number; i++)
            MPI_Send(buffer, position, MPI_PACKED, i, 0, MPI_COMM_WORLD);

        free(buffer);
    } 
    else // Other processes receive and print data
    {
        // Probe for message status
        MPI_Status status;
        MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
    
        // Read buffer size
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
        MPI_Unpack(buffer, buffer_size, &position, data.name, name_length, MPI_CHAR, MPI_COMM_WORLD);

        // Print received data
        printf("======== Process %d ========\n", id);
        printf("Data received:\n");
        printf("\tID: %d\n", data.id);
        printf("\tValue: %.2f\n", data.value);
        printf("\tName: %s\n", data.name);

        // Free dynamic data
        free(buffer);
    }

    /* Finalize */
    MPI_Finalize();
    return 0;
}
