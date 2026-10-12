#include <stdio.h> 

int main(void) 
{
    // code 
    printf("\n\n"); 
    printf("*******************************************************************"); 
    printf("\n\n"); 

    printf("Hello World !!!\n\n"); 

    int a = 13; 
    printf("Integer Decimal value of 'a' = %d\n", a); 
    printf("Integer Octal value of 'a' = %o\n", a); 
    printf("Integer hexadecimal value of 'a' (Hexadecimal characters in lower case) = %x\n", a); 
    printf("Integer Hexadecinal value of 'a' (Hexadecimal letters In upper case) = %X\n\n", a);   

    char ch = 'A'; 
    printf("Character ch = %c\n", ch); 
    char str[] = "AstroMediComp's Real Time Rendering Batch"; 
    printf("string str = %s\n\n", str); 

    long num = 30121995L; 
    printf("Long integer = %ld\n\n", num); 

    unsigned int b = 7; 
    printf("Unsigned integer 'b' = %u\n\n", b); 

    float f_num = 3012.1995f; 
    printf("Floating point number with just %%f 'f_num = %f\n", f_num); 
    printf("Floating point number with %%4.2f 'f_num = %4.2f\n", f_num); 
    printf("Floating point number with just %%6.5f 'f_num = %6.5f\n", f_num); 

    double d_pi = 3.14159265358979323846; 
    printf("Double Precision floating point number without exponential = %g\n", d_pi); 
    printf("Double precision floating point number with exponential (lower case) = %e\n", d_pi); 
    printf("Double precision floating point number with exponential (upper case) = %E\n\n", d_pi); 
    printf("Double Hexadecimal value of 'd_pi' (Hexadecimal letters in lower case) = %a\n", d_pi); 
    printf("Double Hexadecimal value of 'd_pi' (Hexadecimal letters in upper case) = %A\n", d_pi); 

    printf("***************************************************************************************************\n"); 
    printf("\n\n"); 
    return (0); 
} 
