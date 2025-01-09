#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "mpi.h"

#define ROOT 0
#define enable_user_input false
#define default_digit_num 1000

int main(int argc, char** argv)
{
    /* Initialization */
    MPI_Init(&argc, &argv);

    // Get ID and size
    int id, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Get the required numebr of digits 
    int global_digits = default_digit_num;
    if (id == ROOT) // Root performs the reading
    {
        if (enable_user_input)
        {
            printf("PI digits: ");
            fflush(stdout);
            scanf("%d", &global_digits);
        }
    }

    // Broadcast the number of digits to all processes
    MPI_Bcast(&global_digits, 1, MPI_INT, ROOT, MPI_COMM_WORLD);

    /* Task */
    // Decompose
    int global_block_length = ceil((double)global_digits / (double)size);

    int local_start = id * global_block_length;
    int local_stride = 1;
    int local_end = (id == size - 1) ? global_digits : local_start + global_block_length;

    // Calculate PI
    // Formula:
    // Pi = (1 - 1 / 3 + 1/5 - 1/7 + 1/9...) * 4

    // Local calculation
    // Formula: local_pi = (-1/3 + 1/5 - 1/7 + 1/9.. )
    double local_pi = 0.f;

    for (int i = local_start; i < local_end; i += local_stride)
    {
        double denominator = 1 + (i * 2);
        double fraction = 1.f / denominator;

        if (i % 2 == 0)
            local_pi += fraction;
        else 
            local_pi -= fraction;
    }

    // Sum up all results at root
    double root_pi_sum = 0.f;
    MPI_Reduce(&local_pi, &root_pi_sum, 1, MPI_DOUBLE, MPI_SUM, ROOT, MPI_COMM_WORLD);

    // Finish the calculation and print the result as root
    if (id == ROOT)
    {
        double root_final_pi = root_pi_sum * 4.f;
        printf ("Final result: %lf\n", root_final_pi);
    }

    /* Finalization */
    MPI_Finalize();
}