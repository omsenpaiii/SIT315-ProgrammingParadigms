/* qSort.cpp
 *
 * required simple header file qSort.h 
 * 
 * foundation code from https://gsamaras.wordpress.com/code/quicksort-c/
 *
 * to make for ComplexThreading.cpp:

$g++ -c qSort.cpp

*/

#include <iostream>
#include "./qSort.h"

using namespace std;

void swap(int& a, int& b)
{
    int temp = a;
    a = b;
    b = temp;
}

int pivot(int array[], int first, int last) 
{
    int  p = first;
    int pivotElement = array[first];
 
    for(int i = first+1 ; i <= last ; i++)
    {
        if(array[i] <= pivotElement)
        {
            p++;
            swap(array[i], array[p]);
        }
    }
 
    swap(array[p], array[first]);
 
    return p;
}

void quickSort( int array[], int first, int last ) 
{
    int pivotElement;
 
    if(first < last)
    {
        pivotElement = pivot(array, first, last);
        quickSort(array, first, pivotElement-1);
        quickSort(array, pivotElement+1, last);
    }
}