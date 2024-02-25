# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#CFLAGS  = -g -Wall

# the build target executable:
#TARGET = myprog

default: all

all: qSort.o
	g++ -fopenmp ComplexThreading.cpp qSort.o

qSort.o:
	g++ -c qSort.cpp

run:
	./a.out

clean:
	rm ./a.out *.o
