#include <GL/freeglut.h>
#include <math.h>

float FRONT_W = 0.38f, FRONT_T = 0.52f, FRONT_B = 0.20f, BAR = 0.015f;
float BACK_W = 0.30f, BACK_T = 0.47f, BACK_B = 0.25f;

void circle(float cx, float cy, float r)
{
   glBegin(GL_TRIANGLE_FAN);
   glVertex2f(cx, cy);
   for (int i = 0; i <= 30; i++)
   {
       float a = i * 3.1415926f * 2.0f / 30.0f;
       glVertex2f(cx + r * cos(a), cy + r * sin(a));
   }
   glEnd();
}

void ground(void)
{
   int i;
   glColor3f(0.16f, 0.40f, 0.12f);
   glBegin(GL_QUADS);
       glVertex2f(-1.2f, -1.0f);
       glVertex2f( 1.2f, -1.0f);
       glVertex2f( 0.88f, 0.15f);
       glVertex2f(-0.88f, 0.15f);
   glEnd();
   glColor3f(0.24f, 0.52f, 0.17f);
   glLineWidth(1.5f);
   glBegin(GL_LINES);
       for (i = -4; i <= 4; i++)
       {
           glVertex2f(i * 0.28f, -1.0f);
           glVertex2f(i * 0.20f, 0.15f);
       }
       float yb[4] = { -0.72f, -0.42f, -0.12f, 0.08f };
       float xb[4] = { 1.14f, 1.06f, 0.97f, 0.90f };
       for (i = 0; i < 4; i++)
       {
           glVertex2f(-xb[i], yb[i]);
           glVertex2f( xb[i], yb[i]);
       }
   glEnd();
}

void stadium(void)
{
   int i, j;
   glColor3f(0.13f, 0.13f, 0.17f);
   glBegin(GL_QUAD_STRIP);
   for (i = 0; i <= 40; i++)
   {
       float x = -1.0f + 2.0f * (i / 40.0f);
       float top = 0.50f + 0.30f * x * x;
       glVertex2f(x, 0.15f);
       glVertex2f(x, top);
   }
   glEnd();
   glPointSize(2.5f);
   glBegin(GL_POINTS);
   for (j = 0; j < 5; j++)
   {
       for (i = 0; i <= 50; i++)
       {
           float x = -0.98f + 1.96f * (i / 50.0f);
           float top = 0.50f + 0.30f * x * x;
           float y = 0.21f + (top - 0.25f) * (j / 5.0f);
           float c = ((i + j) % 3) * 0.12f;
           glColor3f(0.90f - c, 0.85f - c, 0.70f + c);
           glVertex2f(x, y);
       }
   }
   glEnd();
}

void net(void)
{
   int i;
   glColor3f(0.72f, 0.72f, 0.72f);
   glBegin(GL_LINES);
       for (i = 0; i <= 16; i++)
       {
           float x = -BACK_W + (2.0f * BACK_W) * (i / 16.0f);
           glVertex2f(x, BACK_B);
           glVertex2f(x, BACK_T);
       }
       for (i = 0; i <= 8; i++)
       {
           float y = BACK_B + (BACK_T - BACK_B) * (i / 8.0f);
           glVertex2f(-BACK_W, y);
           glVertex2f( BACK_W, y);
       }
       for (i = 0; i <= 16; i++)
       {
           float t = i / 16.0f;
           glVertex2f(-FRONT_W + 2.0f * FRONT_W * t, FRONT_T);
           glVertex2f(-BACK_W + 2.0f * BACK_W * t, BACK_T);
       }
       for (i = 0; i <= 8; i++)
       {
           float t = i / 8.0f;
           glVertex2f( FRONT_W, FRONT_B + (FRONT_T - FRONT_B) * t);
           glVertex2f( BACK_W,  BACK_B + (BACK_T - BACK_B) * t);
           glVertex2f(-FRONT_W, FRONT_B + (FRONT_T - FRONT_B) * t);
           glVertex2f(-BACK_W,  BACK_B + (BACK_T - BACK_B) * t);
       }
   glEnd();
}

void right_vert_goalbar(void)
{
   glColor3f(0.75f, 0.76f, 0.74f);
   glBegin(GL_QUADS);
       glVertex2f(FRONT_W,       FRONT_B);
       glVertex2f(FRONT_W - BAR, FRONT_B);
       glVertex2f(FRONT_W - BAR, FRONT_T);
       glVertex2f(FRONT_W,       FRONT_T);
   glEnd();
}

void left_vert_goalbar(void)
{
   glPushMatrix();
       glScalef(-1.0f, 1.0f, 1.0f);
       right_vert_goalbar();
   glPopMatrix();
}

void horizontal_goalbar(void)
{
   glColor3f(0.75f, 0.76f, 0.74f);
   glBegin(GL_QUADS);
       glVertex2f(-FRONT_W, FRONT_T);
       glVertex2f( FRONT_W, FRONT_T);
       glVertex2f( FRONT_W, FRONT_T + BAR);
       glVertex2f(-FRONT_W, FRONT_T + BAR);
   glEnd();
}

void right_horz_back_goalbar(void)
{
   glColor3f(0.75f, 0.76f, 0.74f);
   glBegin(GL_QUADS);
       glVertex2f(FRONT_W,       FRONT_T);
       glVertex2f(FRONT_W - BAR, FRONT_T);
       glVertex2f(BACK_W - BAR,  BACK_T);
       glVertex2f(BACK_W,        BACK_T);
   glEnd();
}

void left_horz_back_goalbar(void)
{
   glPushMatrix();
       glScalef(-1.0f, 1.0f, 1.0f);
       right_horz_back_goalbar();
   glPopMatrix();
}

void horz_back_goalbar(void)
{
   glColor3f(0.75f, 0.76f, 0.74f);
   glBegin(GL_QUADS);
       glVertex2f(-BACK_W, BACK_T + BAR);
       glVertex2f( BACK_W, BACK_T + BAR);
       glVertex2f( BACK_W, BACK_T);
       glVertex2f(-BACK_W, BACK_T);
   glEnd();
}

void horz_back_ground_bar(void)
{
   glColor3f(0.75f, 0.76f, 0.74f);
   glBegin(GL_QUADS);
       glVertex2f(-BACK_W, BACK_B + BAR);
       glVertex2f( BACK_W, BACK_B + BAR);
       glVertex2f( BACK_W, BACK_B);
       glVertex2f(-BACK_W, BACK_B);
   glEnd();
}

void right_vert_back_post(void)
{
   glColor3f(0.75f, 0.76f, 0.74f);
   glBegin(GL_QUADS);
       glVertex2f(BACK_W,       BACK_B);
       glVertex2f(BACK_W - BAR, BACK_B);
       glVertex2f(BACK_W - BAR, BACK_T);
       glVertex2f(BACK_W,       BACK_T);
   glEnd();
}

void left_vert_back_post(void)
{
   glPushMatrix();
       glScalef(-1.0f, 1.0f, 1.0f);
       right_vert_back_post();
   glPopMatrix();
}

void goalpost(void)
{
   glPushMatrix();
   glTranslatef(0.55f, -0.55f, 0.0f);
   glScalef(0.75f, 0.75f, 1.0f);
   net();
   right_vert_back_post();
   left_vert_back_post();
   horz_back_goalbar();
   horz_back_ground_bar();
   right_horz_back_goalbar();
   left_horz_back_goalbar();
   right_vert_goalbar();
   left_vert_goalbar();
   horizontal_goalbar();
   glPopMatrix();
}

void player(float cx, float base, float r, float g, float b, int keeper)
{
   glColor3f(0.9f, 0.75f, 0.6f);
   glRectf(cx - 0.045f, base, cx - 0.01f, base + 0.08f);
   glRectf(cx + 0.01f, base, cx + 0.045f, base + 0.08f);
   glColor3f(0.95f, 0.95f, 0.95f);
   glRectf(cx - 0.055f, base + 0.07f, cx + 0.055f, base + 0.115f);
   glColor3f(r, g, b);
   glRectf(cx - 0.055f, base + 0.115f, cx + 0.055f, base + 0.19f);
   if (keeper)
   {
       glRectf(cx - 0.14f, base + 0.16f, cx - 0.055f, base + 0.19f);
       glRectf(cx + 0.055f, base + 0.16f, cx + 0.14f, base + 0.19f);
   }
   else
   {
       glRectf(cx - 0.08f, base + 0.115f, cx - 0.055f, base + 0.18f);
       glRectf(cx + 0.055f, base + 0.115f, cx + 0.08f, base + 0.18f);
   }
   glColor3f(0.9f, 0.75f, 0.6f);
   circle(cx, base + 0.215f, 0.028f);
}

void football(float cx, float cy, float r)
{
   glColor3f(1.0f, 1.0f, 1.0f);
   circle(cx, cy, r);
   glColor3f(0.0f, 0.0f, 0.0f);
   glBegin(GL_TRIANGLE_FAN);
   glVertex2f(cx, cy);
   for (int i = 0; i <= 5; i++)
   {
       float a = 1.5708f + i * 1.2566f;
       glVertex2f(cx + r * 0.34f * cos(a), cy + r * 0.34f * sin(a));
   }
   glEnd();
   for (int i = 0; i < 5; i++)
   {
       float a = 1.5708f + (i + 0.5f) * 1.2566f;
       circle(cx + r * 0.62f * cos(a), cy + r * 0.62f * sin(a), r * 0.16f);
   }
   glColor3f(0.0f, 0.0f, 0.0f);
   glBegin(GL_LINE_LOOP);
   for (int i = 0; i < 30; i++)
   {
       float a = i * 3.1415926f * 2.0f / 30.0f;
       glVertex2f(cx + r * cos(a), cy + r * sin(a));
   }
   glEnd();
}
