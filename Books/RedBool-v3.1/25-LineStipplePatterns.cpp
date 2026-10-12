#include <GL/freeglut.h> 

#define drawOneLine(x1, y1, x2, y2) \
    glBegin(GL_LINES);              \
    glVertex2f((x1), (y1));         \
    glVertex2f((x2), (y2));         \
    glEnd();

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
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGBA); 
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
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT); 
} 

void resize(int width, int height) 
{
    // code 
    if(height <= 0) 
        height = 1; 
   glViewport(0, 0, (GLsizei) width, (GLsizei) height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0, (GLdouble) width, 0.0, (GLdouble) height);
} 

void display(void)
{
    int i;
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0, 1.0, 1.0);

    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0x0101); /*  dotted  */
    drawOneLine(50.0, 125.0, 150.0, 125.0);

    glLineStipple(1, 0x00FF); /*  dashed  */
    drawOneLine(150.0, 125.0, 250.0, 125.0);

    glLineStipple(1, 0x1C47); /*  dash/dot/dash  */
    drawOneLine(250.0, 125.0, 350.0, 125.0);

    glLineWidth(5.0);
    glLineStipple(1, 0x0101); /*  dotted  */
    drawOneLine(50.0, 100.0, 150.0, 100.0);
    glLineStipple(1, 0x00FF); /*  dashed  */
    drawOneLine(150.0, 100.0, 250.0, 100.0);
    glLineStipple(1, 0x1C47); /*  dash/dot/dash  */
    drawOneLine(250.0, 100.0, 350.0, 100.0);

    glLineWidth(1.0);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i < 7; i++)
        glVertex2f(50.0 + ((GLfloat)i * 50.0), 75.0);
    glEnd();

    for (i = 0; i < 6; i++)
    {
        drawOneLine(50.0 + ((GLfloat)i * 50.0), 50.0,
                    50.0 + ((GLfloat)(i + 1) * 50.0), 50.0);
    }

    glLineStipple(5, 0x1C47); /*  dash/dot/dash  */
    drawOneLine(50.0, 25.0, 350.0, 25.0);

    glDisable(GL_LINE_STIPPLE);
    
    glutSwapBuffers(); 
    glFlush();
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

