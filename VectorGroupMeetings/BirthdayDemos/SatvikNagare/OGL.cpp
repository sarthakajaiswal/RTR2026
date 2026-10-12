#include <stdio.h> 
#include <stdlib.h>
#include <cstdarg>
#include <string>
#include <windows.h>
#include <GL/freeglut.h> 
#include <math.h> 
#include <time.h> 
#include "global.h" 

bool bIsFullScreen = false; 
float ballX = 0.0f, ballY = 0.0f; 
float maxHeight = 1.0f; 
float radius = 0.1f; 

LARGE_INTEGER startTime, currentTime, freq; 
float mainTimer = 0.0f; 

FILE* gpFile = NULL; 

//Generic translation and scaling factors
float tx = 0.0f, ty = 0.0f, tz = 0.0f;
float sx = 1.0f, sy = 1.0f, sz = 1.0f;

// scene 1 related variables 
float schoolTx = 0.0f; 
float schoolTy = 0.0f; 
float schoolSx = 1.0f, schoolSy = 1.0f; 

// scene2 related variables 
float satvikTx = 0.0f, satvikTy = 0.0f; 
float satvikSx = 1.0f, satvikSy = 1.0f; 

// scene3 related variables 
float player1Tx = 0.0f, player2Tx = 0.0f;  
float footballTx = 0.0f; 

// scene4 related variables 
float hbdSx = 0.0f, hbdSy = 0.0f; 

int main(int argc, char *argv[]) 
{
    void initialize(void); 
    void uninitialize(void); 
    void resize(int, int); 
    void display(void); 
    void update(void); 
    void keyboard(unsigned char, int, int); 
    void mouse(int, int, int, int); 

    srand((unsigned int)time(NULL)); 

    glutInit(&argc, argv); 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); 
    glutInitWindowPosition(100, 100); 
    glutInitWindowSize(800, 800); 
    glutCreateWindow("Satvik Birthday Demo"); 

    initialize(); 

    glutReshapeFunc(resize); 
    glutDisplayFunc(display); 
    glutIdleFunc(update); 
    glutKeyboardFunc(keyboard); 
    glutMouseFunc(mouse); 
    glutCloseFunc(uninitialize); 

    TCHAR name[] = TEXT("SatvikAudio.wav"); 
    sndPlaySound(name, SND_ASYNC); 

    glutMainLoop(); 

    return 0; 
} 

void initialize(void) 
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 

    QueryPerformanceFrequency(&freq); 
    QueryPerformanceCounter(&startTime); 

    gpFile = fopen("log.txt", "w"); 
} 

void resize(int width, int height) 
{
    if (height <= 0) 
        height = 1; 

    glViewport(0, 0, (GLsizei)width, (GLsizei)height); 

    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity(); 
} 

void display(void) 
{
    // function declarations 
    void displayScene1(void); 
    void displayScene2(void); 
    void displayScene3(void); 
    void displayScene4(void); 
    void displayEnd(void); 

    // code 
    glClear(GL_COLOR_BUFFER_BIT); 

    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity(); 

    if(mainTimer < 4.0)
    {
        displayScene1(); 
    }
    else if(mainTimer < 11.0f)
    {
        displayScene2(); 
    } 
    else if(mainTimer < 19.5f)
    {
        displayScene3(); 
    }
    else if(mainTimer < 32.0f)
    {
        displayScene4(); 
    } 
    else 
    {
        displayEnd(); 
    } 

    if(mainTimer > 34.0f) 
        glutLeaveMainLoop(); 

    char timerStr[128]; 
    sprintf(timerStr, "%lf", mainTimer); 
    glutSetWindowTitle(timerStr); 

    // timer related code 
    QueryPerformanceCounter(&currentTime); 
    mainTimer = (float)(currentTime.QuadPart - startTime.QuadPart) / (float)freq.QuadPart; 
    if (gpFile) 
    {
        // fprintf(gpFile, "current time = %f\n", mainTimer); 
        fflush(gpFile);
    }
 
    glutSwapBuffers(); 
} 

void displayScene1(void) 
{
    // function declarations 
    void drawCollege(void); 

    // code 
    glPushMatrix(); 
    {
        glTranslatef(schoolTx, schoolTy, 0.0f); 
        glScalef(schoolSx, schoolSy, 0.0f); 
        drawCollege(); 
    } 
    glPopMatrix(); 
} 

void displayScene2(void) 
{
    // function declarations 
    void displayStudentWithChair(void); 

    glPushMatrix(); 
    {
        glTranslatef(satvikTx, satvikTy, 0.0f); 
        glScalef(satvikSx, satvikSy, 0.0f); 
        displayStudentWithChair(); 
    } 
    glPopMatrix(); 
} 

void displayScene3(void)
{
    // function declarations 
    void stadium(void); 
    void ground(void); 
    void goalpost(void); 
    void DrawFootball(float cx, float cy, float radius); 
    void DrawGround(void); 
    void DrawGoalPost(float x, float y, float width, float height); 
    void player(float cx, float base, float r, float g, float b, int keeper); 

   // Code
   stadium();
   ground();
   goalpost();

   glPushMatrix(); 
   {
        glTranslatef(footballTx, 0.0f, 0.0f); 
        DrawFootball(-0.45f, -0.365f, 0.035f); 
   } 
   glPopMatrix(); 
   
   glPushMatrix(); 
   {
        glTranslatef(player1Tx, 0.0f, 0.0f); 
        // glScalef(player1Sx, player1Sy, 0.0f);  
        player(-0.70f, -0.40f, 0.12f, 0.30f, 0.85f, 0); 
   } 
   glPopMatrix(); 

   glPushMatrix(); 
   {
        glTranslatef(player2Tx, 0.0f, 0.0f); 
        player(0.75f, -0.40f, 0.10f, 0.70f, 0.20f, 1);
   }
   glPopMatrix(); 
}

void displayScene4(void) 
{
    // function declarations 
    void displayHBD(void); 
    void displayLadyWithCake(void); 

    // code 
	//Background
	glColor3f(135.0f / 255.0f, 135.0f / 255.0f, 77.0f / 255.0f);
    glBegin(GL_QUADS); 
	glVertex2f(1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f); 
    glEnd(); 

    glPushMatrix(); 
    {
        glTranslatef(-0.55, 0.25, 0.0f);
        glScalef(0.70, 0.85, 0.0f);
        displayLadyWithCake(); 
    } 
    glPopMatrix(); 

    glPushMatrix(); 
    {
        glTranslatef(0.25, 0.0, 0.0f);
        glScalef(hbdSx, hbdSy, 0.0f); 
        displayHBD(); 
    } 
    glPopMatrix(); 
} 

void displayEnd(void) 
{
    // function declarations 
    void drawText(float x, float y, const char* text, ...); 

    // variable declarations 
    float textOpacity = 1.0f; 

    // code 
	glColor3f(115.0 / 255.0f, 251.0f / 255.0f, 253.0f / 255.0f);
    glBegin(GL_QUADS); 
	glVertex2f(1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f); 
    glEnd(); 

	glColor3f(1.0f, 1.0f, 1.0f);
	drawText(-0.3, 0.5, "*** Best  Wishes  From 9 Vectors ***");
	
	glColor4f(0.92f, 0.4f, 0.33f, textOpacity);	drawText(-0.1, 0.4, "Manasi Patil");
	glColor4f(0.85f, 0.15f, 0.15f, textOpacity); drawText(-0.1, 0.3, "Onkar Kulkarni");
	glColor4f(0.95f, 0.5f, 0.1f, textOpacity);	drawText(-0.1, 0.2, "Ranjit Jagtap");
	glColor4f(0.24f, 0.11f, 0.28f, textOpacity); drawText(-0.1, 0.1, "Ruchita Jaiswal");
	glColor4f(0.95f, 0.85f, 0.1f, textOpacity);	drawText(-0.1, 0.0, "Santosh Phalke");
	glColor4f(0.15f, 0.75f, 0.25f, textOpacity);	drawText(-0.1, -0.1, "Sarthak Jaiswal");
	glColor4f(0.15f, 0.45f, 0.85f, textOpacity);	drawText(-0.1, -0.2, "Shriram Dindore");
	glColor4f(0.29f, 0.0f, 0.51f, textOpacity);	drawText(-0.1, -0.3, "Vipin Mulkalwar");
	glColor4f(0.55f, 0.15f, 0.7f, textOpacity); drawText(-0.1, -0.4, "Shubham Shirsat");

	textOpacity += 0.003f;
} 

void drawText(float x, float y, const char* text, ...)
{
	
	char buffer[256];
	va_list args;

	va_start(args, text);
	std::vsnprintf(buffer, sizeof(buffer), text, args);
	va_end(args);

	glRasterPos2f(x, y);
	for (char* c = buffer; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
	}
}

void update(void) 
{
    // function declaration 
    void updateScene1(void); 
    void updateScene2(void); 
    void updateScene3(void); 
    void updateScene4(void); 

    if(mainTimer < 4.0)
    {
        updateScene1(); 
    }
    else if(mainTimer < 11.0f)
    {
        updateScene2(); 
    } 
    else if(mainTimer < 19.5f)
    {
        updateScene3(); 
    }
    else 
    {
        updateScene4(); 
    } 

    glutPostRedisplay(); 
} 

void updateScene1(void) 
{
    static float schoolAnimationSpeed = 0.005; 

    if(schoolTx < 2.40f)
    {
        schoolTx += schoolAnimationSpeed; 
    } 
    if(schoolTy > -1.15) 
    {
        schoolTy -= schoolAnimationSpeed; 
    } 

    if(schoolSx < 5.90)
    {
        schoolSx += schoolAnimationSpeed; 
    } 
    if(schoolSy < 6.90) 
    {
        schoolSy += schoolAnimationSpeed; 
    } 
} 

void updateScene2(void) 
{
    // variable declarations 
    static float satvikAnimationSpeed = 0.0005f; 

    // code 
    if(satvikTx > -0.55f) 
    {
        satvikTx -= satvikAnimationSpeed; 
    } 
    if(satvikTy > -0.45f)
    {
        satvikTy -= satvikAnimationSpeed; 
    } 

    if(satvikSx < 4.85f) 
    {
        satvikSx += satvikAnimationSpeed; 
    } 
    if(satvikSy < 4.25f) 
    {
        satvikSy += satvikAnimationSpeed; 
    } 
} 

void updateScene3(void) 
{
    // variable declarations 
    static float playerAnimationSpeed = 0.0004f; 

    if(player1Tx < 0.18f) 
    {
        player1Tx += playerAnimationSpeed; 
    } 
    else if(footballTx < 0.95f)
    {
        footballTx += 0.001; 
    } 

    if(footballTx > 0.65f && player2Tx > -0.25f) 
    {
        player2Tx -= 0.0004f;
    }  

} 

void updateScene4(void) 
{
    if(hbdSx < 1.0f) 
    {
        hbdSx += 0.001f; 
        hbdSy += 0.001f; 
    } 
} 

void keyboard(unsigned char key, int x, int y) 
{
    static float step = 0.05f;

    switch (key) 
    {
        case 27: 
            glutLeaveMainLoop(); 
            break; 

		case 's':
			ty -= step;
			display();
			break;

		case 'w':
			ty += step;
			display();
			break;

		case 'a':
			tx -= step;
			display();
			break;

		case  'd':
			tx += step;
			display();
			break;

		case 'z':
			sx -= step;
			display();
			break;

		case 'Z':
			sx += step;
			display();
			break;

		case 'x':
			sy -= step;
			display();
			break;

		case 'X':
			sy += step;
			display();
			break;

		case 'c':
			sy -= step;
			display();
			break;

		case 'C':
			sy += step;
			display();
			break;

		case 'p':
        case 'P': 
			fprintf(gpFile, "glTranslatef(%.2f, %.2f, 0.0f);\nglScalef(%.2f, %.2f, 0.0f);", tx, ty, sx, sy);
			break;

        case 'F':
        case 'f': 
            if (!bIsFullScreen) 
            {
                glutFullScreen(); 
                bIsFullScreen = true; 
            } 
            else 
            {
                glutLeaveFullScreen(); 
                glutReshapeWindow(800, 800);
                glutPositionWindow(100, 100);
                bIsFullScreen = false; 
            } 
            break; 
    } 
} 

void mouse(int button, int state, int x, int y) 
{
    switch (button) 
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
    if (gpFile) 
    {
        fclose(gpFile); 
        gpFile = NULL; 
    }
}

