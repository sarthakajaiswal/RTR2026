#ifndef _CAMERA_HPP 
#define _CAMERA_HPP 

#include <Windows.h> 
#include <iostream> 
#include <vector> 
#include <windowsx.h> 
#include "vmath.h" 

// symbolic constants 
#define CAMERA_DEFAULT_YAW     -90.0f 
#define CAMERA_DEFAULT_PITCH   0.0f 

extern HWND ghwnd;  

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

        // automatic camera related 
        std::vector<vmath::vec3> positionArray; 
        std::vector<float> yawArray, pitchArray; 

        void updateVectorsAfterChangesInAngle(void);  

    public: 
        Camera(); 
        
        void automise(std::vector<vmath::vec3> positions, std::vector<float> yaws, std::vector<float> pitches); 
        vmath::vec3 getPosition(); 
        vmath::vec3 getCenter() const; 
        vmath::mat4 getViewMatrix(); 
        void setState(vmath::vec3 position, float yaw, float pitch); 
        void setPosition(vmath::vec3 position); 
        void cameraCallback(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam); 
        void printInfo(); 
}; 

#endif // CAMERA_HPP 

