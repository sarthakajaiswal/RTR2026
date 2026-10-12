#pragma once 

#include <gl\glew.h> 
#include <gl\gl.h> 
#include "vmath.h" 


typedef struct E_camera
{
    vmath::vec3 pos;
    vmath::vec3 lookatPoint;
    vmath::mat4 viewMatrix;
    vmath::mat4 projectionMatrix;
    vmath::vec3 front = vmath::vec3(0.0f, 0.0f, -1.0f);
    vmath::vec3 up = vmath::vec3(0.0f, 1.0f, 0.0f);
    float speed = 0.01f;

    // moves pos based on keyW/A/S/D/Z/X (see E_inputs.h) and recomputes viewMatrix
    void update();
    void resize(int width, int height);
    void mouse_callback(GLfloat xpos, GLfloat ypos); 
    void init(); 
} E_camera;

extern E_camera engineCamera;

//mouse
extern GLfloat xpos;
extern GLfloat ypos;
extern GLfloat yaw;
extern GLfloat pitch;
extern GLfloat lastX;
extern GLfloat lastY;

extern bool keyW, keyA, keyS, keyD, keyZ, keyX; 

void mouse_callback(GLfloat xpos, GLfloat ypos);



