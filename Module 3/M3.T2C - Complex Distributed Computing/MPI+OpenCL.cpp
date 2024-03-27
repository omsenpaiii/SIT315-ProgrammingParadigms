#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <CL/cl.hpp>

#define MAX 1000000
#define WORKGROUP_SIZE 256
#define LOCAL_SIZE WORKGROUP_SIZE

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Initialize the random seed using the current time
    srand(time(NULL) + rank);

    // Determine the local portion of the array to be summed
    int local_size = MAX / size;
    int local_data[local_size];
    for (int i = 0; i < local_size; i++)
    {
        local_data[i] = rand() % 20;
    }

    // Create OpenCL context and command queue
    cl::Context context(CL_DEVICE_TYPE_GPU);
    std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
    cl::CommandQueue queue(context, devices[0]);

    // Create OpenCL buffers for the input and output arrays
    cl::Buffer input_buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, local_size * sizeof(int), local_data);
    cl::Buffer output_buffer(context, CL_MEM_WRITE_ONLY, sizeof(int));

    // Create OpenCL program and kernel
    cl::Program program(context, "#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable\n __kernel void sum(__global int *input, __global int *output, const int local_size) { \n\
        int sum = 0; \n\
        __global int *data = input + get_global_id(0) * local_size; \n\
        for (int i = 0; i < local_size; i++) { \n\
            sum += data[i]; \n\
        } \n\
        atom_add(output, sum); \n\
    }");
    program.build(devices);
    cl::Kernel kernel(program, "sum");

    // Set kernel arguments
    kernel.setArg(0, input_buffer);
    kernel.setArg(1, output_buffer);
    kernel.setArg(2, local_size);

    // Execute the kernel
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(local_size / WORKGROUP_SIZE), cl::NDRange(WORKGROUP_SIZE));

    // Read the output buffer
    int local_sum;
    queue.enqueueReadBuffer(output_buffer, CL_TRUE, 0, sizeof(int), &local_sum);

    // Combine the local sums from each process using MPI_Reduce
    long global_sum;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    // Output the final sum and execution time of the summation
    if (rank == 0)
    {
        std::cout << "The final sum = " << global_sum << std::endl;
    }

    MPI_Finalize();
    return 0;
}
