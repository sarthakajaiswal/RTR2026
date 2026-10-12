#include <GL/freeglut.h> 
#include <math.h> 

#define DEG_TO_RAD  3.14/180.0 

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
    // local function declarations 
    void drawBallon(float posX, float posY, float r, float g, float b); 
    void drawRose(float cx, float cy, float r); 

    // code 
    glClear(GL_COLOR_BUFFER_BIT); 

    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity(); 

    drawBallon(-0.2f, 0.2f, 0.0f, 1.0f, 0.0f); 
    // drawRose(0.0f, 0.0f, 0.2f); 

    glutSwapBuffers(); 
} 

void drawBallon(float posX, float posY, float r, float g, float b) 
{
    // function declarations 
    void drawSemicircle(float cx, float cy, float radius); 

    // variable declarations 
    float radius = 0.2f; 

    // code 
    glColor3f(r, g, b); 

    // semicircle 
    drawSemicircle(posX, posY, radius); 

    // lower triangle 
    glBegin(GL_TRIANGLES); 
    glVertex3f(posX+radius, posY, 0.0f); 
    glVertex3f(posX-radius, posY, 0.0f);  
    glVertex3f(posX, posY-radius, 0.0f); 
    glEnd(); 

    // thread  
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES); 
    glVertex3f(posX, posY-radius, 0.0f);
    glVertex3f(posX, posY-radius-0.5, 0.0f);
    glEnd(); 

} 

void drawSemicircle(float cx, float cy, float radius)  
{
    // variable declarations 
    int nrSteps = 15; 
    float angle = 0; 

    // code
    glBegin(GL_POLYGON); 
    
    glVertex3f(cx, cy, 0.0f); 
    while(angle <= 180.0f) 
    {
        glVertex3f(cx + radius*cos(angle*DEG_TO_RAD), cy + radius*sin(angle*DEG_TO_RAD), 0.0f); 
        angle += 180.0f/(float)nrSteps; 
    } 

    glEnd();  
} 

void drawRose(float cx, float cy, float r) 
{
    // r = cos(k*theta) k = n/d 
    // n=4 d=5 

    // variable declarations 
    float n = 4;
    float d = 5; 

    glColor3f(1.0f, 1.0f, 1.0f); 
    glBegin(GL_LINE_LOOP); 
    for(float angle = 0.0f; angle < 1800.0f; angle += 0.5f) 
    {
        glVertex3f(
            cx + cos(n/d * angle*DEG_TO_RAD) * r*cos(angle*DEG_TO_RAD), 
            cy + cos(n/d * angle*DEG_TO_RAD) * r*sin(angle*DEG_TO_RAD), 
            0.0f); 
    } 
    glEnd(); 
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

