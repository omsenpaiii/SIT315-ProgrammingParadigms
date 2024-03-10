/* OpenMP MPI
 * Om Tomar
 * 09/03/24
 * 
 * Compile with 
 * $ mpicxx openmpMPI.cpp -fopenmp
 * 
 * Run
 * $ mpirun -np 4 --hostfile ~/Desktop/Slave.list
 * $ mpirun -np 4 ./a.out
 * 
 */

#include<mpi.h>
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/time.h>
#include<time.h>
#include<omp.h>

#define N 800

using namespace std;

void intialiseArray(int array[N][N]);
void printArrays(int array[N][N]);
void openmpMatrixMultiplication(int np, int rank, int inputArray1[N][N], int inputArray2[N][N], int outputArray[N][N]);
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
    MPI_Comm_size(MPI_COMM_WORLD, &np);     //number of nodes

    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int inputArray1[N][N], inputArray2[N][N];
    int outputArray[N][N]={{0}};
    //int outputArray[N*N]={0};

    if (rank==0) {
        intialiseArray(inputArray1);
        intialiseArray(inputArray2);

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
    MPI_Bcast(&inputArray1, N*N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&inputArray2, N*N, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    openmpMatrixMultiplication(np, rank, inputArray1, inputArray2, outputArray);
    MPI_Barrier(MPI_COMM_WORLD);
    
    //if(rank==0)printArrays(outputArray);
    
    MPI_Finalize();

    //Timer end
    gettimeofday(&timecheck, NULL);
	long timeofday_end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec /1000;
	double time_elapsed = timeofday_end - timeofday_start;
	
    if (rank == 0){
        printf("\t\tTime elapsed: %f ms\n", time_elapsed);
    }

    return 0;
}

void intialiseArray(int array[N][N]) {
	printf("intialising array... ");
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			array[i][j] = rand() % ((10 - 1) + 1) + 1;
		}
	}
	printf("complete\n");
}		//intialises array with random values, uses the N global variable

void printArrays(int array[N][N]){
	printf("[");
	for (int i = 0; i < N; i++) {
		printf("[");
		for (int j = 0; j < N; j++) {
			printf("%i", array[i][j]);
			printf(" ");
		}
		printf("]\n");
	}
	printf("]\n\n");
}		//prints array to console

void openmpMatrixMultiplication(int np, int rank, int inputArray1[N][N], int inputArray2[N][N], int outputArray[N][N]){
    long value;
    int range = N/np;
	int start = rank * range;
	int end = start + range;
    int buffArray[range][N]={0};
  
#pragma omp parallel
{  
    #pragma omp for
    for (int i = start ; i < end ; i++)
	{
		for (int j = 0; j < N; j++)
		{
			value = 0;
			for (int k = 0; k < N; k++)
			{
				value += inputArray1[i][k] * inputArray2[k][j];
			}
            buffArray[i - start][j]=value;
		}
	}
}
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(buffArray, range * N, MPI_INT, outputArray, range * N, MPI_INT, 0, MPI_COMM_WORLD);
}
