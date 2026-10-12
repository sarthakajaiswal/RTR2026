#include <stdio.h> 

int arr[10] = {10, 15, 17, 24, 30, 16}; 
int N = 6; 

void insertion_sort(int arr[], int n); 

int main(void) 
{
    printf("Array elements are before sort:\n"); 
    for(int i = 0; i < N; i++) 
    {
        printf("%d\n", arr[i]); 
    }

    // sorting 
    int temp = arr[N-1]; 

    int i = N-2; 
    while(i > -1 && arr[i] > temp) 
    {
        arr[i+1] = arr[i]; 
        i = i - 1; 
    } 
    arr[i+1] = temp; 

    printf("Array elements after sort:\n"); 
    for(int i = 0; i < N; i++) 
    {
        printf("%d\n", arr[i]); 
    }
} 
