#pragma once 

#include <stdio.h> 

enum 
{
    AMC_ATTRIBUTE_POSITION = 0, 
    AMC_ATTRIBUTE_COLOR, 
    AMC_ATTRIBUTE_TEXTURE0, 
    AMC_ATTRIBUTE_NORMAL, 
}; 

extern float gWinWidth, gWinHeight; 
extern FILE* gpFile; 

