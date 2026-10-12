#include <stdio.h> 
#include <stdlib.h> 

struct Node 
{
    int data; 
    struct Node* next; 
}; 

void test(void); 

int main(void)  
{
    test(); 
    return (0); 
} 

void test(void) 
{
    void* xmalloc(size_t size); 

    // code 
    struct Node* p_head_node = NULL; 

    // dummy / head node 
    p_head_node = (struct Node*)xmalloc(sizeof(struct Node)); 
    p_head_node->data = 0; 
    p_head_node->next = NULL; 

    // insert node with data 10 
    int new_data = 10; 
    p_head_node->next = (struct Node*)xmalloc(sizeof(struct Node)); 
    p_head_node->next->next = NULL; 
    p_head_node->next->data = new_data; 

    // insert node with data 100 
    new_data = 100; 
    p_head_node->next->next = (struct Node*)xmalloc(sizeof(struct Node)); 
    p_head_node->next->next->data = new_data; 
    p_head_node->next->next->next = NULL; 

    // traversal of list 
    struct Node* p_run = NULL; 

    p_run = p_head_node->next; 
    while(p_run != NULL) 
    {
        printf("[%d]\n", p_run->data); 
        p_run = p_run->next; 
    }
} 

void* xmalloc(size_t size) 
{
    void* ptr = NULL; 
    ptr = malloc(size); 
    if(ptr == NULL) 
    {
        printf("malloc() failed to allocate memory.. Exitting.. \n"); 
        exit(EXIT_FAILURE); 
    } 

    return (ptr); 
} 


