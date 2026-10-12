// 1) general header file declrations 
#include <stdio.h> 

// 2) CUDA header file declrations 
#include <cuda.h> 

// 3) global variables 
// host arrays 
float* hostInput1 = NULL; 
float* hostInput2 = NULL; 
float* hostOutput = NULL; 

// device arrays 
float* deviceInput1 = NULL; 
float* deviceInput2 = NULL; 
float* deviceOutput = NULL; 

const float iNumberOfArrayElements = 5; 

// 4) cuda kernel definition 
__global__ void vecAddGPU(float* in1, float* in2, float* output, int length) 
{
    // code 
    int i = blockIdx.x * blockDim.x + threadIdx.x; 

    if(i < length) 
    {
        output[i] = in1[i] + in2[i]; 
    } 
} 

// 5) entry point function 
int main(void) 
{
    // function declarations 
    void cleanup(void); 

    // code 
    int size = iNumberOfArrayElements * sizeof(float); 
    cudaError_t result = cudaSuccess; 

    // 6) allocating memory for host input 1 
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
    result = cudaMalloc((void**)&deviceInput1, size);  
    if(result != cudaSuccess) 
    {
        printf("Failed to allocate memory for deviceInput1 array\n"); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    result = cudaMalloc((void**)&deviceInput2, size);  
    if(result != cudaSuccess) 
    {
        printf("Failed to allocate memory for deviceInput2 array\n"); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    result = cudaMalloc((void**)&deviceOutput, size);  
    if(result != cudaSuccess) 
    {
        printf("Failed to allocate memory for deviceOutput array\n"); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    // 10) copy data from host arrays to device arrays 
    result = cudaMemcpy(deviceInput1, hostInput1, size, cudaMemcpyHostToDevice); 
    if(result != cudaSuccess) 
    {
        printf("Host to device memory copying failed for deviceInput1 array.\n"); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    result = cudaMemcpy(deviceInput2, hostInput2, size, cudaMemcpyHostToDevice); 
    if(result != cudaSuccess) 
    {
        printf("Host to device memory copying failed for deviceInput2 array.\n"); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    // grid and block config 
    dim3 dimGrid = dim3(iNumberOfArrayElements, 1, 1); 
    dim3 dimBlock = dim3(1, 1, 1);  
    
    // call to CUDA kernel 
    vecAddGPU<<<dimGrid, dimBlock>>>(deviceInput1, deviceInput2, deviceOutput, iNumberOfArrayElements); 
    
    // 13) copy device output to host output 
    result = cudaMemcpy(hostOutput, deviceOutput, size, cudaMemcpyDeviceToHost); 
    if(result != cudaSuccess) 
    {
        printf("Device to host data copy for HostOutput array failed.\n"); 
        exit(EXIT_FAILURE); 
    } 

    // 14) display of result 
    for(int i = 0; i < iNumberOfArrayElements; ++i) 
    {
        printf("%f + %f = %f\n", hostInput1[i], hostInput2[i], hostOutput[i]); 
    } 

    // call to cleanup function  
    cleanup(); 
 
    return (0); 
}

// definition of cleanup function 
void cleanup(void) 
{
    // code 
    if(deviceOutput) 
    {
        cudaFree(deviceOutput); 
        deviceOutput = NULL; 
    } 

    if(deviceInput2) 
    {
        cudaFree(deviceInput2);  
        deviceInput2 = NULL;  
    } 

    if(deviceInput1) 
    {
        cudaFree(deviceInput1); 
        deviceInput1 = NULL; 
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
