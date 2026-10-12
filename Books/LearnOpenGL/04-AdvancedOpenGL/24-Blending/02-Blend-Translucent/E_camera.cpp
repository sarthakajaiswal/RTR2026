#include <stdio.h> 
#include <iostream> 
#include <vector> 
#include <gl/glew.h>

#include "E_camera.h"

bool keyW = false, keyA = false, keyS = false, keyD = false, keyZ = false, keyX = false;

GLfloat xpos;
GLfloat ypos;
GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;
GLfloat lastX;
GLfloat lastY;

std::vector<vmath::vec3> cameraPositions; 
std::vector<vmath::vec3> cameraTargets; 

#define DEBUG   1 

void E_camera::init() 
{
    pos              = vmath::vec3(0.0f, 0.0f, 5.0f);
    front            = vmath::vec3(0.0f, 0.0f, -1.0f);
    up               = vmath::vec3(0.0f, 1.0f, 0.0f);
    lookatPoint      = pos + front;
    speed            = 0.1f;

    viewMatrix       = vmath::mat4::identity(); 
    projectionMatrix = vmath::mat4::identity(); 
} 

void E_camera::mouse_callback(GLfloat xpos, GLfloat ypos)
{
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    vmath::vec3 direction = vmath::vec3(
        cos(vmath::radians(yaw)) * cos(vmath::radians(pitch)), 
        sin(vmath::radians(pitch)), 
        sin(vmath::radians(yaw)) * cos(vmath::radians(pitch))
    );
    front = vmath::normalize(direction);
}

void E_camera::update()
{
#if DEBUG
    if (keyW) pos = pos + front * speed;
    if (keyS) pos = pos - front * speed;
    if (keyA) pos = pos - vmath::normalize(vmath::cross(front, up)) * speed;
    if (keyD) pos = pos + vmath::normalize(vmath::cross(front, up)) * speed;
    if (keyZ) pos = pos - vmath::cross(front, vmath::normalize(vmath::cross(front, up))) * speed;
    if (keyX) pos = pos + vmath::cross(front, vmath::normalize(vmath::cross(front, up))) * speed;
    lookatPoint = pos + front;
#else
    vmath::vec3 dir = lookatPoint - pos;
    float dirLength = vmath::length(dir);
    
    // Protect against division by zero if pos == lookatPoint
    if (dirLength > 0.0001f)
        front = dir / dirLength;

#endif

    vmath::vec3 right = vmath::cross(front, up);
    float rightLength = vmath::length(right);

    if (rightLength < 0.0001f)
        right = vmath::vec3(1.0f, 0.0f, 0.0f);
    else
        right = right / rightLength;

    vmath::vec3 cameraUp = vmath::cross(right, front);
    viewMatrix = vmath::lookat(pos, lookatPoint, cameraUp);
}

void E_camera::resize(int width, int height)
{
    // Prevent division by zero
    if (height <= 0) 
        height = 1;

    projectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

