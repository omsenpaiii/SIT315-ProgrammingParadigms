/* ComplexThreading.cpp
 * Om Tomar
 * 25/02/2023
 * 
 * This program generates a random array of LENGTH and sorts it seqentualy and in parallel
 * this program requires qSort.o to compile, makefile is included, use the comman below for simplest build and run
g++ -fopenmp ComplexThreading.cpp qSort.o
*/

#include "qSort.h"
#include <iostream>
#include <random>
#include <sys/time.h>
#include <omp.h>
#include <pthread.h> 

using namespace std;

#define LENGTH 10  //60000
int Array[LENGTH];
int NUM_THREADS = 2;
int P;

void initArray(int array[LENGTH]){
	srand (time(NULL));
	cout<<"Using Array of size: "<<LENGTH<<endl;
	cout<<"Initialising Array with random values...\t";
	for (int i = 0 ; i < LENGTH ; i++){
		array[i] = rand() % ((100 - 1) + 1) + 1;
	}
	cout<<"Done."<<endl;

}			//intialises array with random values

void printArray(int array[LENGTH]){
	cout<<"[";
	for (int i = 0 ; i < LENGTH ; i++){
		cout<<array[i]<<", ";
	}
	cout<<"]\n";
}			//prints array to the console

void openmpQuickSort( int array[], int first, int last ) 
{
    int pivotElement;
 
    if(first < last)
    {
        pivotElement = pivot(array, first, last);
        //#pragma omp parallel sections
        //{
        //	#pragma omp section
        	quickSort(array, first, pivotElement-1);
        	//cout<<"1"<<endl;

        	//#pragma omp section
        	quickSort(array, pivotElement+1, last);
        	//cout<<"2"<<endl;
    	//}
    }
}		//non functioning OpenMP attempt causes compiler error

void* pthread_QuickSort(void *threadid){

	int pivotElement;

	long tid = (long)threadid;
 	int range = LENGTH/NUM_THREADS;
	
 	int first = tid * range;;
 	int last = first + range - 1;

 	if(tid == 0){

		last = P - 1 ;

	    if(first < last)
	    {
	        pivotElement = pivot(Array, first, last);
	        quickSort(Array, first, pivotElement-1);
	        quickSort(Array, pivotElement+1, last);
	    }
	}else{

		first = P;

	    if(first < last)
	    {
	        pivotElement = pivot(Array, first, last);
	        quickSort(Array, first, pivotElement-1);
	        quickSort(Array, pivotElement+1, last);
	    }
	}
}		//Generates values for the threads to begin the recursion


void pthreadQuickSort(){

	pthread_t threads[NUM_THREADS];

    	P = pivot(Array, 0, LENGTH-1);			//global P

		for (long tid = 0; tid < NUM_THREADS; tid++) 
			pthread_create(&threads[tid], NULL, pthread_QuickSort, (void *)tid);

		for (long tid = 0; tid < NUM_THREADS; tid++) 
			pthread_join(threads[tid], NULL); 
}		// identification of the piviot and creation of the thread

int main(int argc, char *argv[]){

	struct timeval timecheck;
	//NUM_THREADS = atoi(argv[1]);

	initArray(Array);
	//printArray(Array);

	cout<<"Sequential QuickSort.\t\t\t\tTime elapsed: ";

	gettimeofday(&timecheck, NULL);

	long timeofday_start = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec /1000;

	quickSort(Array, 0, LENGTH-1);
	//printArray(Array);

	gettimeofday(&timecheck, NULL);
	long timeofday_end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec /1000;

	double time_elapsed = timeofday_end - timeofday_start;
	cout<<time_elapsed<<"ms"<<endl;

	
	initArray(Array);
	printArray(Array);
	cout<<"pthread QuickSort.\t\t\t\tTime elapsed: ";

	gettimeofday(&timecheck, NULL);

	timeofday_start = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec /1000;

	//pthread_QuickSort((void*)NULL);
	pthreadQuickSort();
	

	gettimeofday(&timecheck, NULL);
	timeofday_end = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec /1000;

	time_elapsed = timeofday_end - timeofday_start;
	cout<<time_elapsed<<"ms"<<endl;

	printArray(Array);

	return 0;
}
