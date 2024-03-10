// OpenMP MPI Matrix Multiplication
// Author: Om Tomar
// Date: 09/03/24

// To compile:
// $ mpicxx openmpMPI.cpp -fopenmp

// To run:
// $ mpirun -np 4 --hostfile ~/Desktop/Slave.list
// $ mpirun -np 4 ./a.out

#include<mpi.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/time.h>
#include<time.h>
#include<omp.h>

#define N 800 // Define the size of the matrices

using namespace std;

void intialiseArray(int array[N][N]); // Function to initialize the array with random values
void printArrays(int array[N][N]); // Function to print arrays to the console
void openmpMatrixMultiplication(int np, int rank, int inputArray1[N][N], int inputArray2[N][N], int outputArray[N][N]); // Function to perform matrix multiplication
//void MatrixMultiplication(int np, int rank, int inputArray1[N][N], int inputArray2[N][N], int outputArray[N*N]);
struct timeval timecheck;

void printOutput(int outputArray[N]){
    for (int i = 0 ; i < N; i++){
        printf(" %d :", outputArray[i]);
    }
    
}


int main(){
    MPI_Init(NULL, NULL);

    int np = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &np);     // Get the number of nodes

    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);    // Get the rank of the current process

    int inputArray1[N][N], inputArray2[N][N]; // Declare input arrays
    int outputArray[N][N]={{0}}; // Declare output array
    //int outputArray[N*N]={0};

    if (rank==0) { // If it is the root process
        intialiseArray(inputArray1); // Initialize inputArray1
        intialiseArray(inputArray2); // Initialize inputArray2

        //printArrays(inputArray1);
        //printArrays(inputArray2);
    }else{
        //printArrays(outputArray);
    }

    //Timer Start
    if (rank == 0){
        printf("OpenMP MPI Matrix Multiplication.\n");
    }

    gettimeofday(&timecheck, NULL);
	long timeofday_start = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec /1000;

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&inputArray1, N*N, MPI_INT, 0, MPI_COMM_WORLD); // Broadcast inputArray1 to all processes
    MPI_Bcast(&inputArray2, N*N, MPI_INT, 0, MPI_COMM_WORLD); // Broadcast inputArray2 to all processes

    MPI_Barrier(MPI_COMM_WORLD);
    openmpMatrixMultiplication(np, rank, inputArray1, inputArray2, outputArray); // Perform matrix multiplication
    MPI_Barrier(MPI_COMM_WORLD);
    
    //if(rank==0)printArrays(outputArray);
    
    MPI_Finalize(); // Finalize MPI

    //Timer end
    gettimeofday(&timecheck, NULL);
	long timeofday_end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec /1000;
	double time_elapsed = timeofday_end - timeofday_start;
	
    if (rank == 0){
        printf("\t\tTime elapsed: %f ms\n", time_elapsed); // Print the time elapsed for the operation
    }

    return 0;
}

void intialiseArray(int array[N][N]) {
	printf("intialising array... "); // Print message indicating initialization of the array
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			array[i][j] = rand() % ((10 - 1) + 1) + 1; // Assign random values to the array elements
		}
	}
	printf("complete\n"); // Print message indicating completion of initialization
}		//intialises array with random values, uses the N global variable

void printArrays(int array[N][N]){
	printf("["); // Print opening bracket for array
	for (int i = 0; i < N; i++) {
		printf("["); // Print opening bracket for inner array
		for (int j = 0; j < N; j++) {
			printf("%i", array[i][j]); // Print array element
			printf(" ");
		}
		printf("]\n"); // Print closing bracket for inner array and move to next line
	}
	printf("]\n\n"); // Print closing bracket for array and move to next line
}		//prints array to console

void openmpMatrixMultiplication(int np, int rank, int inputArray1[N][N], int inputArray2[N][N], int outputArray[N][N]){
    long value;
    int range = N/np; // Determine the range of rows each process will handle
	int start = rank * range; // Determine the starting row for the current process
	int end = start + range; // Determine the ending row for the current process
    int buffArray[range][N]={0}; // Declare a buffer array to store intermediate results
  
#pragma omp parallel // Start OpenMP parallel region
{  
    #pragma omp for // Distribute loop iterations among OpenMP threads
    for (int i = start ; i < end ; i++) // Iterate over rows assigned to the current process
	{
		for (int j = 0; j < N; j++) // Iterate over columns
		{
			value = 0;
			for (int k = 0; k < N; k++) // Iterate over elements in the row and column
			{
				value += inputArray1[i][k] * inputArray2[k][j]; // Perform matrix multiplication
			}
            buffArray[i - start][j]=value; // Store the result in the buffer array
		}
	}
}
    MPI_Barrier(MPI_COMM_WORLD); // Synchronize all processes
    MPI_Gather(buffArray, range * N, MPI_INT, outputArray, range * N, MPI_INT, 0, MPI_COMM_WORLD); // Gather results from all processes
}
