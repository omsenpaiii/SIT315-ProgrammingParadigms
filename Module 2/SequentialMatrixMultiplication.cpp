// SequentialMatrixMultiplication.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include "pch.h"
#include <iostream>
#include <random>
#include <time.h>

#define n 1000

int inputArray1[n][n];
int inputArray2[n][n];
int OutputArray[n][n];

void printArrays(int array1[n][n], int array2[n][n], int arrayOut[n][n])
{
	//std::cout << "Hello World!\n";
	std::cout << "Input Array 1\n[";

	for (int i = 0; i < n; i++) {
		std::cout << "[";
		for (int j = 0; j < n; j++) {
			std::cout << array1[i][j];
			std::cout << " ";
		}
		std::cout << "]\n";
	}
	std::cout << "]\n\n";

	std::cout << "Input Array 2\n[";

	for (int i = 0; i < n; i++) {
		std::cout << "[";
		for (int j = 0; j < n; j++) {
			std::cout << array2[i][j];
			std::cout << " ";
		}
		std::cout << "]\n";
	}
	std::cout << "]\n\n";

	std::cout << "Output Array \n[";

	for (int i = 0; i < n; i++) {
		std::cout << "[";
		for (int j = 0; j < n; j++) {
			std::cout << arrayOut[i][j];
			std::cout << " ";
		}
		std::cout << "]\n";
	}
	std::cout << "]\n\n";

}

void intialiseArray(int array[n][n]) {
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			array[i][j] = rand() % ((100 - 1) + 1) + 1;
		}
	}

}

void SequentialMatrixMultiplication(int array1[n][n], int array2[n][n], int arrayOut[n][n])
{
	int value;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			value = 0;
			for (int k = 0; k < n; k++)
			{
				value += array1[i][k] * array2[k][j];
			}
			arrayOut[i][j] = value;

		}
	}
}

int main()
{
	clock_t start, stop;
	//int n = rand() % ((100 - 1) + 1) + 1; //not allowed in C++ language
//	int inputArray1[n][n];
//	int inputArray2[n][n];
//	int OutputArray[n][n];

	for (int i = 0; i < 5; i++) {
		intialiseArray(inputArray1);
		intialiseArray(inputArray2);

		int start_s = clock();
		SequentialMatrixMultiplication(inputArray1, inputArray2, OutputArray);
		int stop_s = clock();

		//std::cout << "Runtime is: " << (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << "ms" << std::endl;

		//printArrays(inputArray1, inputArray2, OutputArray);

		std::cout << "Runtime is: " << (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << "ms" << std::endl;
	}

}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
