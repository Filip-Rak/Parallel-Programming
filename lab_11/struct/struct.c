#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "mpi.h"

#define processes 3
#define name_length 50

typedef struct
{
    int id;
    float value;
    char name[name_length];
} CustomType;

void create_mpi_type(MPI_Datatype *mpi_custom_type)
{
    int block_lengths[3] = {1, 1, name_length};
    MPI_Datatype types[3] = {MPI_INT, MPI_FLOAT, MPI_CHAR};

    CustomType temp;
    MPI_Aint displacements[3];
    displacements[0] = (char*)&temp.id - (char*)&temp;
    displacements[1] = (char*)&temp.value - (char*)&temp;
    displacements[2] = (char*)&temp.name - (char*)&temp;

    MPI_Type_create_struct(3, block_lengths, displacements, types, mpi_custom_type);
    MPI_Type_commit(mpi_custom_type);
}

void process_data(CustomType* data, int id)
{
    if (id == 1)
    {
        // Count all the characters
        int i = 0;
        while (data->name[i] != '\0')
        {
            data->value += 1;
            i += 1;
        }
    }
    else if (id == 2)
    {
        // Uppercase names
        int i = 0;
        while (data->name[i] != '\0')
        {
            // Make sure the character is lowercase
            if (data->name[i] >= 'a' && data->name[i] <= 'z')
            {
                data->name[i] -= 32;
            }
            i += 1;
        }
    }
    else if (id == 3)
    {
        // Sum up all ASCII values
        int i = 0;
        data->value = 0;
        while (data->name[i] != '\0')
        {
            data->value += data->name[i];
            i += 1;
        }
    }

    // Update the id of the data
    data->id = id;
}


int main(int argc, char **argv)
{
    /* Initialization */
    MPI_Init(&argc, &argv);

    // Retrieve id and size
    int id, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Create a custom MPI type
    MPI_Datatype mpi_custom_type;
    create_mpi_type(&mpi_custom_type);

    /* Task */

    // Process 0 creates and provides the dataset
    if (id == 0)
    {
        // Make a custom dataset
        CustomType dataset[] = 
        {
            {0, 0, "Walter"},
            {0, 0, "Ayre"},
            {0, 0, "Michigan"},
            {0, 0, "Chatty"},
            {0, 0, "Snail"},
            {-1, 0, ""},
        };

        // Send the dataset to next process
        int arr_size = sizeof(dataset) / sizeof(CustomType);
        for (int i = 0; i < arr_size; i++)
            MPI_Send(&dataset[i], 1, mpi_custom_type, 1, 0, MPI_COMM_WORLD);
    }
    else    // Rest of processes
    {
        CustomType data;
        MPI_Status status;
        while (true)
        {
            // Await the transfer
            MPI_Recv(&data, 1, mpi_custom_type, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

            // If last package is found
            if (data.id == -1 && id < size - 1)
            {
                // Pass it on and break out of loop
                MPI_Send(&data, 1, mpi_custom_type, id + 1, 0, MPI_COMM_WORLD);
                break;
            }

            // Display data
            printf("PRIOR WORK:\n\tFrom: %d\n\tTo: %d\n\tData:\n\tID: %d\n\tValue: %f\n\tName: %s\n",status.MPI_SOURCE, id, data.id, data.value, data.name);

            // Process the data
            process_data(&data, id);

            // Display data
            printf("AFTER WORK:\n\tWorker: %d\n\tData:\n\tID: %d\n\tValue: %f\n\tName: %s\n",id, data.id, data.value, data.name);

            // Pass data to next process
            if (id != size - 1) // Ignore for last
            {
                MPI_Send(&data, 1, mpi_custom_type, id + 1, 0, MPI_COMM_WORLD);
            }
            else // Last process breaks out
            {
                break;
            }
        }
    }

    /* Finalization */
    MPI_Finalize();
}