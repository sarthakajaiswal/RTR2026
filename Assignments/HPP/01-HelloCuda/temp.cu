#include<stdio.h>

#include<cuda.h>
#include "helper_timer.h"

//global variables
//host arrays
float* hostInput1=NULL;
float* hostInput2=NULL;
float* hostOutput=NULL;
float* gold=NULL;

//device arrays
float* deviceInput1=NULL;
float* deviceInput2=NULL;
float* deviceOutput=NULL;

const int iNumberOfArrayElements=11444777;

float timeOnCPU = 0.0f;
float timeOnGPU = 0.0f;


//cuda kernel defination
__global__ void vecAddGPU(float* in1,float* in2,float* out,int len)
{
	//code
	int i=blockIdx.x * blockDim.x + threadIdx.x;

	if(i < len)
	{
		out[i] = in1[i]+in2[i];
	}
}

//entry point fucntion
int main(void)
{
	//funtion declaration
	void cleanup(void);
	void fillFloatArrayWithRandomNumbers(float*,int);
	void vecAddCPU(const float*,const float*,float*,int);

	//variable declarations
	int size= iNumberOfArrayElements * sizeof(float);//pudha jaun 5 float bharayche aahet so prateyka block madhe 5 float yevdhi memory pahije
	cudaError_t result = cudaSuccess;//cuda error type cha variable banavlay jya madhe cuda error aahe ka dakhvta

	//code

	//host memory allocation
	hostInput1 = (float*)malloc(size);

	if(hostInput1==NULL)
	{
		printf("Host Memory Allocation is failed for hostInput1 array.\n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	hostInput2 = (float*)malloc(size);

	if(hostInput2==NULL)
	{
		printf("Host Memory Allocation is failed for hostInput2 array.\n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	hostOutput = (float*)malloc(size);

	if(hostOutput==NULL)
	{
		printf("Host Memory Allocation is failed for hostOutput array.\n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	gold=(float*)malloc(size);
	if(gold==NULL)
	{
		printf("Host Memory Allocation is failed for gold array.\n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	//filling values into host arrays
	fillFloatArrayWithRandomNumbers(hostInput1,iNumberOfArrayElements);
	fillFloatArrayWithRandomNumbers(hostInput2,iNumberOfArrayElements);

	//device memory allocation
	result =cudaMalloc((void**)&deviceInput1,size);
	if(result != cudaSuccess)
	{
		printf("Device Memory allocation is failed for deviceInput1 array.\n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	result = cudaMalloc((void**)&deviceInput2,size);//one address is going to hold another address ie host address is holding in device address thats why void**
	if(result != cudaSuccess)
	{
		printf("Device Memory allocation is failed for deviceInput2 array.\n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	result = cudaMalloc((void**)&deviceOutput,size);//one address is going to hold another address ie host address is holding in device address thats why void**
	if(result != cudaSuccess)
	{
		printf("Device Memory allocation is failed for deviceOutput array.\n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	//copy data from host arrays into device arrays
	result = cudaMemcpy(deviceInput1,hostInput1,size,cudaMemcpyHostToDevice);
	if(result != cudaSuccess)
	{
		printf("Host to Device data Copy is failed for deviceInput1 array.\n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	result = cudaMemcpy(deviceInput2,hostInput2,size,cudaMemcpyHostToDevice);//destination,source,size,copy process from where to where
	if(result != cudaSuccess)
	{
		printf("Host to Device data Copy is failed for deviceInput2 array.\n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	dim3 dimGrid = dim3((int)ceil((float)iNumberOfArrayElements/256.0f),1,1);//will get 5 blocks in x direction and as usual no. of blocks in 
	dim3 dimBlock = dim3(256,1,1);//1 thread in x axis and y and z main 1 1 hi by default

	StopWatchInterface* timer=NULL;
	sdkCreateTimer(&timer);
	sdkStartTimer(&timer);

	//cuda kernel for vector addition
	vecAddGPU <<<dimGrid, dimBlock >>> (deviceInput1,deviceInput2,deviceOutput,iNumberOfArrayElements);//calling kernel no. of times as no. of threads
	//pratyek thread aapla aapla kaam karnar mhanje ek space 1 ch thread bharnar jo tyala alloted aasel aaplya aaplya id ne pratyek pixel bharnar so jevdhe pixel tevdhech threads tya peksha jasta nahi 
	
	sdkStopTimer(&timer);
	timeOnGPU = sdkGetTimerValue(&timer);
	sdkDeleteTimer(&timer);
	timer=NULL;
	
	//copy data from device array to host array
	result = cudaMemcpy(hostOutput,deviceOutput,size,cudaMemcpyDeviceToHost);
	if(result != cudaSuccess)
	{
		printf("Device to Host Data Copy is failed for host Output array./n");
		cleanup();
		exit(EXIT_FAILURE);
	}

	vecAddCPU(hostInput1,hostInput2,gold,iNumberOfArrayElements);
	const float epsilon=0.000001f;
	int breakValue=-1;
	bool bAccuracy=true;
	//vector addition on host
	for(int i=0;i< iNumberOfArrayElements;i++)
	{
		float val1=gold[i];
		float val2=hostOutput[i];
		if(fabs(val1-val2)>epsilon){
			bAccuracy=false;
			breakValue=i;
			break;
		}
	}

	char str[128];
	if(bAccuracy==false)
		sprintf(str,"Comparision of CPU and GPU vector addition is not within accuracy of 0.000001 at array index %d",breakValue);
	else	
		sprintf(str,"Comparision of CPU and GPU vector addition is within accurarcy of 0.000001");
	//output
	printf("Array1 begins from 0th index %.6f to %dth index %.6f\n",hostInput1[0], iNumberOfArrayElements - 1, hostInput1[iNumberOfArrayElements - 1]);
	printf("Array2 begins from Oth index %.6f to %dth index %.6f\n",hostInput2[0], iNumberOfArrayElements - 1, hostInput2[iNumberOfArrayElements - 1]);

	printf("CUDA Kernel Grid dimension = %d,%d, %d and Block dimension = %d,%d,%d\n", dimGrid.x, dimGrid.y, dimGrid.z, dimBlock.x, dimBlock.y,dimBlock.z);

	printf("Output Array begins from Oth index %.6f to %dth index %.6f\n",hostOutput[0], iNumberOfArrayElements - 1, hostOutput[iNumberOfArrayElements - 1]);

	printf("Time taken for Vector Addition on CPU = %.6f\n", timeOnCPU);
	printf("Time taken for Vector Addition on GPU =%.6f\n",timeOnGPU);
	printf("%s\n",str);

	cleanup();

	return(0);
}

void fillFloatArrayWithRandomNumbers(float* arr,int len){
	//code
	const float fscale=0.1f / (float)RAND_MAX;
	for(int i=0;i<len;i++){
		arr[i]=fscale * rand();
	}
}

void vecAddCPU(const float* arr1,const float* arr2,float *out,int len){
	//code
	StopWatchInterface* timer=NULL;
	sdkCreateTimer(&timer);
	sdkStartTimer(&timer);

	for(int i=0;i<len;i++)
	{
		out[i]=arr1[i]+arr2[i];
	}

	sdkStopTimer(&timer);
	timeOnCPU=sdkGetTimerValue(&timer);
	sdkDeleteTimer(&timer);
	timer=NULL;
}

void cleanup(void)
{
	//code
	if (deviceOutput)
	{
		cudaFree(deviceOutput);
		deviceOutput=NULL;
	}

	if (deviceInput2)
	{
		cudaFree(deviceInput2);
		deviceOutput=NULL;
	}

	if (deviceInput1)
	{
		cudaFree(deviceInput1);
		deviceOutput=NULL;
	}

	if(gold){
		free(gold);
		gold=NULL;
	}

	if (hostOutput)
	{
		cudaFree(hostOutput);
		deviceOutput=NULL;
	}

	if (hostInput2)
	{
		cudaFree(hostInput2);
		deviceOutput=NULL;
	}

	if (hostInput1)
	{
		cudaFree(hostInput1);
		deviceOutput=NULL;
	}

}








