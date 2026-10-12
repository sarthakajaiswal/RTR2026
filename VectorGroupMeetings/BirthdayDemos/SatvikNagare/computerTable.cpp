#include <Windows.h> 
#include <GL/freeglut.h> 
#include <math.h> 
#include "global.h" 

extern float tx, ty, sx, sy; 

void displayComputerTable(void) 
{
    void drawCircle(float, float, float);
    void drawTable(void); 
    void drawMonitor(void);
    
    glPushMatrix(); 
    {
        glTranslatef(-0.80, -0.10, 0.0f); 
        glScalef(1.50, 0.50, 0.0f); 

        drawTable(); 
    } 
    glPopMatrix(); 

    glPushMatrix(); 
    {
        glTranslatef(-0.60, -0.50, 0.0f);
        glScalef(0.60, 1.00, 0.0f);
        drawMonitor();

        glTranslatef(0.60, 0.00, 0.0f);
        drawMonitor(); 

        glTranslatef(0.60, 0.00, 0.0f);
        drawMonitor(); 

        glTranslatef(0.60, 0.00, 0.0f); 
        drawMonitor(); 
    }
    glPopMatrix(); 
} 

void drawCircle(float cx, float cy, float r) 
{
    glBegin(GL_POLYGON); 

    for(float angle = 0.0f; angle <= 360.0f; angle += 0.2) 
    {
        float x = cx + r * sin(angle*PI/180.0); 
        float y = cy + r * cos(angle*PI/180.0); 
        glVertex2f(x, y); 
    } 

    glEnd(); 
} 

void drawTable(void)
{
    glBegin(GL_QUADS);

    //top
    glColor3f(255.0f/255.0f,128.0f/255.0f,0.0/255.0f);
    glVertex2f(0.8f,0.1f);
    glVertex2f(0.2f,0.1f);
    glVertex2f(0.0f,-0.1f);
    glVertex2f(1.0f,-0.1f);

    // legs 
    glColor3f(96.0f/255.0f,96.0f/255.0f,96.0/255.0f);
    glVertex2f(0.2f,-0.1f);
    glVertex2f(0.2f,-0.6f);
    glVertex2f(0.23f,-0.6f);
    glVertex2f(0.23f,-0.1f);

    glVertex2f(0.0f,-0.1f);
    glVertex2f(0.0f,-0.6f);
    glVertex2f(0.03f,-0.6f);
    glVertex2f(0.03f,-0.1f);

    glVertex2f(0.77f,-0.1f);
    glVertex2f(0.77f,-0.6f);
    glVertex2f(0.8f,-0.6f);
    glVertex2f(0.8f,-0.1f);

    glVertex2f(0.97f,-0.1f);
    glVertex2f(0.97f,-0.6f);
    glVertex2f(1.0f,-0.6f);
    glVertex2f(1.0f,-0.1f);

    glColor3f(140.0f/255.0f,70.0f/255.0f,0.0/255.0f);
    glVertex2f(1.0f,-0.1f);
    glVertex2f(0.0f,-0.1f);
    glVertex2f(0.0f,-0.14);
    glVertex2f(1.0f,-0.14);
    
    glEnd();
    
}

void drawMonitor(void)
{
    glBegin(GL_QUADS);

    glColor3f(64.0f/255.0f,64.0f/255.0f,64.0/255.0f);
    glVertex2f(0.25f,0.75f);
    glVertex2f(-0.25f,0.75f);
    glVertex2f(-0.25f,0.45f);
    glVertex2f(0.25f,0.45f);

    glColor3f(204.0f/255.0f,229.0f/255.0f,255.0/255.0f);
    glVertex2f(0.23f,0.73f);
    glVertex2f(-0.23f,0.73f);
    glVertex2f(-0.23f,0.47f);
    glVertex2f(0.23f,0.47f);

    glColor3f(64.0f/255.0f,64.0f/255.0f,64.0/255.0f);
    glVertex2f(0.02f,0.45f);
    glVertex2f(-0.02f,0.45);
    glVertex2f(-0.1f,0.40f);
    glVertex2f(0.1f,0.40f);

    glEnd();
}

/*
void drawClassroom(void)
{   
    //top
    glBegin(GL_QUADS);
    glColor3f(237.0f/255.0f,242.0f/255.0f,208.0/255.0f);
    glVertex2f(0.7f,0.5f);
    glVertex2f(-0.7f,0.5f);
    glVertex2f(-0.7f,-0.5f);
    glVertex2f(0.7f,-0.5f);

    //upper top
    glColor3f(250.0f/255.0f,252.0f/255.0f,255.0/255.0f);
    glVertex2f(1.1f,0.8f);
    glVertex2f(-1.1f,0.8f);
    glVertex2f(-0.7f,0.5f);
    glVertex2f(0.7f,0.5f);

    //left
    glColor3f(237.0f/255.0f,242.0f/255.0f,208.0/255.0f);
    glVertex2f(-1.1f,0.8f);
    glVertex2f(-1.1f,-0.8f);
    glVertex2f(-0.7f,-0.5f);
    glVertex2f(-0.7f,0.5f);

    //bottom
    glColor3f(255.0f/255.0f,255.0f/255.0f,255.0/255.0f);
    glVertex2f(0.7f,-0.5f);
    glVertex2f(-0.7f,-0.5f);
    glVertex2f(-1.1f,-0.8f);
    glVertex2f(1.1f,-0.8f);

    //right
    glColor3f(237.0f/255.0f,242.0f/255.0f,208.0/255.0f);
    glVertex2f(1.1f,-0.8f);
    glVertex2f(1.1f,0.8f);
    glVertex2f(0.7f,0.5f);
    glVertex2f(0.7f,-0.5f);

    glEnd();
}
*/ 
