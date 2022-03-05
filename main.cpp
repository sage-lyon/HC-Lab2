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

// OpenCL runtime configuration
static cl_platform_id platform = NULL;
static cl_device_id device = NULL;
static cl_context context = NULL;
static cl_command_queue queue = NULL;
static cl_kernel kernel = NULL;
static cl_program program = NULL;

void cleanup();

int main() {
    cl_int status;

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

    // Enqueue and launch kernel
    status = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, gSize, lSize, 0, NULL, NULL);

}
