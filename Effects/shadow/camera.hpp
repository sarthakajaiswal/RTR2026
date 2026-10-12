#ifndef CAMERA_HPP 
#define CAMERA_HPP 

// standard headers 
#include <Windows.h> 
#include <iostream> 
#include <vector> 
#include <windowsx.h> 
#include <math.h> 

// custom headers 
#include "vmath.h"

// symbolic constants 
#define CAMERA_DEFAULT_YAW     -90.0f 
#define CAMERA_DEFAULT_PITCH   0.0f 

extern HWND ghwnd; 
extern FILE* gpFile; 

enum CameraMode 
{
    CAMERA_GAME_MODE = 1, 
    CAMERA_AUTO_MODE  
}; 

class Camera 
{
    private: 
        vmath::vec3 position; 
        vmath::vec3 up; 
        vmath::vec3 target; 
        vmath::vec3 direction; 

        vmath::vec3 right; 
        vmath::vec3 front; 

        float yaw, pitch; 

        void updateVectorsAfterChangesInAngle(void);  

    public: 
        Camera(); 
        vmath::vec3 getPosition(); 
        vmath::mat4 getViewMatrix(); 
        void setPosition(vmath::vec3 newPosition); 
        void setYaw(float _yaw); 
        void setPitch(float _pitch); 
        void cameraCallback(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam); 
        void printInfo(); 
}; 

#endif // CAMERA_HPP 

