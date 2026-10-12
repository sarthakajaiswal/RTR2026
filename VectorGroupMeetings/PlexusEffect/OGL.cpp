#include <GL/freeglut.h> 
#include <math.h> 
#include <stdio.h> 

#define NUM_VERTICES    64 
#define MAX_LINE_LENGHT 0.2f 

// structures 
struct Vertex 
{
    float px, py; 
    float vx, vy; 
}; 

struct Vertex vertices[NUM_VERTICES]; 

bool bIsFullScreen = false; 

FILE* file = NULL; 

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
    glutInitWindowSize(800, 600); 
    glutCreateWindow("My first RTR window"); 

    initialize(); 

    glutReshapeFunc(resize); 
    glutDisplayFunc(display); 
    glutIdleFunc(update); 
    glutKeyboardFunc(keyboard); 
    glutMouseFunc(mouse); 
    glutCloseFunc(uninitialize); 

    glutMainLoop(); 

    // control flow should not come here 
    return (0); 
} 

void initialize(void) 
{
    // function declarations 
    float random_float(float, float); 

    // code 
    for(int i = 0; i < NUM_VERTICES; ++i) 
    {
        vertices[i].px = random_float(-1.0f, 1.0f); 
        vertices[i].py = random_float(-1.0f, 1.0f); 
        vertices[i].vx = random_float(-0.0002f, 0.0002f); 
        vertices[i].vy = random_float(-0.0002f, 0.0002f); 
    } 

    file = fopen("log.txt", "w"); 

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
} 

float random_float(float min, float max) 
{
    // variable declarations 
    float zero_to_one; 
    float random_number; 

    // code 
    zero_to_one = (float)rand() / (float)RAND_MAX; 
    random_number = min + (zero_to_one * (max-min)); 

    return (random_number); 
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
    // variable declarations 
    float distance(struct Vertex p1, struct Vertex p2); 

    // code 
    glClear(GL_COLOR_BUFFER_BIT); 

    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity(); 

    glPointSize(4.0f); 
    for(int i = 0; i < NUM_VERTICES; ++i) 
    {
        glBegin(GL_POINTS); 
        glVertex2f(vertices[i].px, vertices[i].py); 
        glEnd(); 

        for(int j = i+1; j < NUM_VERTICES; ++j) 
        {
            float dist = distance(vertices[i], vertices[j]); 
            if(dist < MAX_LINE_LENGHT) 
            {
                glLineWidth(0.2 + dist*8.0); 
                glBegin(GL_LINES); 
                glVertex2f(vertices[i].px, vertices[i].py);
                glVertex2f(vertices[j].px, vertices[j].py);
                glEnd();  
            } 
        } 
    } 

    glutSwapBuffers(); 
} 

float distance(struct Vertex p1, struct Vertex p2) 
{
    // code 
    float dist = sqrt((p2.px-p1.px)*(p2.px-p1.px) + (p2.py-p1.py)*(p2.py-p1.py)); 
    return (dist); 
} 

void update(void) 
{
    // code 
    for(int i = 0; i < NUM_VERTICES; ++i) 
    {
        if(vertices[i].px > 1.0f || vertices[i].px < -1.0f)
            vertices[i].vx *= -1.0f; 
        if(vertices[i].py > 1.0f || vertices[i].py < -1.0f)
            vertices[i].vy *= -1.0f; 

        vertices[i].px += vertices[i].vx; 
        vertices[i].py += vertices[i].vy; 
    } 

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
    fclose(file); 
} 

