// 1) general header file declrations 
#include <stdio.h> 

// 2) CUDA header file declrations 
#include <cuda.h> 
#include "helper_timer.h" 

// 3) global variables 
// host arrays 
float* hostInput1 = NULL; 
float* hostInput2 = NULL; 
float* hostOutput = NULL; 
float* gold = NULL; 

// device arrays 
float* deviceInput1 = NULL; 
float* deviceInput2 = NULL; 
float* deviceOutput = NULL; 

float timeOnCPU = 0.0f; 
float timeOnGPU = 0.0f; 

// const float iNumberOfArrayElements = 5; 
const int iNumberOfArrayElements = 11444777; 

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
    void fillFloatArrayWithRandomNumbers(float*, int); 
    void vecAddHost(const float*, const float*, float*, int); 
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

    gold = (float*)malloc(size); 
    if(gold == NULL) 
    {
        printf("Host memory allocation is failed for gold array\n"); 
        cleanup(); 
        exit(EXIT_FAILURE); 
    } 

    // filling values into float arrays 
    fillFloatArrayWithRandomNumbers(hostInput1, iNumberOfArrayElements); 
    fillFloatArrayWithRandomNumbers(hostInput2, iNumberOfArrayElements); 

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
    dim3 dimGrid = dim3((int)ceil((float)iNumberOfArrayElements/256.0f), 1, 1); 
    dim3 dimBlock = dim3(256, 1, 1);  

    StopWatchInterface* timer = NULL; 
    sdkCreateTimer(&timer); 
    sdkStartTimer(&timer); 

    // call to CUDA kernel 
    vecAddGPU<<<dimGrid, dimBlock>>>(deviceInput1, deviceInput2, deviceOutput, iNumberOfArrayElements); 
    
    sdkStopTimer(&timer); 
    timeOnGPU = sdkGetTimerValue(&timer); 
    sdkDeleteTimer(&timer); 
    timer = NULL; 

    // 13) copy device output to host output 
    result = cudaMemcpy(hostOutput, deviceOutput, size, cudaMemcpyDeviceToHost); 
    if(result != cudaSuccess) 
    {
        printf("Device to host data copy for HostOutput array failed.\n"); 
        exit(EXIT_FAILURE); 
    } 

    // vector addition on host 
    vecAddHost(hostInput1, hostInput2, gold, iNumberOfArrayElements); 

    // accuracy checking 
    const float epsilon = 0.000001f; 
    int breakValue = -1; 
    bool bAccuracy = true; 

    for(int i = 0; i < iNumberOfArrayElements; ++i) 
    {
        float val1 = hostOutput[i]; 
        float val2 = gold[i]; 
        if(fabs(val1-val2) > epsilon) 
        {
            bAccuracy = false; 
            breakValue = i; 
            break; 
        } 
    }
    
    char str[128]; 
    if (bAccuracy == false) 
        sprintf(str, "Comparison of CPU and GPU Vector Addition is not within accuracy of 0.000001 at array index %d\n", breakValue);
    else
        sprintf(str, "Comparison of CPU and GPU Vector Addition is within accuracy of 0.000001 at array index %d\n", breakValue);

    // output
    printf("Array1 begins from 0th index %.6f to %dth index %.6f\n", hostInput1[0], iNumberOfArrayElements - 1, hostInput1[iNumberOfArrayElements-1]); 
    printf("Array2 begins from 0th index %.6f to %dth index %.6f\n", hostInput2[0], iNumberOfArrayElements - 1, hostInput2[iNumberOfArrayElements-1]); 
    printf("CUDA kernel Grid dimention = %d, %d, %d and block dimention = %d, %d, %d\n", dimGrid.x, dimGrid.y, dimGrid.z, dimBlock.x, dimBlock.y, dimBlock.z); 
    printf("Output begins from 0th index %.6f to %dth index %.6f\n", hostOutput[0], iNumberOfArrayElements - 1, hostOutput[iNumberOfArrayElements-1]); 
    printf("Time taken for vector addition on CPU = %.6f\n", timeOnCPU); 
    printf("Time taken for vector addition on GPU = %.6f\n", timeOnGPU); 
    printf("%s\n", str); 

    // call to cleanup function  
    cleanup(); 
 
    return (0); 
}

void fillFloatArrayWithRandomNumbers(float* array, int length) 
{
    // code 
    const float fscale = 1.0f / (float)RAND_MAX; 
    for(int i = 0; i < length; ++i) 
    {
        array[i] = fscale * rand(); 
    } 
} 

void vecAddHost(const float* arr1, const float* arr2, float* out, int len) 
{
    // code 
    StopWatchInterface* timer = NULL; 
    sdkCreateTimer(&timer); 
    sdkStartTimer(&timer); 

    for(int i = 0; i < len; ++i) 
    {
        out[i] = arr1[i] + arr2[i]; 
    } 

    sdkStopTimer(&timer); 
    timeOnCPU = sdkGetTimerValue(&timer); 
    sdkDeleteTimer(&timer); 
    timer = NULL; 
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

    if(gold) 
    {
        free(gold); 
        gold = NULL; 
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
