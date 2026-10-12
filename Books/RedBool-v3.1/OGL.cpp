#include <GL/freeglut.h> 

bool bIsFullScreen=false; 

int main(int argc, char *argv[]) 
{
    // function declarations 
    void initialize(void); 
    void uninitialize(void); 
    void resize(int, int); 
    void display(void); 
    void keyboard(unsigned char, int, int); 
    void mouse(int, int, int, int); 

    // code 
    glutInit(&argc, argv); 
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA); 
    glutInitWindowSize(800, 600); 
    glutInitWindowPosition(100, 100); 
    glutCreateWindow("My first RTR7 Program: Sarthak A. Jaiswal"); 

    initialize(); 

    glutReshapeFunc(resize); 
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard); 
    glutMouseFunc(mouse); 
    glutCloseFunc(uninitialize); 

    glutMainLoop(); 

    // flow should not come here 
    return (0); 
} 

void initialize(void) 
{
    // code 
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
} 

void resize(int width, int height) 
{
    // code 
    if(height <= 0) 
        height = 1; 
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity(); 

    glViewport(0, 0, (GLsizei)width, (GLsizei)height); 
} 

void display(void) 
{
    // code 
    glClear(GL_COLOR_BUFFER_BIT); 

    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity(); 

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_POLYGON);
    
    glColor3f(1.0f, 0.0f, 0.0f);

    glEdgeFlag(GL_TRUE);
    glVertex3f(0.0, 1.0, 0.0);
    glEdgeFlag(GL_FALSE);
    glVertex3f(-1.0, -1.0, 0.0);
    glEdgeFlag(GL_TRUE);
    glVertex3f(1.0, -1.0, 0.0); 
    
    glEnd();

    glutSwapBuffers();  
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
        if(bIsFullScreen==false)
        {
            glutFullScreen(); 
            bIsFullScreen=true; 
        } 
        else 
        {
            glutLeaveFullScreen(); 
            bIsFullScreen=false; 
        } 
        break; 

    default: 
        break; 
    } 
} 

void mouse(int button, int state, int x, int y) 
{
    // code 
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

