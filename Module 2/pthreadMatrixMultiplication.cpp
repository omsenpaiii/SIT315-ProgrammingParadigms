// pthreadMatrixMultiplication.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <random>
#include <time.h>
#include <pthread.h>

#define NUM_THREADS 5

using namespace std;

const int n = 100;

void printArrays(int array1[n][n], int array2[n][n], int arrayOut[n][n])
{
	cout << "Input Array 1\n[";

	for (int i = 0; i < n; i++) {
		cout << "[";
		for (int j = 0; j < n; j++) {
			cout << array1[i][j];
			cout << " ";
		}
		cout << "]\n";
	}
	cout << "]\n\n";

	cout << "Input Array 2\n[";

	for (int i = 0; i < n; i++) {
		cout << "[";
		for (int j = 0; j < n; j++) {
			cout << array2[i][j];
			cout << " ";
		}
		cout << "]\n";
	}
	cout << "]\n\n";

	cout << "Output Array \n[";

	for (int i = 0; i < n; i++) {
		cout << "[";
		for (int j = 0; j < n; j++) {
			cout << arrayOut[i][j];
			cout << " ";
		}
		cout << "]\n";
	}
	cout << "]\n\n";
}		//Prints the three arrays, In1 in2 and out

void intialiseArray(int array[n][n]) {
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			array[i][j] = rand() % ((100 - 1) + 1) + 1;
		}
	}

}		//intialises a random array of n x n size

int calculateCellValue(int i, int j, int array1[n][n], int array2[n][n])
{
                        int value = 0;
                        for (int k = 0; k < n; k++)
                        {
                                value += array1[i][k] * array2[k][j];
                        }
                        return value;
}		//perform the calculation for a particualar cell

void SequentialMatrixMultiplication(int array1[n][n], int array2[n][n], int arrayOut[n][n])
{
	int value;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			arrayOut[i][j] = calculateCellValue(i, j, array1, array2);
		}
	}
}		//performs the iteration throught the arrays

int main()
{
	//int n = rand() % ((100 - 1) + 1) + 1; //not allowed in C++ language
	
	pthread_t threads[NUM_THREADS];

	int inputArray1[n][n];
	int inputArray2[n][n];
	int OutputArray[n][n];

	cout << "pthread using " << NUM_THREADS << " threads" << endl;

	for (int i = 0; i < 5; i++) {
		intialiseArray(inputArray1);
		intialiseArray(inputArray2);

		int start_s = clock();
		SequentialMatrixMultiplication(inputArray1, inputArray2, OutputArray);
		int stop_s = clock();

		//printArrays(inputArray1, inputArray2, OutputArray);
		cout << (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << endl;
	}
}

// g++ <file.cpp>
// g++ -o <name> <file.cpp>
