#include <stdio.h> 
#include <stdlib.h> 

#define INVALID_INDEX   -1 
#define VECTOR_EMPTY    -2 
#define SUCCESS         0 

struct vector 
{
    int* array;
    unsigned int size;  
}; 


struct vector* create_vector(unsigned long long size); 
void insert_end(struct vector* pvec, int new_data); 
int remove_end(struct vector* pvec); 
void show_vector(struct vector* pvec); 

void destroy_vector(struct vector* pvec); 

void test_vector(void); 

// =====================================================
// =================== CLIENT SIDE ===================== 
// =====================================================

int main(void) 
{
    test_vector(); 
    return (0); 
} 

struct vector gVector; 

void test_vector(void) 
{
    struct vector* pvec1; 

    pvec1 = create_vector(5); 

    // fill 
    for(int i = 0; i < pvec1->size; ++i) 
        pvec1->array[i] = (i+1)*10; 

    show_vector(pvec1); 

    insert_end(pvec1, 100); 
    show_vector(pvec1); 

    insert_start(pvec1, 200); 
    show_vector(pvec1); 

    int retVal = insert_at_index(pvec1, 500, 2); 
    if(retVal == INVALID_INDEX) 
    {
        printf("invalid index provided\n"); 
    } 

    show_vector(pvec1); 

    remove_start(pvec1); 
    show_vector(pvec1); 

    remove_end(pvec1); 
    show_vector(pvec1); 

    remove_index(pvec1, 2); 
    show_vector(pvec1); 

    destroy_vector(pvec1); 
} 

// ===================================================== 
// ============ VECTOR INTERFACE FUNCTIONS =============
// ===================================================== 
struct vector* create_vector(unsigned long long size) 
{
    struct vector* pvec; 

    pvec = (struct vector*)malloc(sizeof(struct vector)); 
    if(pvec == NULL) 
    {
        printf("malloc() failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    pvec->array = (int*)malloc(size * sizeof(int)); 
    if(pvec->array == NULL) 
    {
        printf("malloc() failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    pvec->size = size; 

    return (pvec); 
} 

void insert_start(struct vector* pvec, int new_data) 
{
    pvec->size += 1; 
    pvec->array = (int*)realloc(pvec->array, pvec->size * sizeof(int)); 
    if(pvec->array == NULL) 
    {
        printf("realloc() failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    
    for(int i = pvec->size-2; i >= 0; --i) 
    {
        pvec->array[i+1] = pvec->array[i]; 
    } 
    pvec->array[0] = new_data; 
} 

void insert_end(struct vector* pvec, int new_data) 
{
    pvec->size += 1; 
    pvec->array = (int*)realloc(pvec->array, pvec->size * sizeof(int)); 
    if(pvec->array == NULL) 
    {
        printf("realloc() failed\n"); 
        exit(EXIT_FAILURE); 
    } 
    pvec->array[pvec->size-1] = new_data; 
} 

int insert_at_index(struct vector* pvec, int new_data, unsigned long long index) 
{
    if(index >= pvec->size || index < 0)     
        return (INVALID_INDEX); 

    pvec->size += 1; 
    pvec->array = (int*)realloc(pvec->array, sizeof(int) * pvec->size); 
    if(pvec->array == NULL) 
    {
        printf("realloc() failed\n"); 
        exit(EXIT_FAILURE); 
    } 

    for(unsigned int i = pvec->size-2; i >= index; i--) 
        pvec->array[i+1] = pvec->array[i]; 

    pvec->array[index] = new_data; 

    return (0); 
} 


int remove_start(struct vector* pvec)  
{
    for(unsigned int i = 0; i <= pvec->size-2; i++)
        pvec->array[i] = pvec->array[i+1]; 

    pvec->size -= 1; 
    pvec->array = (int*)realloc(pvec->array, pvec->size * sizeof(int)); 
    if(pvec->array == NULL) 
    {
        printf("realloc() failed\n"); 
        exit(EXIT_FAILURE); 
    } 

    return (0); 
} 

int remove_end(struct vector* pvec) 
{
    if(pvec->size == 0) 
        return (VECTOR_EMPTY); 

    pvec->size -= 1; 
    pvec->array = (int*)realloc(pvec->array, pvec->size * sizeof(int)); 
    if(pvec->array == NULL) 
    {
        printf("realloc() failed\n"); 
        exit(EXIT_FAILURE); 
    } 

    return (0); 
} 

int remove_index(struct vector* pvec, unsigned long long index) 
{
    if(index >= pvec->size)
        return (INVALID_INDEX); 

    for(unsigned int i = index; i <= pvec->size-2; ++i) 
        pvec->array[i] = pvec->array[i+1]; 

    pvec->size -= 1; 
    pvec->array = (int*)realloc(pvec->array, sizeof(int)*pvec->size); 
    if(pvec->array == NULL) 
    {
        printf("realloc() failed\n"); 
        exit(EXIT_FAILURE); 
    } 

    return (0); 
} 


void sort_vector(struct vector* pvec) 
{

} 


void show_vector(struct vector* pvec) 
{
    for(int i = 0; i < pvec->size; ++i) 
        printf("vector[%d] = %d\n", i, pvec->array[i]); 
    printf("-------------------\n"); 
} 


void destroy_vector(struct vector* pvec) 
{
    if(pvec->array != NULL)
    {
        free(pvec->array); 
        pvec->array = NULL; 
        pvec->size = 0; 
    } 
} 


