#include <GL/freeglut.h>
#include <math.h>

const float INSET_X = 0.08f; // horizontal perspective inset
const float INSET_Y = 0.05f; // vertical inset — smaller = lower angle / flatter field

const float FRONT_W = 0.38f; // front's width (half) — smaller = goal looks farther
const float FRONT_T = 0.52f; // front's top (where the crossbar meets)
const float FRONT_B = 0.20f; // front's bottom — raised so the goal sits near top of green
const float BAR = 0.015f;   // thickness of bards

// Back frame, derived from the two numbers above so nothing can drift apart
const float BACK_W = FRONT_W - INSET_X;   // 0.30
const float BACK_T = FRONT_T - INSET_Y;   // 0.47
const float BACK_B = FRONT_B + INSET_Y;   // 0.25

void stadium(void)
{
    // Far edge sits just BEHIND the back frame (y = BACK_B = 0.25), so the goal
    // sits almost at the top of the green. Raised horizon = flatter, deeper field.
    glColor3f(0.337f, 0.651f, 0.275f);
    glBegin(GL_QUADS);
        glVertex2f(-1.0f, -1.0f);   // near-left  (bottom edge of window)
        glVertex2f( 1.0f, -1.0f);   // near-right (bottom edge of window)
        glVertex2f( 0.60f, 0.28f);  // far-right
        glVertex2f(-0.60f, 0.28f);  // far-left
    glEnd();
}

void net(void)
{
    int i;

    glColor3f(0.72f, 0.72f, 0.72f);
    glBegin(GL_LINES);

        // back side strings
        for (i = 0; i <= 16; i++) // 16 vertical strings
        {
            float x = -BACK_W + (2.0f * BACK_W) * (i / 16.0f);
            glVertex2f(x, BACK_B);
            glVertex2f(x, BACK_T);
        }

        // back side strings
        for (i = 0; i <= 8; i++) // 8 horizontal strings
        {
            float y = BACK_B + (BACK_T - BACK_B) * (i / 8.0f);
            glVertex2f(-BACK_W, y);
            glVertex2f( BACK_W, y);
        }

        // Upper to back side strings : front -> back
        for (i = 0; i <= 16; i++)
        {
            float t = i / 16.0f;
            glVertex2f(-FRONT_W + 2.0f * FRONT_W * t, FRONT_T);
            glVertex2f(-BACK_W  + 2.0f * BACK_W  * t, BACK_T);
        }

        // both side strings : running front -> back
        for (i = 0; i <= 8; i++)
        {
            float t = i / 8.0f;
            glVertex2f( FRONT_W, FRONT_B + (FRONT_T - FRONT_B) * t);
            glVertex2f( BACK_W,  BACK_B  + (BACK_T  - BACK_B)  * t);
            glVertex2f(-FRONT_W, FRONT_B + (FRONT_T - FRONT_B) * t);
            glVertex2f(-BACK_W,  BACK_B  + (BACK_T  - BACK_B)  * t);
        }

    glEnd();
}

void right_vert_goalbar(void)
{
    glColor3f(0.443f, 0.451f, 0.439f);
    glBegin(GL_QUADS);
        glVertex2f(FRONT_W,       FRONT_B);   // bottom-right
        glVertex2f(FRONT_W - BAR, FRONT_B);   // bottom-left
        glVertex2f(FRONT_W - BAR, FRONT_T);   // top-left
        glVertex2f(FRONT_W,       FRONT_T);   // top-right
    glEnd();
}

void left_vert_goalbar(void)
{
    glPushMatrix();                  // 1. save current coordinates
        glScalef(-1.0f, 1.0f, 1.0f); // 2. flip the x-axis
        right_vert_goalbar();        // 3. same function -> now drawn mirrored (on the left)
    glPopMatrix();                   // 4. restore, so later shapes aren't flipped
}

void horizontal_goalbar(void)
{
    glColor3f(0.443f, 0.451f, 0.439f);
    glBegin(GL_QUADS);
        glVertex2f(-FRONT_W, FRONT_T);         // bottom-left
        glVertex2f( FRONT_W, FRONT_T);         // bottom-right
        glVertex2f( FRONT_W, FRONT_T + BAR);   // top-right
        glVertex2f(-FRONT_W, FRONT_T + BAR);   // top-left
    glEnd();
}

void right_horz_back_goalbar(void)
{
    glColor3f(0.443f, 0.451f, 0.439f);
    glBegin(GL_QUADS);
        glVertex2f(FRONT_W,       FRONT_T);   // near-right
        glVertex2f(FRONT_W - BAR, FRONT_T);   // near-left
        glVertex2f(BACK_W - BAR,  BACK_T);    // far-left
        glVertex2f(BACK_W,        BACK_T);    // far-right
    glEnd();
}

void left_horz_back_goalbar(void)
{
    glPushMatrix();
        glScalef(-1.0f, 1.0f, 1.0f);
        right_horz_back_goalbar();
    glPopMatrix();
}

void right_ground_rail(void)
{
    glColor3f(0.443f, 0.451f, 0.439f);
    glBegin(GL_QUADS);
        glVertex2f(FRONT_W,       FRONT_B);   // near-right
        glVertex2f(FRONT_W - BAR, FRONT_B);   // near-left
        glVertex2f(BACK_W - BAR,  BACK_B);    // far-left
        glVertex2f(BACK_W,        BACK_B);    // far-right
    glEnd();
}

void left_ground_rail(void)
{
    glPushMatrix();
        glScalef(-1.0f, 1.0f, 1.0f);
        right_ground_rail();
    glPopMatrix();
}

void horz_back_goalbar(void)
{
    glColor3f(0.443f, 0.451f, 0.439f);
    glBegin(GL_QUADS);
        glVertex2f(-BACK_W, BACK_T + BAR);   // top-left
        glVertex2f( BACK_W, BACK_T + BAR);   // top-right
        glVertex2f( BACK_W, BACK_T);         // bottom-right
        glVertex2f(-BACK_W, BACK_T);         // bottom-left
    glEnd();
}

void horz_back_ground_bar(void)
{
    glColor3f(0.443f, 0.451f, 0.439f);
    glBegin(GL_QUADS);
        glVertex2f(-BACK_W, BACK_B + BAR);   // top-left
        glVertex2f( BACK_W, BACK_B + BAR);   // top-right
        glVertex2f( BACK_W, BACK_B);         // bottom-right
        glVertex2f(-BACK_W, BACK_B);         // bottom-left
    glEnd();
}

void right_vert_back_post(void)
{
    glColor3f(0.443f, 0.451f, 0.439f);
    glBegin(GL_QUADS);
        glVertex2f(BACK_W,       BACK_B);   // bottom-right
        glVertex2f(BACK_W - BAR, BACK_B);   // bottom-left
        glVertex2f(BACK_W - BAR, BACK_T);   // top-left
        glVertex2f(BACK_W,       BACK_T);   // top-right
    glEnd();
}

void left_vert_back_post(void)
{
    glPushMatrix();
        glScalef(-1.0f, 1.0f, 1.0f);
        right_vert_back_post();
    glPopMatrix();
}

void displayGoalPost(void)
{
    stadium();
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
}
