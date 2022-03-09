#include <CL/cl.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cstring>
#include "CL/opencl.h"
#include "AOCLUtils/aocl_utils.h"

using namespace aocl_utils;

#define STRING_BUFFER_LEN 1024

#define LOCAL_SIZE 0
#define WORKGROUPS 0

void cleanup();

int main() {

    // OpenCL runtime configuration
    cl_platform_id platform = NULL;
    cl_device_id device = NULL;
    cl_context context = NULL;
    cl_command_queue queue = NULL;
    cl_kernel kernel = NULL;
    cl_program program = NULL;

    cl_int status;

    double result;

    // Find FPGA Emulator platform
    platform = findPlatform("Intel(R) FPGA Emulation Platform for OpenCL(TM)");

    if(platform == NULL) {
        printf("ERROR: Cannot find platform\n");
        return -1;
    }

    // Find available OpenCL device
    cl_uint num_devices;
    getDevices(platform, CL_DEVICE_TYPE_ALL, &num_devices);
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_DEFAULT, 1, &device, &num_devices);
    checkError(status, "Unable to get device ID");

    // Create context
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &status);
    checkError(status, "Unable to create context");

    // Create command queue
    queue = clCreateCommandQueue(context, device, 0, &status);
    checkError(status, "Unable to create command queue");

    // Create program
    std::string binary_file = getBoardBinaryFile("picalc", device);
    program = createProgramFromBinary(context, binary_file.c_str(), &device, 1);

    // Build program
    status = clBuildProgram(program, 0, NULL, "", NULL, NULL);
    checkError(status, "Unable to build program");

    // Create kernel
    const char* kernel_name = "picalc";
    kernel = clCreateKernel(program, kernel_name, &status);
    checkError(status, "Unable to create kernel");

    // Set kernel arguments 
    int operands_per_item = 16;
    cl_mem result_buffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(result), result, NULL);
    
    status = clSetKernelArg(kernel, 0, sizeof(operands_per_item), &operands_per_item);
    status |= clSetKernelArg(kernel, 1, LOCAL_SIZE * sizeof(double), NULL);
    status |= clSetKernelArg(kernel, 2, WORKGROUPS * sizeof(double), NULL);
    status |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &result_buffer);
    checkError(status, "Unable to set kernel arguments");

    // Enqueue and launch kernel
    size_t gSize[3] = {WORKGROUPS * LOCAL_SIZE, 1 , 1};
    size_t lSize[3] = {LOCAL_SIZE, 1 , 1};
    status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, gSize, lSize, 0, NULL, NULL);
    checkError(status, "Unable to launch kernel");

    // Get results from kernel
    ret = clEnqueueReadBuffer(queue, result_buffer, CL_TRUE, 0, sizeof(result), &result, 0, NULL, NULL);

    // Print result
    printf("Result: %lf\n", result);

    return 0;

}
