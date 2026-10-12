
struct Camera 
{
    vmath::vec3 position; 
    vmath::vec3 up; 
    vmath::vec3 target; 

    vmath::mat4 viewMat; 
    
    float fov; 
    float znear; 
    float zfar; 
    
    vmath::mat4 projectionMat; 
    
}; 

struct Camera engineCam; 

void initCam(void) 
{
    engineCam.position = vmath::vec3(0.0f, 0.0f, 10.0f); 
    engineCam.up = vmath::vec3(0.0f, 1.0f, 0.0f); 
    engineCam.target = vmath::vec3(0.0f, 0.0f, -10.0f); 
    engineCam.fov = 45.0f; 
    engineCam.znear = 0.1f; 
    engineCam.zfar = 1000.0f; 

    engineCam.viewMat = vmath::mat4::identity(); 
    engineCam.projectionMat = vmath::mat4::identity(); 
} 

void resizeCam(float width, float height) 
{
    engineCam.projectionMat = vmath::perspective(engineCam.fov, (GLfloat)width/(GLfloat)height, engineCam.znear, engineCam.zfar);  
} 

void updateCam(void) 
{
    // update cam based on keys
    engineCam.viewMat = vmath::lookat(engineCam.position, engineCam.target, engineCam.up); 
} 
