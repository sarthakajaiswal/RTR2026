#include <stdio.h>  
#include <GL/freeglut.h> 
#include <math.h> 
#include <time.h> 

#define PI  3.1418 

bool bIsFullScreen = false; 
float ballX = 0.0f, ballY = 0.0f; 
float maxHeight = 1.0f; 
float radius = 0.1f; 

int main(int argc, char *argv[]) 
{
    // function declarations 
    void initialize(void); 
    void uninitialize(void); 
    void resize(int, int); 
    void display(void); 
    void update(void); 
    void keyboard(unsigned char, int, int); 
    void mouse(int, int, int, int); 

    // code 
    glutInit(&argc, argv); 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); 
    glutInitWindowPosition(100, 100); 
    glutInitWindowSize(800, 800); 
    glutCreateWindow("My first RTR window"); 

    initialize(); 

    glutReshapeFunc(resize); 
    glutDisplayFunc(display); 
    glutIdleFunc(update); 
    glutKeyboardFunc(keyboard); 
    glutMouseFunc(mouse); 
    glutCloseFunc(uninitialize); 
 
    glutMainLoop(); 

    srand(time(0)); 

    // control flow should not come here 
    return (0); 
} 

void initialize(void) 
{
    // code 
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
} 

void resize(int width, int height) 
{
    if(height <= 0) 
        height = 1; 
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity(); 

    glViewport(0, 0, (GLsizei)width, (GLsizei)height); 
} 

void display(void) 
{
    glClear(GL_COLOR_BUFFER_BIT); 

    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity(); 

    // point 
    glBegin(GL_POINTS); 
    {
        glVertex3f(0.0f, 0.0f, 0.0f); 
    }
    glEnd(); 
    
    // line 
    glBegin(GL_LINES); 
    {
        glVertex2f(1.0f, 1.0f); 
        glVertex2f(-1.0f, -1.0f); 
    } 
    glEnd(); 

    // triangle 
    glBegin(GL_TRIANGLES); 
    {
        glColor3f(1.0f, 0.0f, 0.0f); 
        glVertex2f(0.0f, 1.0f); 
        glColor3f(0.0f, 1.0f, 0.0f); 
        glVertex2f(-1.0f, -1.0f); 
        glColor3f(0.0f, 0.0f, 1.0f); 
        glVertex2f(1.0f, -1.0f); 
    } 
    glEnd(); 

    // rectangle 

    // pentagon 
    glBegin(GL_LINE_LOOP); 
    {
        for(float angle = 0.0f; angle < 360.0f; angle += 360.0/5) 
        {
                float x = 0.2 * cos(angle * PI / 180.0f); 
                float y = 0.2 * sin(angle * PI / 180.0f); 

                glVertex2f(x, y); 
        } 
    } 
    glEnd(); 

    // hexagon 
    glBegin(GL_LINE_LOOP); 
    {
        for(float angle = 0.0f; angle < 360.0f; angle += 360.0/6) 
        {
                float x = 0.2 * cos(angle * PI / 180.0f); 
                float y = 0.2 * sin(angle * PI / 180.0f); 

                glVertex2f(x, y); 
        } 
    } 
    glEnd(); 

    glutSwapBuffers(); 
} 

void update(void) 
{
    glutPostRedisplay(); 
} 

void keyboard(unsigned char key, int x, int y) 
{
    // code 
    switch(key) 
    {
        case 27: 
            glutLeaveMainLoop(); 
            break; 

        case 'F':
        case 'f': 
            if(bIsFullScreen == false) 
            {
                glutFullScreen(); 
                bIsFullScreen = true; 
            } 
            else 
            {
                glutLeaveFullScreen(); 
                bIsFullScreen = false; 
            } 
            break; 
    } 
} 

void mouse(int button, int state, int x, int y) 
{
    switch(button) 
    {
        case GLUT_RIGHT_BUTTON: 
            glutLeaveMainLoop(); 
            break; 
        default: 
            break; 
    } 
}   

void uninitialize(void) 
{
    // code 
} 

