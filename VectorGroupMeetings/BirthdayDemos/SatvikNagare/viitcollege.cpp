#include <GL/freeglut.h>
#include <string>

#include "global.h" 

void drawText(float x, float y, const std::string& text, float r, float g, float b, void* font = GLUT_BITMAP_HELVETICA_18) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}


void drawRect(float x, float y, float width, float height, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}


void drawCircle(float cx, float cy, float r, int num_segments, float red, float green, float blue) {
    glColor3f(red, green, blue);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * M_PI * float(i) / float(num_segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta);
        glVertex2f(cx + x, cy + y);
    }
    glEnd();
}


void drawWindow(float x, float y, float w, float h) 
{
    drawRect(x, y, w, h, 0.45f, 0.55f, 0.65f);

    glColor3f(0.2f, 0.25f, 0.35f);
    glLineWidth(1.5f);
 
    glBegin(GL_LINE_LOOP);
         glVertex2f(x, y);
         glVertex2f(x + w, y);
         glVertex2f(x + w, y + h);
         glVertex2f(x, y + h);
    glEnd();

   
    glBegin(GL_LINES);
        // Vertical divider
            glVertex2f(x + w / 2.0f, y);
            glVertex2f(x + w / 2.0f, y + h);
        // Horizontal divider
            glVertex2f(x, y + h / 2.0f);
            glVertex2f(x + w, y + h / 2.0f);
    glEnd();
}


void drawTree(float x, float y, float scale) 
{

    drawRect(x - 0.03f * scale, y, 0.06f * scale, 0.3f * scale, 0.4f, 0.26f, 0.13f);

    drawCircle(x, y + 0.35f * scale, 0.18f * scale, 20, 0.35f, 0.6f, 0.25f);
    drawCircle(x - 0.1f * scale, y + 0.28f * scale, 0.14f * scale, 20, 0.3f, 0.55f, 0.2f);
    drawCircle(x + 0.1f * scale, y + 0.28f * scale, 0.14f * scale, 20, 0.4f, 0.65f, 0.3f);
    drawCircle(x, y + 0.22f * scale, 0.15f * scale, 20, 0.35f, 0.58f, 0.22f);
}

void drawBackround()
{

    drawRect(-1.0f, -1.0f, 2.0f, 2.0f, 0.8f, 0.88f, 0.95f);
    drawRect(-1.0f, -1.0f, 2.0f, 0.4f, 0.6f, 0.62f, 0.65f);
    drawRect(-0.85f, -0.3f, 1.7f, 0.9f, 0.88f, 0.82f, 0.68f);

}

void drawBuilding()
{
    glColor3f(0.75f, 0.68f, 0.55f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);                                          // building floors marking
    glVertex2f(-0.85f, 0.0f);  glVertex2f(0.85f, 0.0f);
    glVertex2f(-0.85f, 0.3f);  glVertex2f(0.85f, 0.3f);
    glVertex2f(-0.85f, 0.6f);  glVertex2f(0.85f, 0.6f);
    glEnd();


    float windowX[] = { -0.8f, -0.5f, -0.23f, 0.1f, 0.37f, 0.65f };
    for (float wx : windowX)
    {
        drawWindow(wx, -0.22f, 0.18f, 0.18f); // 1st Floor
        drawWindow(wx, 0.08f, 0.18f, 0.18f); // 2nd Floor
        drawWindow(wx, 0.38f, 0.18f, 0.18f); // 3rd Floor
    }

}

void drawCenterTower()
{
    drawRect(-0.12f, -0.3f, 0.24f, 1.05f, 0.85f, 0.78f, 0.64f); //Center Tower
    drawWindow(-0.06f, 0.12f, 0.12f, 0.15f);
    drawWindow(-0.06f, 0.38f, 0.12f, 0.15f);

    drawCircle(0.0f, 0.63f, 0.075f, 30, 0.4f, 0.35f, 0.3f);     // clock
    drawCircle(0.0f, 0.63f, 0.068f, 30, 0.98f, 0.98f, 0.95f);

    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(0.0f, 0.63f); glVertex2f(0.0f, 0.67f);   // Minute hand
    glVertex2f(0.0f, 0.63f); glVertex2f(0.025f, 0.63f); // Hour hand
    glEnd();

    drawRect(-0.16f, -0.3f, 0.32f, 0.04f, 0.9f, 0.85f, 0.75f);  // Roof overhang
    drawRect(-0.14f, -0.42f, 0.28f, 0.12f, 0.4f, 0.45f, 0.5f);  // Entrance 


}

void drawRailing()
{
   
    glColor3f(0.6f, 0.65f, 0.7f);
    glLineWidth(1.5f);
    // Railing horizontal lines
    glBegin(GL_LINES);
    glVertex2f(-0.85f, 0.63f); glVertex2f(-0.12f, 0.63f);
    glVertex2f(-0.85f, 0.68f); glVertex2f(-0.12f, 0.68f);

    glVertex2f(0.12f, 0.63f);  glVertex2f(0.85f, 0.63f);
    glVertex2f(0.12f, 0.68f);  glVertex2f(0.85f, 0.68f);
    glEnd();
   
    // Railing vertical posts
    glBegin(GL_LINES);
    for (float rx = -0.85f; rx <= -0.12f; rx += 0.06f) {
        glVertex2f(rx, 0.6f); glVertex2f(rx, 0.68f);
    }
    for (float rx = 0.12f; rx <= 0.85f; rx += 0.06f) {
        glVertex2f(rx, 0.6f); glVertex2f(rx, 0.68f);
    }
    glEnd();
}

void drawCollegeName()
{
    drawRect(-0.10f, 0.72f, 0.20f, 0.028f, 0.1f, 0.2f, 0.4f);
    drawText(-0.085f, 0.725f, "VIIT, Pune", 1.0f, 1.0f, 1.0f, GLUT_BITMAP_HELVETICA_12);
}

void drawCollege()
{
    drawBackround();
    drawBuilding();
    drawCenterTower();
    drawCollegeName();
    drawRailing();
    drawTree(0.55f, -0.46f, 1.0f);
}

