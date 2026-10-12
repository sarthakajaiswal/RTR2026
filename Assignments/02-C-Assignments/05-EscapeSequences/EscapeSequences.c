#include <stdio.h> 
int main(void) 
{
    // code 
    printf("\n\n"); 
    printf("Going on to next line.. using \\n escape sequence\n"); 
    printf("Demonstatting \t Horizontal \t tab \t using \\t escape sequence !!!\n\n"); 
    printf("\" this is a double quoted output\" done using \\\" \\\" escape sequence\n\n"); 
    printf("\' this is a single quoted output\' done using \\\' \\\' Escape sequence\n\n"); 
    printf("BACKSPACE turned to BACKSPACE\b using escape sequence \\b\n\n"); 

    printf("\r demostrating carriage return using \\t escape sequence\n"); 
    printf("Demonstrating \r carriage return using \\r escape sequence\n"); 
    printf("Demonstrating carriage \r return using \\r escape sequence\n\n"); 

    printf("Demonstrating \x41 using \\xxh escape sequence\n\n"); 
    // 0x41 is the hexadecimal code for letter 'A', 'xxh' is the placeholder for 'x' follewed by 2-digits (hh), altogether forming a hexadecimal number 
    printf("demonstrating \102 using \\ooo escape sequence\n\n"); 
    // 102 is octal code for letter 'B'. 'ooo' is the place-holder fopr 3 digits forming an octal number 

    return (0); 
} 

