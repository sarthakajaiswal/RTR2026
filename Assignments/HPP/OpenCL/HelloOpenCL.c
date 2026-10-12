#include <stdio.h> 
#include <stdlib.h> 

// OpenCL header
// #define CL_TARGET_OPENCL_VERSION    300// define only if version warning comes 
#include <CL/opencl.h> 

// global variables 
const int iNumberOfArrayElements = 5; 

// opencl specific variables 
cl_platform_id oclPlatformID; 
cl_device_id oclDeviceID; 

cl_context oclContext = NULL; 
cl_command_queue oclCommandQueue; 

cl_program oclProgram; 
cl_kernel oclKernel; 

// host arrays 
float* hostInput1 = NULL; 
float* hostInput2 = NULL; 
float* hostOutput = NULL; 

// device arrays 
cl_mem deviceInput1 = NULL; 
cl_mem deviceInput2 = NULL; 
cl_mem deviceOutput = NULL; 

// opencl kernel 
const char* oclSourceCode = 
"__kernel void vecAddGPU(__global float* in1, __global float *in2, __global float* out, int len)" \
"{" \
"int i = get_global_id(0);" \
"if(i < len)" \
"{" \
"out[i] = in1[i] + in2[i];" \
"}" \
"}"; 

// entry point function 
int main(void) 
{
    // function declarations 
    void cleanup(void); 

    // variable declarations 
    int size = iNumberOfArrayElements * sizeof(float); 
    cl_int result = CL_SUCCESS; 

    hostInput1 = (float*)malloc(size); 
    if(hostInput1 == NULL) 
    {
        printf("Allocating memory failed is failed for histInput1 array.\n"); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    hostInput2 = (float*)malloc(size); 
    if(hostInput2 == NULL) 
    {
        printf("Allocating memory failed is failed for histInput1 array.\n"); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    hostOutput = (float*)malloc(size); 
    if(hostOutput == NULL) 
    {
        printf("Allocating memory failed is failed for histInput1 array.\n"); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    // filling values into float arrays 
    hostInput1[0] = 101.0f; 
    hostInput1[1] = 102.0f; 
    hostInput1[2] = 103.0f; 
    hostInput1[3] = 104.0f; 
    hostInput1[4] = 105.0f; 

    hostInput2[0] = 201.0f; 
    hostInput2[1] = 202.0f; 
    hostInput2[2] = 203.0f; 
    hostInput2[3] = 204.0f; 
    hostInput2[4] = 205.0f; 

    // 9) device memory allocation 
    // A) get platform ID 
    result = clGetPlatformIDs(1, &oclPlatformID, NULL); 
    if(result != CL_SUCCESS) 
    {
        printf("clGetPlatformIDs() failed: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    // B) get device ID 
    result = clGetDeviceIDs(oclPlatformID, CL_DEVICE_TYPE_GPU, 1, &oclDeviceID, NULL); 
    if(result != CL_SUCCESS) 
    {
        printf("clGetDeviceIDs() failed: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    // C) create opencl context 
    oclContext = clCreateContext(NULL, 1, &oclDeviceID, NULL, NULL, &result); 
    if(result != CL_SUCCESS) 
    {
        printf("clCreateContext() failed: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    // D) create command queue 
    oclCommandQueue = clCreateCommandQueue(oclContext, oclDeviceID, 0, &result); 
    if(result != CL_SUCCESS) 
    {
        printf("clCreateCommandQueue() failed: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    // E) create opencl program 
    oclProgram = clCreateProgramWithSource(oclContext, 1, (const char**)&oclSourceCode, NULL, &result); 
    if(result != CL_SUCCESS) 
    {
        printf("clCreateProgramWithSource() failed: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    // F) build OCL program 
    result = clBuildProgram(oclProgram, 0, NULL, NULL, NULL, NULL); 
    if(result != CL_SUCCESS) 
    {
        size_t len; 
        char buffer[2048]; 
        clGetProgramBuildInfo(oclProgram, oclDeviceID, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len); 
        printf("Program build log : %s\n", buffer); 
        printf("clBuildProgram() failed: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    // G) create ocl kernel 
    oclKernel = clCreateKernel(oclProgram, "vecAddGPU", &result); 
    if(result != CL_SUCCESS) 
    {
        printf("clCreateKernel() failed: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    // device memory allocation 
    deviceInput1 = clCreateBuffer(oclContext, CL_MEM_READ_ONLY, size, NULL, &result); 
    if(result != CL_SUCCESS) 
    {
        printf("clCreateBuffer() failed for 1st input array: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    deviceInput2 = clCreateBuffer(oclContext, CL_MEM_READ_ONLY, size, NULL, &result); 
    if(result != CL_SUCCESS) 
    {
        printf("clCreateBuffer() failed for 2nd input array: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    deviceOutput = clCreateBuffer(oclContext, CL_MEM_WRITE_ONLY, size, NULL, &result); 
    if(result != CL_SUCCESS) 
    {
        printf("clCreateBuffer() failed for output array: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    result = clSetKernelArg(oclKernel, 0, sizeof(cl_mem), (void *)&deviceInput1); 
    if(result != CL_SUCCESS) 
    {
        printf("clSetKernelArg() failed for 1st argument: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    }  

    result = clSetKernelArg(oclKernel, 1, sizeof(cl_mem), (void *)&deviceInput2); 
    if(result != CL_SUCCESS) 
    {
        printf("clSetKernelArg() failed for 2nd argument: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    }  

    result = clSetKernelArg(oclKernel, 2, sizeof(cl_mem), (void *)&deviceOutput); 
    if(result != CL_SUCCESS) 
    {
        printf("clSetKernelArg() failed for 3rd argument: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    }  

    result = clSetKernelArg(oclKernel, 3, sizeof(cl_int), (void *)&iNumberOfArrayElements); 
    if(result != CL_SUCCESS) 
    {
        printf("clSetKernelArg() failed for 4th argument: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    }  

    // write hostInput1 to deviceInput1 
    result = clEnqueueWriteBuffer(oclCommandQueue, deviceInput1, CL_FALSE, 0, size, hostInput1, 0, NULL, NULL); 
    if(result != CL_SUCCESS) 
    {
        printf("clEnqueueWriteBuffer() failed for 1st input device buffer: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    }  

    result = clEnqueueWriteBuffer(oclCommandQueue, deviceInput2, CL_FALSE, 0, size, hostInput2, 0, NULL, NULL); 
    if(result != CL_SUCCESS) 
    {
        printf("clEnqueueWriteBuffer() failed for 2nd input device buffer: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    }  

    // 12 A) kernel configurations 
    size_t global_size = 5; 
    result = clEnqueueNDRangeKernel(oclCommandQueue, oclKernel, 1, NULL, &global_size, NULL, 0, NULL, NULL); 
    if(result != CL_SUCCESS) 
    {
        printf("clEnqueueNDRangeKernel() failed: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    }  
    
    // 12 B) enqueue finished, now start executing 
    clFinish(oclCommandQueue); 

    // 13] read back result from device 
    result = clEnqueueReadBuffer(oclCommandQueue, deviceOutput, CL_TRUE, 0, size, hostOutput, 0, NULL, NULL); 
    if(result != CL_SUCCESS) 
    {
        printf("clEnqueueReadBuffer() failed: %d\n", result); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    }  

    // 14) display of result 
    for(int i = 0; i < iNumberOfArrayElements; ++i) 
    {
        printf("%f + %f = %f\n", hostInput1[i], hostInput2[i], hostOutput[i]); 
    } 

    // 15) cleanup 
    cleanup(); 

    return (0); 
} 

void cleanup(void) 
{
    // code 
if(deviceOutput) 
    {
        clReleaseMemObject(deviceOutput); 
        deviceOutput = NULL; 
    } 

    if(deviceInput2) 
    {
        clReleaseMemObject(deviceInput2);  
        deviceInput2 = NULL;  
    } 

    if(deviceInput1) 
    {
        clReleaseMemObject(deviceInput1); 
        deviceInput1 = NULL; 
    } 

    if(oclKernel) 
    {
        clReleaseKernel(oclKernel); 
        oclKernel = NULL; 
    } 

    if(oclProgram) 
    {
        clReleaseProgram(oclProgram); 
        oclProgram = NULL; 
    } 

    if(oclCommandQueue) 
    {
        clReleaseCommandQueue(oclCommandQueue); 
        oclCommandQueue = NULL; 
    } 

    if(oclContext) 
    {
        clReleaseContext(oclContext); 
        oclContext = NULL; 
    } 

    if(hostOutput) 
    {
        free(hostOutput); 
        hostOutput = NULL; 
    } 

    if(hostInput2) 
    {
        free(hostInput2); 
        hostInput2 = NULL; 
    } 

    if(hostInput1) 
    {
        free(hostInput1); 
        hostInput1 = NULL; 
    } 
} 


