#include <stdio.h> 
#include "camera.hpp" 

extern FILE* gpFile; 

const float CAMERA_SPEED = 1.0; 

void Camera::updateVectorsAfterChangesInAngle(void) 
{
    direction[0] = cos(vmath::radians(yaw)) * cos(vmath::radians(pitch)); 
    direction[1] = sin(vmath::radians(pitch)); 
    direction[2] = sin(vmath::radians(yaw)) * cos(vmath::radians(pitch)); 
    
    up = vmath::vec3(0.0f, 1.0f, 0.0f); 
    front = normalize(direction); 
    right = normalize(cross(up, direction)); 
} 

Camera::Camera()
{
    position = vmath::vec3(0.0, 0.0, 3.0); 
    up = vmath::vec3(0.0, 1.0, 0.0); 
    target = vmath::vec3(0.0, 0.0, 0.0); 
    front = vmath::vec3(0.0, 0.0, -1.0); 
    
    direction = normalize(position - target); 
    yaw = CAMERA_DEFAULT_YAW; 
    pitch = CAMERA_DEFAULT_PITCH; 
    direction[0] = cos(vmath::radians(yaw)) * cos(vmath::radians(pitch)); 
    direction[1] = sin(vmath::radians(pitch)); 
    direction[2] = sin(vmath::radians(yaw)) * cos(vmath::radians(pitch)); 
    right = normalize(cross(up, direction)); 
    up = cross(direction, right);
} 

void Camera::automise(std::vector<vmath::vec3> _positionArray, std::vector<float> _yawArray, std::vector<float> _pitchArray) 
{
    this->positionArray = _positionArray; 
    this->yawArray = _yawArray; 
    this->pitchArray = _pitchArray; 
}  

vmath::vec3 Camera::getPosition()
{
    return (position); 
} 

vmath::vec3 Camera::getCenter() const 
{
    return (position+direction); 
} 

vmath::mat4 Camera::getViewMatrix() 
{
    vmath::mat4 viewMatrix = vmath::lookat(position, position+front, up); 
    return (viewMatrix); 
} 

void Camera::setState(vmath::vec3 _position, float _yaw, float _pitch)
{
    position = _position; 
    yaw = _yaw; 
    pitch = _pitch; 
    updateVectorsAfterChangesInAngle(); 
} 

void Camera::setPosition(vmath::vec3 _position)
{
    position = _position; 
} 

void setControlPoints(std::vector<vmath::vec3> positions, std::vector<float> yaws, std::vector<float> pitches) 
{

} 

void Camera::cameraCallback(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) 
{
    static BOOL bMouseDown = FALSE; 
    
    // mouse event related variables
    static BOOL bGameMode = FALSE; 
    float sensitivity = 0.1f; 

    static BOOL bJustModeChanged = TRUE; 
    static float lastX = 0.0; 
    static float lastY = 0.0; 
    float currentX, currentY; 
    float xOffset, yOffset; 

    switch(iMsg) 
    {
        case WM_LBUTTONDOWN: 
            lastX = GET_X_LPARAM(lParam); 
            lastY = GET_Y_LPARAM(lParam);
            bMouseDown = TRUE; 
            break; 

        case WM_LBUTTONUP: 
            bMouseDown = FALSE; 
            break; 

        case WM_MOUSEMOVE: 
            if(bGameMode == FALSE && bMouseDown == FALSE) 
                break; 

            if(bJustModeChanged) // get the mouse positions on changing the mode 
            {
                lastX = GET_X_LPARAM(lParam); 
                lastY = GET_Y_LPARAM(lParam);
                ShowCursor(!bGameMode); 
                
                bJustModeChanged = FALSE; 
            } 
            currentX = GET_X_LPARAM(lParam);
            currentY = GET_Y_LPARAM(lParam);
            xOffset = currentX - lastX; 
            yOffset = lastY - currentY; 
            lastX = currentX; 
            lastY = currentY; 

            xOffset *= sensitivity; 
            yOffset *= sensitivity; 

            yaw -= xOffset; 
            pitch -= yOffset; 

            if(pitch > 89.0f)
                pitch = 89.0f;
            if(pitch < -89.0f)
                pitch = -89.0f;
            updateVectorsAfterChangesInAngle(); 

            if(bGameMode == TRUE) 
            {
                RECT windowRect; 
                GetClientRect(ghwnd, &windowRect);
                lastX = windowRect.right/2;  
                lastY = windowRect.bottom/2;  
                // lastY = GetSystemMetrics(SM_CYSCREEN)/2; 
                SetCursorPos(lastX, lastY); 
            } 
            break; 

        case WM_CHAR: 
            switch(wParam) 
            {
            case 'w': 
            case 'W':   
                position += CAMERA_SPEED * front; 
                break; 
            case 's': 
            case 'S': 
                position -= CAMERA_SPEED * front; 
                break; 
            case 'a': 
            case 'A': 
                position -= normalize(cross(front, up)) * CAMERA_SPEED; 
                break; 
            case 'd': 
            case 'D': 
                position += normalize(cross(front, up)) * CAMERA_SPEED; 
                break; 

            case 'q': 
            case 'Q': 
                // position[1] -= sensitivity * 2.0; 
                position[1] -= CAMERA_SPEED * 2.0; 
                break; 
            case 'e': 
            case 'E': 
                // position[1] += sensitivity * 2.0;
                position[1] += CAMERA_SPEED * 2.0;
                break; 

            case 'g': 
            case 'G': 
                bGameMode = !bGameMode; 
                bJustModeChanged = true; // to save the values of lastX and lastY for firsttime after mode is changed 
                break; 

            case 'p': 
                printInfo(); 
                break; 
                
            default: 
                break; 
            } 
            break; 

        default: 
            break;  
    } 
} 

void Camera::printInfo() 
{
    fprintf(gpFile, "%.2f, %.2f, %.2f | %.2f | %.2f\n", position[0], position[1], position[2], yaw, pitch); 
} 
