#include <GL/freeglut.h> 

bool bIsFullScreen = false; 

int main(int argc, char *argv[]) 
{
    // function declarations 
    void initialize(void); 
    void uninitialize(void); 
    void resize(int, int); 
    void display(void); 
    void keyboard(unsigned char, int, int); 
    void mouse(int, int, int, int); 
    void update(void); 


    // code 
    glutInit(&argc, argv); 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); 
    glutInitWindowPosition(100, 100); 
    glutInitWindowSize(800, 600); 
    glutCreateWindow("My first RTR window"); 

    initialize(); 

    glutReshapeFunc(resize); 
    glutDisplayFunc(display); 
    glutKeyboardFunc(keyboard); 
    glutMouseFunc(mouse); 
    glutCloseFunc(uninitialize); 
    // glutUpdateFunc(update); 

    glutMainLoop(); 

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

    // sky 

    // House 
    glBegin(GL_TRIANGLES); 
    glColor3f(142.0f/255.0f, 64.0f/255.0f, 48.0f/255.0f); 
    glVertex3f(0.7f, 0.3f, 0.0f); 
    glColor3f(142.0f/255.0f, 64.0f/255.0f, 48.0f/255.0f); 
    glVertex3f(0.5f, 0.1f, 0.0f); 
    glColor3f(142.0f/255.0f, 64.0f/255.0f, 48.0f/255.0f); 
    glVertex3f(0.9f, 0.1f, 0.0f); 
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

