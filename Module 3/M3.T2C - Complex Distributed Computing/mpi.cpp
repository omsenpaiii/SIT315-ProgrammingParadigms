#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define MAX 1000000

int compare(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Initialize the random seed using the current time
    srand(time(NULL) + rank);

    // Determine the local portion of the array to be sorted
    int local_size = MAX / size;
    int local_data[local_size];
    for (int i = 0; i < local_size; i++)
    {
        local_data[i] = rand() % 20;
    }

    // Start the clock to measure the execution time of the sorting
    double start_time = MPI_Wtime();

    // Sort the local portion of the array using the qsort() function
    qsort(local_data, local_size, sizeof(int), compare);

    // Combine the sorted local arrays using MPI_Allgather
    int sorted_data[MAX];
    MPI_Allgather(local_data, local_size, MPI_INT, sorted_data, local_size, MPI_INT, MPI_COMM_WORLD);

    // Stop the clock to measure the execution time of the sorting
    double end_time = MPI_Wtime();

    // Output the execution time of the sorting
    if (rank == 0)
    {
        std::cout << "Execution time = " << end_time - start_time << " seconds" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
