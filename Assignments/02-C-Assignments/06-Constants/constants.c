#include <stdio.h> 

#define MY_PI   3.1415926535897932 

#define AMC_STRING  "Astromedicomp RTR" 

// unnamed enums 
enum 
{
    SUNDAY, 
    MONDAY, 
    TUESDAY, 
    WEDNESDAY, 
    THURSDAY, 
    FRIDAY, 
    SATURDAY, 
}; 

enum 
{
    JANUARY = 1, 
    FEBRUARY, 
    MARCH, 
    APRIL, 
    MAY, 
    JUNE, 
    JULY, 
    AUGUST, 
    SEPTEMBER, 
    OCTOBER, 
    NOVEMBER, 
    DECEMBER 
}; 

// named enums 
enum Numbers 
{
    ONE, 
    TWO, 
    THREE, 
    FOUR, 
    FIVE = 5, 
    SIX, 
    SEVEN, 
    EIGHT, 
    NINE, 
    TEN 
}; 

enum boolean 
{
    TRUE = 1, 
    FALSE = 0 
}; 

int main(void) 
{
    // local constant declarations 
    const double epsilon = 0.000001; 

    // code 
    printf("\n\n"); 
    printf("Local constant epsilon = %lf\n\n", epsilon); 

    printf("Sunday is day number = %d\n", SUNDAY); 
    printf("Monday is day number = %d\n", MONDAY); 
    printf("Tuesday is day number = %d\n", TUESDAY); 
    printf("Wednesday is day number = %d\n", WEDNESDAY); 
    printf("Thursday is day number = %d\n", THURSDAY); 
    printf("Friday is day number = %d\n", FRIDAY); 
    printf("Saturday is day number = %d\n\n", SATURDAY); 

    printf("One is enum number = %d\n", ONE); 
    printf("Two is enum number = %d\n", TWO); 
    printf("Three is enum number = %d\n", THREE); 
    printf("four is enum number = %d\n", FOUR); 
    printf("five is enum number = %d\n", FIVE); 
    printf("Six is enum number = %d\n", SIX); 
    printf("Seven is enum number = %d\n", SEVEN); 
    printf("Eight is enum number = %d\n", EIGHT); 
    printf("Nine is enum number = %d\n", NINE); 
    printf("Ten is enum number = %d\n\n", TEN); 

    printf("January is month number = %d\n", JANUARY); 
    printf("February is month number = %d\n", FEBRUARY); 
    printf("March is month number = %d\n", MARCH); 
    printf("April is month number = %d\n", APRIL); 
    printf("May is month number = %d\n", MAY); 
    printf("june is month number = %d\n", JUNE); 
    printf("July is month number = %d\n", JULY); 
    printf("August is month number = %d\n", AUGUST); 
    printf("September is month number = %d\n", SEPTEMBER); 
    printf("October is month number = %d\n", OCTOBER); 
    printf("November is month number = %d\n", NOVEMBER); 
    printf("December is month number = %d\n\n", DECEMBER); 

    printf("value of TRUE is %d\n", TRUE); 
    printf("calue of FALSE is %d\n\n", FALSE); 

    printf("MY PI macro value = %.101f\n\n", MY_PI); 
    printf("Area of circle of radius 2 units = %f\n\n", (MY_PI * 2.0f * 2.0f)); 

    printf("\n\n"); 

    printf(AMC_STRING); 
    printf("\n\n"); 
    
    printf("AMC_STRING is : %s\n", AMC_STRING); 
    printf("\n\n"); 

    return (0); 
} 

