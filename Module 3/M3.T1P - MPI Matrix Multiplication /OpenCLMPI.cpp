/* OpenCL MPI
 * Om Tomar
 * 09/03/24
 * 
 * Compile with 
 * $ mpicxx openclMPI.cpp -fopenmp -lOpenCL
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
#include<CL/cl.h>

#define N 800

using namespace std;

void intialiseArray(int array[N][N]);
void printArrays(int array[N][N]);
void openclMatrixMultiplication(int np, int rank, int inputArray1[N][N], int inputArray2[N][N], int outputArray[N][N]);
//void MatrixMultiplication(int np, int rank, int inputArray1[N][N], int inputArray2[N][N], int outputArray[N*N]);
struct timeval timecheck;

cl_device_id device_id;
cl_context context;
cl_program program;
cl_kernel kernel;
cl_command_queue  queue;

cl_event event = NULL;
int err;

int a[N][N], b[N][N], c[N][N];
cl_mem bufA, bufB, bufC;
const int max = N;
    const int TS = 4;
    const size_t local[2] = { TS, TS };
    const size_t global[2] = { max, max }; 
void init (int a[N][N]);
void matrix_mul(int a[N][N], int b[N][N], int c[N][N]) ;
void print_matrix(int a[N][N]) ;

cl_device_id create_device();
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename);

void setup_openCL_device_context_queue_kernel();
void setup_kernel_memory();
void copy_kernel_args();
void free_memory();

void printOutput(int outputArray[N]){
    for (int i = 0 ; i < N; i++){
        printf(" %d :", outputArray[i]);
    }
    
}


int main(){

    init(a);
    init(b);
    matrix_mul(a,b,c);
    //print_matrix(c);

    setup_openCL_device_context_queue_kernel();
    setup_kernel_memory();
    copy_kernel_args();

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
        printf("OpenCL MPI Matrix Multiplication.\n");
    }

    gettimeofday(&timecheck, NULL);
	long timeofday_start = (long)timecheck.tv_sec * 1000 + (long)timecheck.tv_usec /1000;

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&inputArray1, N*N, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&inputArray2, N*N, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    openclMatrixMultiplication(np, rank, inputArray1, inputArray2, outputArray);
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

void openclMatrixMultiplication(int np, int rank, int inputArray1[N][N], int inputArray2[N][N], int outputArray[N][N]){
    long value;
    int range = N/np;
	int start = rank * range;
	int end = start + range;
    int buffArray[range][N]={0};
  
    clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global, local, 0, NULL, &event);
    clWaitForEvents(1, &event);

    //copying data from the device back to host c matrix
    clEnqueueReadBuffer(queue, bufC, CL_TRUE, 0, N * N*sizeof(int), c, 0, NULL, NULL);
    //print_matrix(c);
   
    free_memory();

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Gather(buffArray, range * N, MPI_INT, outputArray, range * N, MPI_INT, 0, MPI_COMM_WORLD);
}

void free_memory() {
   
   clReleaseKernel(kernel);
   clReleaseMemObject(bufA);
    clReleaseMemObject(bufB);
    clReleaseMemObject(bufC);

   clReleaseCommandQueue(queue);
   clReleaseProgram(program);
   clReleaseContext(context);
}
void copy_kernel_args() {
   clSetKernelArg(kernel, 0, sizeof(int), (void*)&max);
    clSetKernelArg(kernel, 1, sizeof(int), (void*)&max);
    clSetKernelArg(kernel, 2, sizeof(int), (void*)&max);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void*)&bufA);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void*)&bufB);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void*)&bufC);
    if(err < 0) {
      perror("Couldn't create a kernel argument");
      printf("error = %d", err);
      exit(1);
   }
}
void setup_kernel_memory() {
     bufA = clCreateBuffer(context, CL_MEM_READ_ONLY,  N*N*sizeof(int), NULL, NULL);
     bufB = clCreateBuffer(context, CL_MEM_READ_ONLY,  N*N*sizeof(int), NULL, NULL);
     bufC = clCreateBuffer(context, CL_MEM_READ_WRITE, N*N*sizeof(int), NULL, NULL);

    // Copy matrices to the GPU
    clEnqueueWriteBuffer(queue, bufA, CL_TRUE, 0, N*N*sizeof(int), a, 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufB, CL_TRUE, 0, N*N*sizeof(int), b, 0, NULL, NULL);
    clEnqueueWriteBuffer(queue, bufC, CL_TRUE, 0, N*N*sizeof(int), c, 0, NULL, NULL);

}

void setup_openCL_device_context_queue_kernel() {
    device_id = create_device();
    cl_int err;
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
   if(err < 0) {
      perror("Couldn't create a context");
      exit(1);   
   }

     program = build_program(context, device_id, "matrix_mul.cl");

   

      queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    if(err < 0) {
      perror("Couldn't create a command queue");
      exit(1);   
    };

    kernel = clCreateKernel(program, "multiply_matrices", &err);
   if(err < 0) {
      perror("Couldn't create a kernel");
      printf("error =%d", err);
      exit(1);
   };

}
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;
  

   /* Read program file and place content into buffer */
   program_handle = fopen(filename, "r");
   if(program_handle == NULL) {
      perror("Couldn't find the program file");
      exit(1);
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size + 1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);

   /* Create program from file 

   Creates a program from the source code in the add_numbers.cl file. 
   Specifically, the code reads the file's content into a char array 
   called program_buffer, and then calls clCreateProgramWithSource.
   */
   program = clCreateProgramWithSource(ctx, 1, 
      (const char**)&program_buffer, &program_size, &err);
   if(err < 0) {
      perror("Couldn't create the program");
      exit(1);
   }
   free(program_buffer);

   /* Build program 

   The fourth parameter accepts options that configure the compilation. 
   These are similar to the flags used by gcc. For example, you can 
   define a macro with the option -DMACRO=VALUE and turn off optimization 
   with -cl-opt-disable.
   */
   err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
   if(err < 0) {

      /* Find size of log and print to std output */
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            0, NULL, &log_size);
      program_log = (char*) malloc(log_size + 1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            log_size + 1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }

   return program;
}

    cl_device_id create_device() {

    cl_platform_id platform;
    cl_device_id dev;
    int err;

    /* Identify a platform */
    err = clGetPlatformIDs(1, &platform, NULL);
    if(err < 0) {
       perror("Couldn't identify a platform");
       exit(1);
    } 

   // Access a device
   // GPU
   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      // CPU
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
   }
   if(err < 0) {
      perror("Couldn't access any devices");
      exit(1);   
   }

   return dev;
}


void init (int a[N][N]) {
    for(int i=0; i < N; i++) {
        int add = 0;
       for(int j=0; j < N; j++) {
           a[i][j] = rand() % 10 ;//+ add;
           add += 10;
        } 
    }
}

void matrix_mul(int a[N][N], int b[N][N], int c[N][N]) {
    for(int i=0; i < N; i++) {
       
       for(int j=0; j < N; j++) {
            int val = 0;
           for(int k=0; k < N; k++) {
            val += a[i][k] * b[k][j]; 
            }
            c[i][j] = val; 
        } 
    }
}
void print_matrix(int a[N][N]) 
{
    for(int i=0; i < N; i++) {
       for(int j=0; j < N; j++) {
           printf("%d ", a[i][j]);
        } 
        printf("\n");
    }
}
