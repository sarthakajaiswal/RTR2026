#include <GL/freeglut.h> 
#include <time.h> 
#include <math.h> 
#include <stdio.h> 

#define NUM_PARTICLES   64 
#define MAX_LINE_LENGTH 0.25f     

struct Particle 
{
    float px, py; 
    float vx, vy; 
}; 

struct Particle particles[NUM_PARTICLES]; 
FILE* gpFile = NULL; 

bool bIsFullScreen = false; 

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
    glutKeyboardFunc(keyboard); 
    glutMouseFunc(mouse); 
    glutCloseFunc(uninitialize); 
    glutIdleFunc(update); 

    glutMainLoop(); 

    // control flow should not come here 
    return (0); 
} 

void initialize(void) 
{
    // function declarations 
    float get_random(float, float); 

    // code 
    // initialize particles array 
    srand(time(0)); 
    for(int i = 0; i < NUM_PARTICLES; ++i) 
    {
        particles[i].px = get_random(-1.0f, 1.0f); 
        particles[i].py = get_random(-1.0f, 1.0f); 
        particles[i].vx = get_random(-0.0004f, 0.0004f); 
        particles[i].vy = get_random(-0.0004f, 0.0004f); 
    } 

    gpFile = fopen("log.txt", "w"); 
    for(int i = 0; i < NUM_PARTICLES; ++i) 
    {
        fprintf(gpFile, "%.2f, %.2f, %.2f, %.2f\n", particles[i].px, particles[i].py, particles[i].vx, particles[i].vy); 
    } 
    fclose(gpFile); 
    gpFile = NULL; 

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
} 

float get_random(float min, float max) 
{
    float random = (float)rand() / (float)RAND_MAX; 
    return (min + random * (max-min)); 
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
    // function declarations 
    float distance(struct Particle, struct Particle); 

    glClear(GL_COLOR_BUFFER_BIT); 

    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity(); 

    glPointSize(3.0f); 
    glEnable(GL_POINT_SMOOTH); 
    
    for(int i = 0; i < NUM_PARTICLES; ++i) 
    {
        glColor3f(1.0f, 1.0f, 1.0f); 
        glBegin(GL_POINTS); 
        glVertex2f(particles[i].px, particles[i].py); 
        glEnd(); 

        for(int j = i+1; j < NUM_PARTICLES; ++j) 
        {
            float dist_ij = distance(particles[i], particles[j]); 
            if(dist_ij < MAX_LINE_LENGTH) 
            {                
                glColor3f(1.0f, 0.5, 0.25f); 
                glLineWidth(0.5f + dist_ij); 
                glBegin(GL_LINES); 
                glVertex2f(particles[i].px, particles[i].py);
                glVertex2f(particles[j].px, particles[j].py);  
                glEnd(); 
            } 
        } 
    } 

    glutSwapBuffers(); 
} 

void update(void) 
{
    for(int i = 0; i < NUM_PARTICLES; ++i) 
    {
        particles[i].px += particles[i].vx; 
        particles[i].py += particles[i].vy; 

        if(particles[i].px > 1.0f || particles[i].px < -1.0f) 
            particles[i].vx *= -1.0f; 

        if(particles[i].py > 1.0f || particles[i].py < -1.0f) 
            particles[i].vy *= -1.0f; 
    }  

    glutPostRedisplay(); 
} 

float distance(struct Particle p1, struct Particle p2) 
{
    return sqrt((p1.px-p2.px)*(p1.px-p2.px) + (p1.py-p2.py)*(p1.py-p2.py)); 
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

        default: 
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

