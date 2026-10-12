#include <Windows.h> 
#include <stdio.h> 
#include <iostream> 
#include <math.h> 
#include <GL/freeglut.h> 

#define PI 3.1415926535897932384f
#define DEG_TO_RAD  3.14/180.0 

// window related variables 
bool bIsFullScreen = false; 

// scene related variables 
int scene_number = 1; 

// transformation related variables 
float tx = 0.0f, ty = 0.0f, tz = 0.0f;
float sx = 1.0f, sy = 1.0f, sz = 1.0f;

// log file related 
FILE* logFile = NULL; 

// animation related variables 
// scene5 
float hanumaanSx = 0.50, hanumaanSy = 0.50; 
float hanumaanTx = -0.45, hanumaanTy = -0.34; 
// scene1 
float birdTx = 0.2f, birdTy = 0.0f; 
// scene2 
bool bDrawRTR = true, bDrawVector = false, bDrawGroup = false; 
float rtrScale=0.0f, vectorScale=0.0f, groupScale=0.0f; 
// scene 4 
float scene4HanumanTx = -0.60, scene4HanumaanTy = 1.5; 
// scene3 
float flyingHanumanTx = -2.25f, flyingHanumanTy = 0.38; 
float flyingHanumanSx = 0.30, flyingHanumanSy = 0.50; 

// helper functions 
void drawArc(float cx, float cy, float radiusx, float radiusy, int segments, float startAngle, float stopAngle);
void drawArcRotate(float cx, float cy, float radiusx, float radiusy, int segments, float startAngle, float stopAngle, float tiltFactorY);
void drawPlaneArc(float cx, float cy, float radiusx, float radiusy, int segments, float startAngle, float stopAngle);

void drawOval(float centerX, float centerY, float radiusX, float radiusY, int segments); 

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
void drawQuad(float x, float y, float w, float h);
void drawCircleWithSlices(float cx, float cy, float r, int slices);
void drawWave(float horizontalShift, float verticalShift);
void drawEllipse(float cx, float cy, float rx, float ry, float angle_start, float angle_end);

// object functions 
void drawBird(void);
void drawHanuman(void);
void jambavan(void);

void drawRocks(void); 

void drawBoat(float x, float y, float scale);
void drawTemple(float baseX, float baseY, float width, float bodyHeight, int tiers);
void drawTree(float baseX, float baseY, float height, float width);
void drawRamSetu(void);
void drawSea(void); 
void drawLanka(void); 

void DrawRTR(void);
void DrawVector(void);
void DrawGroup(void);
void DrawBackground(void);

void drawFlyingHanuman(void); 
void drawClouds(void); 

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
    glutInitWindowSize(800, 800); 
    glutCreateWindow("My first RTR window"); 

    initialize(); 

    glutReshapeFunc(resize); 
    glutDisplayFunc(display); 
    glutIdleFunc(update); 
    glutKeyboardFunc(keyboard); 
    glutMouseFunc(mouse); 
    glutCloseFunc(uninitialize); 
 
    TCHAR name[] = TEXT("AsaHaEkachShriHanuman.wav"); 
    sndPlaySound(name, SND_ASYNC); 

    glutMainLoop(); 

    // control flow should not come here 
    return (0); 
} 

void initialize(void) 
{
    // code 
    glClearColor(0.75f, 0.75f, 0.75f, 1.0f); 

    logFile = fopen("logFile.txt", "w"); 
    if(logFile == NULL) 
    {
        printf("log file creation failed\n"); 
        exit(EXIT_FAILURE); 
    } 
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
    void drawText(float x, float y, const char* text, ...); 
    void drawCircle(float, float, float); 

    glClear(GL_COLOR_BUFFER_BIT); 

    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity(); 

    if(scene_number == 1 || scene_number == 5) 
    {
        // Sky
        glPushMatrix(); 
        { 
            glTranslatef(0.00, 0.46, 0.00);
            glScalef(1.00, 1.00, 1.00);

            glBegin(GL_QUADS);
			glColor3f(0.55f, 0.75f, 0.95f);
            glVertex2f(-1.0f, -0.1f);
			glColor3f(0.996f, 0.671f, 0.514f);
            glVertex2f(1.0f, -0.1f);
            glColor3f(0.15f, 0.35f, 0.75f);
			glVertex2f(1.0f, 1.0f);
			glVertex2f(-1.0f, 1.0f);
            glEnd();
        }
        glPopMatrix();  

		//Bird
		glPushMatrix(); 
		{
			glTranslatef(birdTx, birdTy, 0.0f); 

			glPushMatrix();
			{
				glTranslatef(0.58, 0.37, 0.00);
				glScalef(0.40, 0.70, 1.00);
				drawBird();
			}
			glPopMatrix();

			glPushMatrix();
			{
				glTranslatef(0.43, 0.36, 0.00);
				glScalef(0.30, 0.40, 1.00);
				drawBird();
			}
			glPopMatrix();

			glPushMatrix();
			{
				glTranslatef(-0.20, 0.52, 0.00);
				glScalef(0.40, 0.40, 1.00);
				drawBird();
			}
			glPopMatrix();
		} 
		glPopMatrix(); 

        // ground 
        glPushMatrix(); 
        {
            glTranslatef(-1.00, -1.34, 0.00);
            glScalef(2.20, 1.00, 1.00);
            glColor3f(0.09f, 0.196f, 0.152f);
            drawQuad(0.0, 0.0, 1.0, 1.0); 
        }
        glPopMatrix();  

        // sea 
        glPushMatrix(); 
        {
            glTranslatef(0.00, 0.66, 0.00);
            drawSea(); 
        } 
        glPopMatrix(); 

		//rocks
		glPushMatrix();
		{
			glTranslatef(-0.76, 0.08, 0.00);
			glScalef(-0.30, 1.80, 1.00);
			drawRocks();
		}
		glPopMatrix();

        // hanumaan and jambavan 
        glPushMatrix(); 
        {
            glTranslatef(hanumaanTx, hanumaanTy, 0.00);
            glScalef(hanumaanSx, hanumaanSy, 1.00);  
			drawHanuman();
        } 
        glPopMatrix(); 
        glPushMatrix(); 
        {
            glTranslatef(-0.30, -0.34, 0.00);
            glScalef(0.70, 0.70, 1.00);  
            jambavan();
        } 
        glPopMatrix(); 

        // lanka 
        glPushMatrix(); 
        {
            glTranslatef(0.63, 0.39, 0.00);
            glScalef(0.40, 0.30, 1.00);

            drawTemple(-0.12f, -0.10f, 0.24f, 0.34f, 5);    // center big temple
            drawTemple(-0.31f, -0.10f, 0.16f, 0.24f, 4);    // left side temple
            drawTemple(0.15f, -0.10f, 0.16f, 0.24f, 4);     // right side temple
            drawTemple(-0.51f, -0.10f, 0.11f, 0.16f, 3);    // left smallest temple
            drawTemple(0.4f, -0.10f, 0.11f, 0.16f, 3);      // right smallest temple
        } 
        glPopMatrix(); 

        // rocks
        glPushMatrix(); 
        {
            glTranslatef(1.32, 0.13, 0.00);
            glScalef(1.00, 2.00, 1.00);
            drawRocks(); 
        } 
        glPopMatrix(); 

		glPushMatrix();
		{
			glTranslatef(-1.91, 0.48, 0.00);
			glScalef(1.10, 5.90, 1.00);
		    drawRocks(); 
		} 
		glPopMatrix(); 

		glPushMatrix();
		{
			glTranslatef(0.80, -0.40, 0.00);
			glScalef(-0.40, 2.70, 1.00);
			drawRocks();
		}
		glPopMatrix();
		
    } 
    else if(scene_number == 2) 
    {
        DrawBackground();
		if(bDrawRTR == true) 
		{
			glPushMatrix(); 
			glScalef(rtrScale, rtrScale, 0.0); 
        	DrawRTR();
			glPopMatrix(); 
		} 
		if(bDrawVector == true) 
		{
			glPushMatrix(); 
			glScalef(vectorScale, vectorScale, 0.0); 
        	DrawVector();
			glPopMatrix(); 
		} 
		if(bDrawGroup == true) 
		{
			glPushMatrix(); 
			glScalef(groupScale, groupScale, 0.0); 
        	DrawGroup();
			glPopMatrix(); 
		} 
    }
    else if(scene_number == 3) 
    {
        glPushMatrix(); 
        {
            glTranslatef(0.08, 0.10, 0.00);
            glScalef(1.20, 1.10, 1.00);
            drawSea(); 
        } 
        glPopMatrix(); 

		// clouds 
		glPushMatrix(); 
        {
            glTranslatef(-0.61, 0.50, 0.00);
			glScalef(1.20, 1.00, 1.00);
            drawClouds(); 
        } 
        glPopMatrix(); 
        glPushMatrix(); 
        {
			glTranslatef(1.05, 0.38, 0.00);
			glScalef(1.50, 1.00, 1.00);
            drawClouds(); 
        } 
        glPopMatrix(); 

		// hanumaan 
		glPushMatrix(); 
		{
		 	glTranslatef(flyingHanumanTx, flyingHanumanTy, 0.0); 
			glScalef(flyingHanumanSx, flyingHanumanSx, 0.0); 
			// glTranslatef(tx, ty, tz); 
			// glScalef(sx, sy, sz); 
			drawFlyingHanuman(); 
		} 
		glPopMatrix(); 

    } 
    else if(scene_number == 4) 
    {
        drawLanka();
     
        glPushMatrix(); 
        {
            glTranslatef(scene4HanumanTx, scene4HanumaanTy, 0.00);
            glScalef(0.20, 0.20, 1.00);
            drawHanuman(); 
        } 
        glPopMatrix(); 
    } 
    else if(scene_number == 6) 
    {
        glColor3f(115.0/255.0f, 251.0f/255.0f, 253.0f/255.0f); 
        drawQuad(-1.0f, 1.0f, 2.0f, 2.0f); 

        glColor3f(1.0f, 0.0f, 0.0f); 
        drawText(-0.09, 0.7, "Members"); 
        glColor3f(0.09f, 0.196f, 0.152f);
        drawText(-0.1, 0.6, "Manasi Patil"); 
        drawText(-0.11, 0.5, "Onkar Kulkarni"); 
        drawText(-0.1, 0.4, "Ranjit Jagtap"); 
        drawText(-0.11, 0.3, "Ruchita Jaiswal"); 
        drawText(-0.11, 0.2, "Santosh Phalke"); 
        drawText(-0.1, 0.1, "Satvik Nagare"); 
        drawText(-0.11, 0.0, "Shriram Dindore"); 
        drawText(-0.12, -0.1, "Shubham Shirsat"); 
        drawText(-0.11, -0.2, "Vipin Mulkalwar"); 

        glColor3f(1.0f, 0.0f, 0.0f);
        drawText(-0.1, -0.35, "Project Lead:"); 
        glColor3f(0.09f, 0.196f, 0.152f);
        drawText(-0.11, -0.45, "Vipin Mulkalwar"); 
        drawText(-0.11, -0.55, "Shubham Shirsat"); 

        glColor3f(1.0f, 0.0f, 0.0f);
        drawText(-0.1, -0.7, "Group Lead"); 
        glColor3f(0.09f, 0.196f, 0.152f);
        drawText(-0.11, -0.8, "Sarthak Jaiswal"); 
    } 

    glutSwapBuffers(); 
} 

void drawBird(void)
{
	glBegin(GL_LINE_STRIP);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.45f, 0.6f, 0.0f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.475f, 0.57f, 0.0f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.5f, 0.6f, 0.0f);

	glEnd();

	glBegin(GL_LINE_STRIP);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.6f, 0.5f, 0.0f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.625f, 0.475f, 0.0f);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.65f, 0.5f, 0.0f);

	glEnd();

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

void jambavan(void)
{
	//code

	//Stomach
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.71f, 0.0f, 0.51f);
	drawArc(0.65f, -0.35f, 0.12f, 0.05f, 64, 90.0f, 270.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.65f, -0.35f, 0.0f);
	glVertex3f(0.53f, -0.35f, 0.0f);
	glVertex3f(0.56f, -0.20f, 0.0f);
	glVertex3f(0.65f, -0.20f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.8f, 0.392f, 0.686f);
	glVertex3f(0.65f, -0.35f, 0.0f);
	glVertex3f(0.53f, -0.35f, 0.0f);
	glVertex3f(0.543f, -0.28f, 0.0f);
	glVertex3f(0.65f, -0.30f, 0.0f);
	glEnd();

	//Left Leg
	glBegin(GL_TRIANGLES);
	glColor3f(0.722f, 0.0f, 0.518f);
	glVertex3f(0.53f, -0.35f, 0.0f);
	glVertex3f(0.56f, -0.385f, 0.0f);
	glVertex3f(0.526f, -0.42f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.56f, -0.385f, 0.0f);
	glVertex3f(0.535f, -0.41f, 0.0f);
	glVertex3f(0.555f, -0.45f, 0.0f);
	glEnd();

	//Jwellery
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(0.555f, -0.43f, 0.0f);
	glVertex3f(0.545f, -0.43f, 0.0f);
	glVertex3f(0.541f, -0.42f, 0.0f);
	glVertex3f(0.558f, -0.42f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.555f, -0.45f, 0.0f);
	glVertex3f(0.545f, -0.43f, 0.0f);
	glVertex3f(0.525f, -0.45f, 0.0f);
	glEnd();

	//Right Leg
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.65f, -0.40f, 0.0f);
	glVertex3f(0.66f, -0.47f, 0.0f);
	glVertex3f(0.61f, -0.39f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.66f, -0.47f, 0.0f);
	glVertex3f(0.63f, -0.42f, 0.0f);
	glVertex3f(0.65f, -0.49f, 0.0f);
	glEnd();

	//Jwellery
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(0.66f, -0.47f, 0.0f);
	glVertex3f(0.645f, -0.478f, 0.0f);
	glVertex3f(0.645f, -0.468f, 0.0f);
	glVertex3f(0.66f, -0.46f, 0.0f);
	glEnd();
	
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.66f, -0.47f, 0.0f);
	glVertex3f(0.65f, -0.49f, 0.0f);
	glVertex3f(0.68f, -0.50f, 0.0f);
	glEnd();

	//Chest
	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.56f, -0.20f, 0.0f);
	glVertex3f(0.65f, -0.20f, 0.0f);
	glVertex3f(0.65f, -0.14f, 0.0f);
	glVertex3f(0.57f, -0.14f, 0.0f);
	glEnd();

	//Head
	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.65f, -0.14f, 0.0f);
	glVertex3f(0.57f, -0.14f, 0.0f);
	glVertex3f(0.58f, -0.10f, 0.0f);
	glVertex3f(0.65f, -0.10f, 0.0f);
	glEnd();

	//Chain
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(0.65f, -0.14f, 0.0f);
	glVertex3f(0.57f, -0.14f, 0.0f);
	glVertex3f(0.57f, -0.13f, 0.0f);
	glVertex3f(0.65f, -0.13f, 0.0f);
	glEnd();

	//Mouth
	glBegin(GL_QUADS);
	glColor3f(0.384f, 0.357f, 0.71f);
	glVertex3f(0.58f, -0.10f, 0.0f);
	glVertex3f(0.54f, -0.03f, 0.0f);
	glVertex3f(0.55f, -0.01f, 0.0f);
	glVertex3f(0.63f, -0.025f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.55f, -0.05f, 0.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.56f, -0.07f, 0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.58f, -0.06f, 0.0f);
	
	glEnd();

	//Nose
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.0f, 0.0f, 0.0f);
	drawArc(0.55f, -0.022f, 0.009f, 0.014f, 64, 0.0f, 270.0f);
	glEnd();

	//Forehaed
	glBegin(GL_LINE_STRIP);
	glColor3f(0.384f, 0.357f, 0.71f);
	drawPlaneArc(0.58f, 0.01f, 0.04f, 0.03f, 64, 215.0f, 360.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.58f, -0.10f, 0.0f);
	glVertex3f(0.62f, 0.01f, 0.0f);
	glVertex3f(0.65f, 0.0f, 0.0f);
	glVertex3f(0.65f, -0.10f, 0.0f);
	glEnd();

	//Mukut
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(0.62f, 0.05f, 0.0f);
	glVertex3f(0.67f, 0.12f, 0.0f);
	glVertex3f(0.658f, 0.02f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.761f, 0.118f, 0.337f);
	glVertex3f(0.622f, 0.02f, 0.0f);
	glVertex3f(0.62f, 0.05f, 0.0f);
	glVertex3f(0.66f, 0.03f, 0.0f);
	glVertex3f(0.652f, 0.01f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(0.62f, 0.01f, 0.0f);
	glVertex3f(0.622f, 0.02f, 0.0f);
	glVertex3f(0.652f, 0.01f, 0.0f);
	glVertex3f(0.65f, 0.0f, 0.0f);
	glEnd();

	//Left Hand
	glBegin(GL_POLYGON);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.56f, -0.20f, 0.0f);
	glVertex3f(0.52f, -0.16f, 0.0f);
	glVertex3f(0.50f, -0.10f, 0.0f);
	glVertex3f(0.52f, -0.08f, 0.0f);
	glVertex3f(0.57f, -0.14f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(0.50f, -0.10f, 0.0f);
	glVertex3f(0.52f, -0.08f, 0.0f);
	glVertex3f(0.51f, -0.07f, 0.0f);
	glVertex3f(0.50f, -0.08f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.51f, -0.07f, 0.0f);
	glVertex3f(0.50f, -0.08f, 0.0f);
	glVertex3f(0.48f, -0.06f, 0.0f);
	glEnd();


	//Right Hand
	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.65f, -0.20f, 0.0f);
	glVertex3f(0.65f, -0.14f, 0.0f);
	glVertex3f(0.71f, -0.10f, 0.0f);
	glVertex3f(0.73f, -0.12f, 0.0f);
	glVertex3f(0.70f, -0.16f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(0.71f, -0.10f, 0.0f);
	glVertex3f(0.73f, -0.12f, 0.0f);
	glVertex3f(0.72f, -0.13f, 0.0f);
	glVertex3f(0.70f, -0.105f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 0.0f, 0.0f);;
	glVertex3f(0.71f, -0.10f, 0.0f);
	glVertex3f(0.72f, -0.11f, 0.0f);
	glVertex3f(0.73f, -0.10f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.73f, -0.10f, 0.0f);
	glVertex3f(0.75f, -0.11f, 0.0f);
	glVertex3f(0.73f, -0.12f, 0.0f);
	glVertex3f(0.72f, -0.11f, 0.0f);
	glEnd();

	//eye
	
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.0f, 0.0f, 0.0f);
	drawPlaneArc(0.608f, -0.019f, 0.008f, 0.01f, 64, 0.0f, 360.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 1.0f, 1.0f);
	drawPlaneArc(0.61f, -0.019f, 0.003f, 0.006f, 64, 0.0f, 360.0f);
	glEnd();


}

void drawHanuman(void)
{
	//code
	
	//Chest
	glBegin(GL_QUADS);
	glColor3f(0.349f, 0.211f, 0.137f);
	glVertex3f(-0.18f, -0.2f, 0.0f);
	glVertex3f(0.10f, -0.2f, 0.0f);
	glVertex3f(0.15f, 0.2f, 0.0f);
	glVertex3f(-0.23f, 0.2f, 0.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(-0.12f, 0.2f, 0.0f);
	glVertex3f(-0.11f, 0.0f, 0.0f);
	glVertex3f(0.02f, 0.0f, 0.0f);
	glVertex3f(0.04f, 0.2f, 0.0f);
	glEnd();

	//Jwellery

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.525f, 0.0f);
	drawArc(-0.04f, 0.2f, 0.05f, 0.05f, 64, 180.0f, 360.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(1.0f, 0.749f, 0.0f);
	drawArc(-0.04f, 0.2f, 0.08f, 0.10f, 64, 180.0f, 360.0f);
	glEnd();
	
	//Waist Line

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.525f, 0.0f);
	glVertex3f(0.13f, -0.18f, 0.0f);
	glVertex3f(-0.20f, -0.15f, 0.0f);
	glVertex3f(-0.20f, -0.21f, 0.0f);
	glVertex3f(0.13f, -0.21f, 0.0f);
	glEnd();
	
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.525f, 0.0f);
	drawArc(-0.20f, -0.175f, 0.025f, 0.025f, 16, 90.0f, 270.0f);
	glEnd();

	//left leg
	glBegin(GL_QUADS);
	glColor3f(0.349f, 0.211f, 0.137f);
	glVertex3f(-0.02f, -0.58f, 0.0f);
	glVertex3f(-0.02f, -0.50f, 0.0f);
	glVertex3f(-0.20f, -0.42f, 0.0f);
	glVertex3f(-0.20f, -0.66f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(-0.02f, -0.58f, 0.0f);
	glVertex3f(-0.02f, -0.50f, 0.0f);
	glVertex3f(-0.03f, -0.49f, 0.0f);
	glVertex3f(-0.03f, -0.59f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.349f, 0.211f, 0.137f);
	drawArc(-0.20f, -0.54f, 0.06f, 0.12f, 64, 90.0f, 270.0f);
	glEnd();

	// Dhoti
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(135.0f/255.0f, 33.0f/255.0f, 28.0f/255.0f);
	//glColor3f(255.0f/255.0f, 143.0f/255.0f, 41.0f/255.0f);
	drawArc(-0.20f, -0.20f, 0.32f, 0.35f, 64, 260.0f, 360.0f);
	glEnd();

	//Right leg
	glBegin(GL_QUADS);
	glColor3f(0.349f, 0.211f, 0.137f);
	glVertex3f(0.04f, -0.32f, 0.0f);
	glVertex3f(0.20f, -0.32f, 0.0f);
	glVertex3f(0.16f, -0.56f, 0.0f);
	glVertex3f(0.08f, -0.56f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(0.075f, -0.548f, 0.0f);
	glVertex3f(0.165f, -0.548f, 0.0f);
	glVertex3f(0.16f, -0.56f, 0.0f);
	glVertex3f(0.08f, -0.56f, 0.0f);
	glEnd();


	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.349f, 0.211f, 0.137f);
	drawArc(0.12f, -0.32f, 0.08f, 0.08f, 64, 0.0f, 180.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.349f, 0.211f, 0.137f);
	drawArc(0.095f, -0.56f, 0.008f, 0.05f, 8, 180.0f, 360.0f);
	drawArc(0.115f, -0.56f, 0.006f, 0.06f, 8, 180.0f, 360.0f);
	drawArc(0.125f, -0.56f, 0.006f, 0.06f, 8, 180.0f, 360.0f);
	drawArc(0.135f, -0.56f, 0.006f, 0.06f, 8, 180.0f, 360.0f);
	drawArc(0.145f, -0.56f, 0.006f, 0.06f, 8, 180.0f, 360.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(0.0f, 0.0f, 0.0f);
	drawArc(0.095f, -0.56f, 0.008f, 0.05f, 8, 180.0f, 360.0f);
	drawArc(0.115f, -0.56f, 0.006f, 0.06f, 8, 180.0f, 360.0f);
	drawArc(0.125f, -0.56f, 0.006f, 0.06f, 8, 180.0f, 360.0f);
	drawArc(0.135f, -0.56f, 0.006f, 0.06f, 8, 180.0f, 360.0f);
	drawArc(0.145f, -0.56f, 0.006f, 0.06f, 8, 180.0f, 360.0f);
	glEnd();

	//Left Leg Fingers
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.349f, 0.211f, 0.137f);
	drawArc(-0.02f, -0.52f, 0.06f, 0.02f, 8, 0.0f, 90.0f);
	drawArc(-0.02f, -0.545f, 0.05f, 0.02f, 8, 0.0f, 90.0f);
	drawArc(-0.02f, -0.555f, 0.05f, 0.02f, 8, 0.0f, 90.0f);
	drawArc(-0.02f, -0.565f, 0.05f, 0.02f, 8, 0.0f, 90.0f);
	drawArc(-0.02f, -0.575f, 0.05f, 0.02f, 8, 0.0f, 90.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(0.0f, 0.0f, 0.0f);
	drawArc(-0.02f, -0.52f, 0.06f, 0.02f, 8, 0.0f, 90.0f);
	drawArc(-0.02f, -0.545f, 0.05f, 0.02f, 8, 0.0f, 90.0f);
	drawArc(-0.02f, -0.555f, 0.05f, 0.02f, 8, 0.0f, 90.0f);
	drawArc(-0.02f, -0.565f, 0.05f, 0.02f, 8, 0.0f, 90.0f);
	drawArc(-0.02f, -0.575f, 0.05f, 0.02f, 8, 0.0f, 90.0f);
	glEnd();


	//GADHA MUTH
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(-0.09f, -0.03f, 0.0f);
	glVertex3f(-0.28f, 0.42f, 0.0f);
	glVertex3f(-0.22f, 0.48f, 0.0f);
	glColor3f(1.0f, 0.51f, 0.0f);
	glVertex3f(-0.08f, -0.02f, 0.0f);
	glEnd();

	//Left Hand
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.349f, 0.211f, 0.137f);
	drawArc(-0.30f, 0.19f, 0.08f, 0.08f, 64, 0.0f, 360.0f);
	glEnd();


	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.749f, 0.0f);
	drawArc(-0.30f, 0.19f, 0.08f, 0.08f, 64, 180.0f, 360.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.349f, 0.211f, 0.137f);
	glVertex3f(-0.36f, 0.17f, 0.0f);
	glVertex3f(-0.23f, 0.17f, 0.0f);
	glVertex3f(-0.23f, 0.10f, 0.0f);
	glVertex3f(-0.36f, -0.08f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.349f, 0.211f, 0.137f);
	glVertex3f(-0.23f, 0.10f, 0.0f);
	glVertex3f(-0.16f, 0.10f, 0.0f);
	glVertex3f(-0.16f, 0.02f, 0.0f);
	glVertex3f(-0.36f, -0.08f, 0.0f);
	glEnd();

	//Thumb
	glBegin(GL_QUADS);
	glColor3f(0.349f, 0.211f, 0.137f);
	glVertex3f(-0.14f, 0.00f, 0.0f);
	glVertex3f(-0.12f, 0.01f, 0.0f);
	glVertex3f(-0.13f, 0.14f, 0.0f);
	glVertex3f(-0.15f, 0.14f, 0.0f);
	glEnd();

	//Thumb
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-0.14f, 0.00f, 0.0f);
	glVertex3f(-0.12f, 0.01f, 0.0f);
	glVertex3f(-0.13f, 0.14f, 0.0f);
	glVertex3f(-0.15f, 0.14f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.843f, 0.0f);
	drawArc(-0.16f, 0.06f, 0.04f, 0.04f, 64, 0.0f, 90.0f);
	drawArc(-0.16f, 0.06f, 0.04f, 0.04f, 64, 270.0f, 360.0f);
	glEnd();

	//Fingers

	glBegin(GL_QUADS);
	glColor3f(0.349f, 0.211f, 0.137f);
	glVertex3f(-0.12f, 0.01f, 0.0f);
	glVertex3f(-0.08f, 0.05f, 0.0f);
	glVertex3f(-0.09f, 0.12f, 0.0f);
	glVertex3f(-0.125f, 0.10f, 0.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-0.12f, 0.01f, 0.0f);
	glVertex3f(-0.08f, 0.05f, 0.0f);
	glVertex3f(-0.09f, 0.12f, 0.0f);
	glVertex3f(-0.125f, 0.10f, 0.0f);
	glEnd();

	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-0.11f, 0.11f, 0.0f);
	glVertex3f(-0.10f, 0.03f, 0.0f);

	glVertex3f(-0.08f, 0.065f, 0.0f);
	glVertex3f(-0.103f, 0.045f, 0.0f);

	glVertex3f(-0.086f, 0.077f, 0.0f);
	glVertex3f(-0.106f, 0.065f, 0.0f);

	glVertex3f(-0.087f, 0.093f, 0.0f);
	glVertex3f(-0.108f, 0.081f, 0.0f);

	glEnd();

	//Gadda

	//Small Circle
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.83f, 0.69f, 0.22f);
	drawArc(-0.081f, -0.03f, 0.01f, 0.01f, 64, 0.0f, 360.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(-0.28f, 0.42f, 0.0f);
	glVertex3f(-0.22f, 0.48f, 0.0f);
	glColor3f(1.0f, 0.51f, 0.0f);
	glVertex3f(-0.22f, 0.57f, 0.0f);
	glColor3f(1.0f, 0.51f, 0.0f);
	glVertex3f(-0.33f, 0.47f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(-0.20f, 0.58f, 0.0f);
	glVertex3f(-0.35f, 0.45f, 0.0f);
	glVertex3f(-0.37f, 0.48f, 0.0f);
	glVertex3f(-0.21f, 0.62f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.51f, 0.0f);
	glVertex3f(-0.22f, 0.61f, 0.0f);
	glColor3f(1.0f, 0.51f, 0.0f);
	glVertex3f(-0.23f, 0.64f, 0.0f);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(-0.36f, 0.52f, 0.0f);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(-0.355f, 0.48f, 0.0f);
	glEnd();


	//Big Circle
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.51f, 0.0f);
	drawArcRotate(-0.295f, 0.580f, 0.06f, 0.90f, 64, 24.0f, 210.0f, 0.12f);
	glEnd();

	//Top Small Circle
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.83f, 0.69f, 0.22f);
	drawArcRotate(-0.33f, 0.70f, 0.01f, 0.24f, 64, 0.0f, 360.0f, 0.10f);
	glEnd();

	//Right Hand
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.349f, 0.211f, 0.137f);
	drawArc(0.22f, 0.19f, 0.08f, 0.08f, 64, 0.0f, 360.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.349f, 0.211f, 0.137f);
	glVertex3f(0.18f, 0.26f, 0.0f);
	glVertex3f(0.295f, 0.16f, 0.0f);
	glVertex3f(0.34f, 0.28f, 0.0f);
	glVertex3f(0.30f, 0.33f, 0.0f);
	glEnd();

	glBegin(GL_POLYGON);
	glColor3f(0.349f, 0.211f, 0.137f);
	glVertex3f(0.34f, 0.28f, 0.0f);
	glVertex3f(0.30f, 0.32f, 0.0f);
	glVertex3f(0.12f, 0.43f, 0.0f);
	glVertex3f(0.12f, 0.47f, 0.0f);
	glVertex3f(0.36f, 0.47f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(0.18f, 0.26f, 0.0f);
	glVertex3f(0.295f, 0.16f, 0.0f);
	glVertex3f(0.30f, 0.18f, 0.0f);
	glVertex3f(0.20f, 0.27f, 0.0f);
	glEnd();


	glBegin(GL_QUADS);
	glColor3f(0.349f, 0.211f, 0.137f);
	glVertex3f(0.12f, 0.47f, 0.0f);
	glVertex3f(0.04f, 0.47f, 0.0f);
	glVertex3f(0.01f, 0.45f, 0.0f);
	glVertex3f(0.07f, 0.45f, 0.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.12f, 0.47f, 0.0f);
	glVertex3f(0.04f, 0.47f, 0.0f);
	glVertex3f(0.01f, 0.45f, 0.0f);
	glVertex3f(0.07f, 0.45f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.349f, 0.211f, 0.137f);
	glVertex3f(0.05f, 0.43f, 0.0f);
	glVertex3f(0.04f, 0.44f, 0.0f);
	glVertex3f(0.10f, 0.47f, 0.0f);
	glVertex3f(0.10f, 0.44f, 0.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.05f, 0.43f, 0.0f);
	glVertex3f(0.04f, 0.44f, 0.0f);
	glVertex3f(0.10f, 0.47f, 0.0f);
	glVertex3f(0.10f, 0.44f, 0.0f);
	glEnd();

	//Hand Bracelet
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.749f, 0.0f);
	drawArc(0.13f, 0.45f, 0.04f, 0.04f, 64, 90.0f, 270.0f);
	glEnd();

	//Head
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.843f, 0.0f);
	glVertex3f(0.02f, 0.2f, 0.0f);
	glVertex3f(-0.1f, 0.2f, 0.0f);
	glVertex3f(-0.095f, 0.25f, 0.0f);
	glVertex3f(0.015f, 0.25f, 0.0f);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.349f, 0.211f, 0.137f);
	glVertex3f(-0.095f, 0.25f, 0.0f);
	glVertex3f(0.015f, 0.25f, 0.0f);
	glVertex3f(0.01f, 0.45f, 0.0f);
	glVertex3f(-0.09f, 0.45f, 0.0f);
	glEnd();

	//Ear
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.349f, 0.211f, 0.137f);
	drawArc(-0.093f, 0.35f, 0.02f, 0.04f, 64, 85.0f, 270.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.745f, 0.549f);
	drawArc(-0.093f, 0.35f, 0.01f, 0.03f, 64, 85.0f, 270.0f);
	glEnd();

	//Mouth
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.820f, 0.565f, 0.557f);
	drawArc(0.0135f, 0.32f, 0.09f, 0.07f, 64, 0.0f, 90.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.820f, 0.565f, 0.557f);
	drawArc(0.0135f, 0.32f, 0.09f, 0.07f, 64, 270.0f, 360.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.820f, 0.565f, 0.557f);
	drawArc(0.0135f, 0.32f, 0.04f, 0.07f, 64, 90.0f, 270.0f);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glColor3f(0.820f, 0.565f, 0.557f);
	drawPlaneArc(0.06f, 0.44f, 0.05f, 0.06f, 64, 180.0f, 270.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(1.0f, 0.306f, 0.306f);
	drawArc(0.07f, 0.30f, 0.03f, 0.001f, 64, 0.0f, 360.0f);
	glEnd();

	//Eye
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 1.0f, 1.0f);
	drawArc(-0.005f, 0.39f, 0.03f, 0.025f, 64, 0.0f, 360.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.0f, 0.0f, 0.0f);
	drawArc(0.006f, 0.39f, 0.02f, 0.015f, 64, 0.0f, 360.0f);
	glEnd();

	//Mukut

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.749f, 0.0f);
	glVertex3f(0.01f, 0.45f, 0.0f);
	glVertex3f(-0.09f, 0.45f, 0.0f);
	glVertex3f(-0.07f, 0.60f, 0.0f);
	glVertex3f(-0.01f, 0.60f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.71f, 0.086f, 0.0f);
	glVertex3f(0.02f, 0.45f, 0.0f);
	glVertex3f(-0.1f, 0.45f, 0.0f);
	glVertex3f(-0.10f, 0.50f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.761f, 0.118f, 0.337f);
	glVertex3f(0.02f, 0.51f, 0.0f);
	glVertex3f(0.02f, 0.58f, 0.0f);
	glVertex3f(-0.02f, 0.51f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.0f, 0.361f, 0.09f);
	drawArc(0.02f, 0.49f, 0.05f, 0.04f, 64, 90.0f, 270.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.976f, 0.0f);
	drawArc(-0.04f, 0.60f, 0.025f, 0.035f, 64, 0.0f, 180.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.78f, 0.0f, 0.514f);
	drawArc(-0.04f, 0.642f, 0.02f, 0.004f, 64, 0.0f, 360.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.035f, 0.31f, 0.0f);
	drawArc(-0.04f, 0.652f, 0.006f, 0.006f, 64, 0.0f, 360.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.6f, 0.0f);
	drawArc(-0.04f, 0.665f, 0.006f, 0.006f, 64, 0.0f, 360.0f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-0.035f, 0.67f, 0.0f);
	glVertex3f(-0.045f, 0.67f, 0.0f);
	glVertex3f(-0.04f, 0.74f, 0.0f);
	glEnd();

}

void drawSea(void)
{
	// code

	glBegin(GL_QUADS);

	// Top of sea (lighter blue)
	//glColor3f(0.2f, 0.7f, 1.0f);
	glColor3f(0.2f, 0.7f, 1.0f);
	glVertex2f(-1.0f, -0.3f);
	glColor3f(0.55f, 0.75f, 0.95f);
	glVertex2f(1.0f, -0.3f);

	// Bottom of sea (darker blue)
	glColor3f(0.0f, 0.2f, 0.8f);
	glVertex2f(1.0f, -1.0f);
	glColor3f(0.0f, 0.2f, 0.8f);
	glVertex2f(-1.0f, -1.0f);

    glEnd();
	glLineWidth(2.0f);
	glColor3f(1.0f, 1.0f, 1.0f);

	//Wave 1
	glBegin(GL_LINE_STRIP);
	glVertex2f(-0.90f, -0.45f);
	glVertex2f(-0.85f, -0.43f);
	glVertex2f(-0.80f, -0.42f);
	glVertex2f(-0.75f, -0.43f);
	glVertex2f(-0.70f, -0.45f);

	glEnd();

	//Wave 2
	glBegin(GL_LINE_STRIP);
	glVertex2f(-0.55f, -0.55f);
	glVertex2f(-0.50f, -0.53f);
	glVertex2f(-0.45f, -0.52f);
	glVertex2f(-0.40f, -0.53f);
	glVertex2f(-0.35f, -0.55f);

	glEnd();

	//Wave 3
	glBegin(GL_LINE_STRIP);
	glVertex2f(-0.10f, -0.42f);
	glVertex2f(-0.05f, -0.40f);
	glVertex2f(0.00f, -0.39f);
	glVertex2f(0.05f, -0.40f);
	glVertex2f(0.10f, -0.42f);

	glEnd();

	//Wave 4
	glBegin(GL_LINE_STRIP);
	glVertex2f(0.25f, -0.60f);
	glVertex2f(0.30f, -0.58f);
	glVertex2f(0.35f, -0.57f);
	glVertex2f(0.40f, -0.58f);
	glVertex2f(0.45f, -0.60f);

	glEnd();

	//Wave 5
	glBegin(GL_LINE_STRIP);
	glVertex2f(0.60f, -0.48f);
	glVertex2f(0.65f, -0.46f);
	glVertex2f(0.70f, -0.45f);
	glVertex2f(0.75f, -0.46f);
	glVertex2f(0.80f, -0.48f);

	glEnd();

	//Wave 6
	glBegin(GL_LINE_STRIP);
	glVertex2f(-0.80f, -0.75f);
	glVertex2f(-0.75f, -0.73f);
	glVertex2f(-0.70f, -0.72f);
	glVertex2f(-0.65f, -0.73f);
	glVertex2f(-0.60f, -0.75f);

	glEnd();

	//Wave 7
	glBegin(GL_LINE_STRIP);
	glVertex2f(-0.30f, -0.85f);
	glVertex2f(-0.25f, -0.83f);
	glVertex2f(-0.20f, -0.82f);
	glVertex2f(-0.15f, -0.83f);
	glVertex2f(-0.10f, -0.85f);

	glEnd();

	//Wave 8
	glBegin(GL_LINE_STRIP);
	glVertex2f(0.20f, -0.78f);
	glVertex2f(0.25f, -0.76f);
	glVertex2f(0.30f, -0.75f);
	glVertex2f(0.35f, -0.76f);
	glVertex2f(0.40f, -0.78f);

	glEnd();

	//Wave 9
	glBegin(GL_LINE_STRIP);
	glVertex2f(0.65f, -0.88f);
	glVertex2f(0.70f, -0.86f);
	glVertex2f(0.75f, -0.85f);
	glVertex2f(0.80f, -0.86f);
	glVertex2f(0.85f, -0.88f);

	glEnd();
}

void drawBoat(float x, float y, float scale) {
	//body
	glColor3f(0.45f, 0.28f, 0.12f);
	glBegin(GL_QUADS);
	glVertex2f(x - 0.032f * scale, y);
	glVertex2f(x + 0.032f * scale, y);
	glVertex2f(x + 0.045f * scale, y + 0.025f * scale);
	glVertex2f(x - 0.045f * scale, y + 0.025f * scale);
	glEnd();

	// shaft stick
	glColor3f(0.3f, 0.18f, 0.08f);
	drawQuad(x - 0.0025f * scale, y + 0.025f * scale, 0.005f * scale, 0.06f * scale);

	//shaft
	glColor3f(1.0f, 1.0f, 1.0f);
	drawTriangle(x, y + 0.085f * scale, x, y + 0.03f * scale, x + 0.04f * scale, y + 0.055f * scale);
}

void drawTemple(float baseX, float baseY, float width, float bodyHeight, int tiers) {

	// find center of your temple
	float centerX = baseX + width * 0.5f;

	// Temple body
	glColor3f(0.85f, 0.65f, 0.13f);
	drawQuad(baseX, baseY, width, bodyHeight);

	// Door
	glColor3f(0.55f, 0.38f, 0.08f);
	drawQuad(baseX + width * 0.35f, baseY, width * 0.3f, bodyHeight * 0.35f);    // starts 35% from baseX, height is 35% of body height, width is 30% of body width 

	// Left right windows
	glColor3f(0.75f, 0.55f, 0.10f);
	drawQuad(baseX + width * 0.08f, baseY + bodyHeight * 0.55f, width * 0.18f, bodyHeight * 0.3f);   // starts at 8% from baseX and 55% from baseY, width is 18% of body width, height is 30% of body heigth
	drawQuad(baseX + width * 0.74f, baseY + bodyHeight * 0.55f, width * 0.18f, bodyHeight * 0.3f);   // starts at 74% from baseX and 55% from baseY, width is 18% of body width, height is 30% of body heigth

	// kalash
	float kalashW = width;
	float kalashY = baseY + bodyHeight;
	float roofHeight = bodyHeight;
	float tierHeight = roofHeight / tiers;

	for (int i = 0; i < tiers; i++) {
		float shade = 0.85f - i * 0.07f;     // Dark the shade by 7% in each iteration of loop (0.85, 0.78, 0.71, 0.64, 0.57)

		glColor3f(shade, shade * 0.78f, 0.12f);
		drawQuad(centerX - kalashW * 0.5f, kalashY, kalashW, tierHeight);
		kalashW -= width * 0.15f;    //reduce width as u go up
		kalashY += tierHeight;       //update base as base of previous kalash level
	}

	// Top design
	glColor3f(0.95f, 0.85f, 0.2f);
	drawQuad(centerX - width * 0.02f, kalashY, width * 0.04f, roofHeight * 0.10f);

	glColor3f(0.95f, 0.8f, 0.1f);
	drawCircleWithSlices(centerX, kalashY + roofHeight * 0.16f, width * 0.07f, 14);

	glColor3f(1.0f, 0.9f, 0.3f);
	drawTriangle(centerX - width * 0.025f, kalashY + roofHeight * 0.22f, centerX + width * 0.025f, kalashY + roofHeight * 0.22f, centerX, kalashY + roofHeight * 0.32f);
}

void drawTree(float baseX, float baseY,float height, float width) {

	//stem of tree
	glColor3f(0.45f, 0.28f, 0.12f);
	drawQuad(baseX, baseY, width, height);
	
	//upper portion
	float centerX = baseX + width * 0.5f;
	float topY = baseY + height;

	glColor3f(0.1f, 0.5f, 0.0f);
	drawCircleWithSlices(centerX - width * 0.30f, topY + height*0.3f , width*2.0f ,50);
	glColor3f(0.1f, 0.5f, 0.0f);
	drawCircleWithSlices(centerX + width * 0.30f, topY + height * 0.3f, width*2.0f , 50);
	glColor3f(0.1f, 0.5f, 0.0f);
	drawCircleWithSlices(centerX, topY + height * 0.75f, width*2.0f , 50);
}

void drawRamSetu(void) {

	//1
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(-0.096f, -0.95f, 0.05f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.0f, -0.95f, 0.05f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.096f, -0.95f, 0.05f, 50);
	//2
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(-0.085f, -0.83f, 0.045f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.0f, -0.83f, 0.045f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.085f, -0.83f, 0.045f, 50);
	//3
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(-0.077f, -0.72f, 0.04f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.0f, -0.72f, 0.04f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.077f, -0.72f, 0.04f, 50);
	//4
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(-0.068f, -0.62f, 0.035f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.0f, -0.62f, 0.035f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.068f, -0.62f, 0.035f, 50);
	//5
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(-0.057f, -0.535f, 0.03f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.0f, -0.535f, 0.03f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.057f, -0.535f, 0.03f, 50);
	//6
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(-0.048f, -0.46f, 0.025f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.0f, -0.46f, 0.025f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.048f, -0.46f, 0.025f, 50);
	//7
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(-0.043f, -0.4f, 0.022f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.0f, -0.4f, 0.022f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.043f, -0.4f, 0.022f, 50);
	//8
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(-0.038f, -0.35f, 0.019f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.0f, -0.35f, 0.019f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.038f, -0.35f, 0.019f, 50);
	//8
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(-0.033f, -0.31f, 0.017f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.0f, -0.31f, 0.017f, 50);
	glColor3f(0.5f, 0.6f, 0.7f);
	drawCircleWithSlices(0.033f, -0.31f, 0.017f, 50);
}

void DrawBackground(void)
{
	glBegin(GL_QUADS);
	glColor3f(0.149f, 0.384f, 0.729f);
	glVertex3f(1.0f, 1.0f, 0.0f);
	glColor3f(0.498f, 0.510f, 0.729f);
	glVertex3f(-1.0f, 1.0f, 0.0f);
	glColor3f(0.498f, 0.106f, 0.529f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glColor3f(0.149f, 0.384f, 0.729f);
	glVertex3f(1.0f, -1.0f, 0.0f);


	glColor3f(0.141f, 0.369f, 0.702f);
	glVertex3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);

	glEnd();
}

void DrawRTR(void)
{

	glBegin(GL_QUADS);
	glColor3f(1.f, 1.0f, 1.0f);
	//R Letter
	glVertex3f(-0.3f, 0.9f, 0.0f);
	glVertex3f(-0.35f, 0.9f, 0.0f);
	glVertex3f(-0.35f, 0.4f, 0.0f);
	glVertex3f(-0.3f, 0.4f, 0.0f);

	glVertex3f(-0.3f, 0.9f, 0.0f);
	glVertex3f(-0.3f, 0.8f, 0.0f);
	glVertex3f(-0.15f, 0.8f, 0.0f);
	glVertex3f(-0.15f, 0.9f, 0.0f);

	glVertex3f(-0.15f, 0.8f, 0.0f);
	glVertex3f(-0.2f, 0.8f, 0.0f);
	glVertex3f(-0.2f, 0.6f, 0.0f);
	glVertex3f(-0.15f, 0.6f, 0.0f);

	glVertex3f(-0.2f, 0.7f, 0.0f);
	glVertex3f(-0.3f, 0.7f, 0.0f);
	glVertex3f(-0.3f, 0.6f, 0.0f);
	glVertex3f(-0.2f, 0.6f, 0.0f);

	glVertex3f(-0.2f, 0.6f, 0.0f);
	glVertex3f(-0.25f, 0.6f, 0.0f);
	glVertex3f(-0.2f, 0.4f, 0.0f);
	glVertex3f(-0.15f, 0.4f, 0.0f);

	//T Letter
	glVertex3f(-0.1f, 0.9f, 0.0f);
	glVertex3f(-0.1f, 0.8f, 0.0f);
	glVertex3f(0.1f, 0.8f, 0.0f);
	glVertex3f(0.1f, 0.9f, 0.0f);

	glVertex3f(0.025f, 0.8f, 0.0f);
	glVertex3f(-0.025f, 0.8f, 0.0f);
	glVertex3f(-0.025f, 0.4f, 0.0f);
	glVertex3f(0.025f, 0.4f, 0.0f);

	//R Letter
	glVertex3f(0.2f, 0.9f, 0.0f);
	glVertex3f(0.15f, 0.9f, 0.0f);
	glVertex3f(0.15f, 0.4f, 0.0f);
	glVertex3f(0.2f, 0.4f, 0.0f);


	glVertex3f(0.2f, 0.9f, 0.0f);
	glVertex3f(0.2f, 0.8f, 0.0f);
	glVertex3f(0.35f, 0.8f, 0.0f);
	glVertex3f(0.35f, 0.9f, 0.0f);

	glVertex3f(0.35f, 0.8f, 0.0f);
	glVertex3f(0.3f, 0.8f, 0.0f);
	glVertex3f(0.3f, 0.6f, 0.0f);
	glVertex3f(0.35f, 0.6f, 0.0f);

	glVertex3f(0.3f, 0.7f, 0.0f);
	glVertex3f(0.2f, 0.7f, 0.0f);
	glVertex3f(0.2f, 0.6f, 0.0f);
	glVertex3f(0.3f, 0.6f, 0.0f);

	glVertex3f(0.3f, 0.6f, 0.0f);
	glVertex3f(0.25f, 0.6f, 0.0f);
	glVertex3f(0.3f, 0.4f, 0.0f);
	glVertex3f(0.35f, 0.4f, 0.0f);

	//7

	glVertex3f(0.4f, 0.9f, 0.0f);
	glVertex3f(0.4f, 0.8f, 0.0f);
	glVertex3f(0.6f, 0.8f, 0.0f);
	glVertex3f(0.6f, 0.9f, 0.0f);

	glVertex3f(0.6f, 0.8f, 0.0f);
	glVertex3f(0.55f, 0.8f, 0.0f);
	glVertex3f(0.4f, 0.4f, 0.0f);
	glVertex3f(0.45f, 0.4f, 0.0f);
	glEnd();

}

void DrawVector(void)
{
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	//V Letter
	glVertex3f(-0.7f, 0.3f, 0.0f);
	glVertex3f(-0.75f, 0.3f, 0.0f);
	glVertex3f(-0.65f, -0.2f, 0.0f);
	glVertex3f(-0.65f, -0.1f, 0.0f);

	glVertex3f(-0.55f, 0.3f, 0.0f);
	glVertex3f(-0.6f, 0.3f, 0.0f);
	glVertex3f(-0.65f, -0.1f, 0.0f);
	glVertex3f(-0.65f, -0.2f, 0.0f);

	//E Letter
	glVertex3f(-0.45f, 0.3f, 0.0f);
	glVertex3f(-0.5f, 0.3f, 0.0f);
	glVertex3f(-0.5f, -0.2f, 0.0f);
	glVertex3f(-0.45f, -0.2f, 0.0f);

	glVertex3f(-0.45f, 0.3f, 0.0f);
	glVertex3f(-0.45f,0.2f, 0.0f);
	glVertex3f(-0.3f, 0.2f, 0.0f);
	glVertex3f(-0.3f, 0.3f, 0.0f);

	glVertex3f(-0.45f, 0.1f, 0.0f);
	glVertex3f(-0.45f, 0.0f, 0.0f);
	glVertex3f(-0.3f, 0.0f, 0.0f);
	glVertex3f(-0.3f, 0.1f, 0.0f);

	glVertex3f(-0.45f, -0.1f, 0.0f);
	glVertex3f(-0.45f, -0.2f, 0.0f);
	glVertex3f(-0.3f, -0.2f, 0.0f);
	glVertex3f(-0.3f, -0.1f, 0.0f);

	//C Letter
	glVertex3f(-0.2f, 0.3f, 0.0f);
	glVertex3f(-0.25f, 0.3f, 0.0f);
	glVertex3f(-0.25f, -0.2f, 0.0f);
	glVertex3f(-0.2f, -0.2f, 0.0f);

	glVertex3f(-0.2f, 0.3f, 0.0f);
	glVertex3f(-0.2f, 0.2f, 0.0f);
	glVertex3f(-0.05f, 0.2f, 0.0f);
	glVertex3f(-0.05f, 0.3f, 0.0f);

	glVertex3f(-0.2f, -0.1f, 0.0f);
	glVertex3f(-0.2f, -0.2f, 0.0f);
	glVertex3f(-0.05f, -0.2f, 0.0f);
	glVertex3f(-0.05f, -0.1f, 0.0f);

	//T Letter
	glVertex3f(0.0f, 0.3f, 0.0f);
	glVertex3f(0.0f, 0.2f, 0.0f);
	glVertex3f(0.2f, 0.2f, 0.0f);
	glVertex3f(0.2f, 0.3f, 0.0f);

	glVertex3f(0.125f, 0.2f, 0.0f);
	glVertex3f(0.075f, 0.2f, 0.0f);
	glVertex3f(0.075f, -0.2f, 0.0f);
	glVertex3f(0.125f, -0.2f, 0.0f);
	
	//O Letter

	glVertex3f(0.3f, 0.3f, 0.0f);
	glVertex3f(0.25f, 0.3f, 0.0f);
	glVertex3f(0.25f, -0.2f, 0.0f);
	glVertex3f(0.3f, -0.2f, 0.0f);

	glVertex3f(0.3f, 0.3f, 0.0f);
	glVertex3f(0.3f, 0.2f, 0.0f);
	glVertex3f(0.4f, 0.2f, 0.0f);
	glVertex3f(0.4f, 0.3f, 0.0f);

	glVertex3f(0.3f, -0.1f, 0.0f);
	glVertex3f(0.3f, -0.2f, 0.0f);
	glVertex3f(0.4f, -0.2f, 0.0f);
	glVertex3f(0.4f, -0.1f, 0.0f);

	glVertex3f(0.45f, 0.3f, 0.0f);
	glVertex3f(0.4f, 0.3f, 0.0f);
	glVertex3f(0.4f, -0.2f, 0.0f);
	glVertex3f(0.45f, -0.2f, 0.0f);

	//R Letter
	glVertex3f(0.55f, 0.3f, 0.0f);
	glVertex3f(0.5f, 0.3f, 0.0f);
	glVertex3f(0.5f, -0.2f, 0.0f);
	glVertex3f(0.55f, -0.2f, 0.0f);

	glVertex3f(0.55f, 0.3f, 0.0f);
	glVertex3f(0.55f, 0.2f, 0.0f);
	glVertex3f(0.7f, 0.2f, 0.0f);
	glVertex3f(0.7f, 0.3f, 0.0f);

	glVertex3f(0.7f, 0.2f, 0.0f);
	glVertex3f(0.65f, 0.2f, 0.0f);
	glVertex3f(0.65f, 0.1f, 0.0f);
	glVertex3f(0.7f, 0.1f, 0.0f);

	glVertex3f(0.55f, 0.1f, 0.0f);
	glVertex3f(0.55f, 0.0f, 0.0f);
	glVertex3f(0.7f, 0.0f, 0.0f);
	glVertex3f(0.7f, 0.1f, 0.0f);

	glVertex3f(0.65f, 0.0f, 0.0f);
	glVertex3f(0.6f, 0.0f, 0.0f);
	glVertex3f(0.65f, -0.2f, 0.0f);
	glVertex3f(0.7f, -0.2f, 0.0f);

	glEnd();
}

void DrawGroup(void)
{
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	//G Letter
	glVertex3f(-0.6f, -0.3f, 0.0f);
	glVertex3f(-0.65f, -0.3f, 0.0f);
	glVertex3f(-0.65f, -0.8f, 0.0f);
	glVertex3f(-0.6f, -0.8f, 0.0f);

	glVertex3f(-0.6f, -0.3f, 0.0f);
	glVertex3f(-0.6f, -0.4f, 0.0f);
	glVertex3f(-0.45f, -0.4f, 0.0f);
	glVertex3f(-0.45f, -0.3f, 0.0f);

	glVertex3f(-0.6f, -0.7f, 0.0f);
	glVertex3f(-0.6f, -0.8f, 0.0f);
	glVertex3f(-0.45f, -0.8f, 0.0f);
	glVertex3f(-0.45f, -0.7f, 0.0f);

	glVertex3f(-0.45f, -0.6f, 0.0f);
	glVertex3f(-0.5f, -0.6f, 0.0f);
	glVertex3f(-0.5f, -0.7f, 0.0f);
	glVertex3f(-0.45f, -0.7f, 0.0f);

	glVertex3f(-0.55f, -0.6f, 0.0f);
	glVertex3f(-0.55f, -0.65f, 0.0f);
	glVertex3f(-0.5f, -0.65f, 0.0f);
	glVertex3f(-0.5f, -0.6f, 0.0f);

	//R Letter
	glVertex3f(-0.35f, -0.3f, 0.0f);
	glVertex3f(-0.4f, -0.3f, 0.0f);
	glVertex3f(-0.4f, -0.8f, 0.0f);
	glVertex3f(-0.35f, -0.8f, 0.0f);

	glVertex3f(-0.35f, -0.3f, 0.0f);
	glVertex3f(-0.35f, -0.4f, 0.0f);
	glVertex3f(-0.2f, -0.4f, 0.0f);
	glVertex3f(-0.2f, -0.3f, 0.0f);

	glVertex3f(-0.2f, -0.4f, 0.0f);
	glVertex3f(-0.25f, -0.4f, 0.0f);
	glVertex3f(-0.25f, -0.6f, 0.0f);
	glVertex3f(-0.2f, -0.6f, 0.0f);

	glVertex3f(-0.25f, -0.5f, 0.0f);
	glVertex3f(-0.35f, -0.5f, 0.0f);
	glVertex3f(-0.35f, -0.6f, 0.0f);
	glVertex3f(-0.25f, -0.6f, 0.0f);

	glVertex3f(-0.25f, -0.6f, 0.0f);
	glVertex3f(-0.3f, -0.6f, 0.0f);
	glVertex3f(-0.25f, -0.8f, 0.0f);
	glVertex3f(-0.2f, -0.8f, 0.0f);
	//O Letter
	glVertex3f(-0.1f, -0.3f, 0.0f);
	glVertex3f(-0.15f, -0.3f, 0.0f);
	glVertex3f(-0.15f, -0.8f, 0.0f);
	glVertex3f(-0.1f, -0.8f, 0.0f);

	glVertex3f(0.0f, -0.3f, 0.0f);
	glVertex3f(-0.1f, -0.3f, 0.0f);
	glVertex3f(-0.1f, -0.4f, 0.0f);
	glVertex3f(0.0f, -0.4f, 0.0f);

	glVertex3f(0.0f, -0.7f, 0.0f);
	glVertex3f(-0.1f, -0.7f, 0.0f);
	glVertex3f(-0.1f, -0.8f, 0.0f);
	glVertex3f(0.0f, -0.8f, 0.0f);

	glVertex3f(0.05f, -0.3f, 0.0f);
	glVertex3f(0.0f, -0.3f, 0.0f);
	glVertex3f(0.0f, -0.8f, 0.0f);
	glVertex3f(0.05f, -0.8f, 0.0f);
	//U Letter
	glVertex3f(0.15f, -0.3f, 0.0f);
	glVertex3f(0.1f, -0.3f, 0.0f);
	glVertex3f(0.1f, -0.8f, 0.0f);
	glVertex3f(0.15f, -0.8f, 0.0f);

	glVertex3f(0.15f, -0.7f, 0.0f);
	glVertex3f(0.15f, -0.8f, 0.0f);
	glVertex3f(0.25f, -0.8f, 0.0f);
	glVertex3f(0.25f, -0.7f, 0.0f);

	glVertex3f(0.3f, -0.3f, 0.0f);
	glVertex3f(0.25f, -0.3f, 0.0f);
	glVertex3f(0.25f, -0.8f, 0.0f);
	glVertex3f(0.3f, -0.8f, 0.0f);
	//P Letter
	glVertex3f(0.4f, -0.3f, 0.0f);
	glVertex3f(0.35f, -0.3f, 0.0f);
	glVertex3f(0.35f, -0.8f, 0.0f);
	glVertex3f(0.4f, -0.8f, 0.0f);

	glVertex3f(0.4f, -0.3f, 0.0f);
	glVertex3f(0.4f, -0.4f, 0.0f);
	glVertex3f(0.5f, -0.4f, 0.0f);
	glVertex3f(0.5f, -0.3f, 0.0f);

	glVertex3f(0.5f, -0.5f, 0.0f);
	glVertex3f(0.4f, -0.5f, 0.0f);
	glVertex3f(0.4f, -0.6f, 0.0f);
	glVertex3f(0.5f, -0.6f, 0.0f);

	glVertex3f(0.55f, -0.3f, 0.0f);
	glVertex3f(0.5f, -0.3f, 0.0f);
	glVertex3f(0.5f, -0.6f, 0.0f);
	glVertex3f(0.55f, -0.6f, 0.0f);

	glEnd();
}

void drawRocks(void){
    //glColor3f(0.161f, 0.102f, 0.216f);
	glColor3f(0.102f, 0.051f, 0.004f);
	drawOval(0.9f, -0.25f, 0.2f, 0.05f, 30);
	//glColor3f(0.161f, 0.102f, 0.216f);
	glColor3f(0.102f, 0.051f, 0.004f);
	drawOval(0.7f, -0.25f, 0.2f, 0.05f, 30);
	//glColor3f(0.161f, 0.102f, 0.216f);
	glColor3f(0.102f, 0.051f, 0.004f);
	drawOval(0.5f, -0.25f, 0.2f, 0.05f, 30);

	//glColor3f(0.161f, 0.102f, 0.216f);
	glColor3f(0.102f, 0.051f, 0.004f);
	drawOval(-0.9f, -0.25f, 0.2f, 0.05f, 30);
	glColor3f(0.102f, 0.051f, 0.004f);
	//glColor3f(0.161f, 0.106f, 0.051f);
	drawOval(-0.7f, -0.25f, 0.2f, 0.05f, 30);
	glColor3f(0.102f, 0.051f, 0.004f);
	//glColor3f(0.161f, 0.106f, 0.051f);
	drawOval(-0.5f, -0.25f, 0.2f, 0.05f, 30);
}

void drawClouds(void)
{
    // function declarations 
    void drawCircle(float, float, float); 

    //cloud4
    drawCircle(0.70,0.95,0.1);
    drawCircle(0.85,0.96,0.1);
    drawCircle(0.70,0.85,0.1);
    drawCircle(0.80,0.83,0.1);

    //cloud3
    drawCircle(0.2,0.5,0.1);
    drawCircle(0.35,0.51,0.1);
    drawCircle(0.30,0.40,0.1);

    //cloud2
    drawCircle(-0.2,0.5,0.1);
    drawCircle(-0.35,0.51,0.1);
    drawCircle(-0.30,0.40,0.1);
    drawCircle(-0.40,0.48,0.1);

    //cloud1
    drawCircle(-0.7,0.75,0.1);
    drawCircle(-0.85,0.76,0.1);
    drawCircle(-0.70,0.65,0.1);   
}

void drawArc(float cx, float cy, float radiusx, float radiusy, int segments, float startAngle, float stopAngle)
{
	// code
	glVertex3f(cx, cy, 0.0f);
	float angle = stopAngle - startAngle;
	while (startAngle <= stopAngle)
	{
		glVertex3f(cx + radiusx * cos(startAngle * DEG_TO_RAD), cy + radiusy * sin(startAngle * DEG_TO_RAD), 0.0f);
		startAngle += angle / (float)segments;
	}
}

void drawPlaneArc(float cx, float cy, float radiusx, float radiusy, int segments, float startAngle, float stopAngle)
{
	// code
	//glVertex3f(cx, cy, 0.0f);
	float angle = stopAngle - startAngle;
	while (startAngle < stopAngle)
	{
		glVertex3f(cx + radiusx * cos(startAngle * DEG_TO_RAD), cy + radiusy * sin(startAngle * DEG_TO_RAD), 0.0f);
		startAngle += angle / (float)segments;
	}
}

void drawArcRotate(float cx, float cy, float radiusx, float radiusy, int segments, float startAngle, float stopAngle, float tiltFactorY)
{
	// code
	glVertex3f(cx, cy, 0.0f);
	float angle = stopAngle - startAngle;
	while (startAngle <= stopAngle)
	{
		glVertex3f((cx + radiusx * cos(startAngle * DEG_TO_RAD)), (cy + (radiusy * sin(startAngle * DEG_TO_RAD)) * tiltFactorY), 0.0f);
		startAngle += angle / (float)segments;
	}
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3) {
	glBegin(GL_TRIANGLES);
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
	glEnd();
}

void drawQuad(float x, float y, float w, float h) {
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glEnd();
}

void drawCircleWithSlices(float cx, float cy, float r, int slices) {
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy); //drawn center
	for (int i = 0; i <= slices; i++) {
		float theta = (360.0f / slices) * i;
		float thetaRad = theta * (PI / 180.0f);
		float x = cx + r * cosf(thetaRad);
		float y = cy + (r + (r * 0.4)) * sinf(thetaRad);
		glVertex2f(x, y); //draw points from circumference
	}
	glEnd();
}

void drawCircle(float cx,float cy,float r)
{
   glBegin(GL_POLYGON);
    for(float angle = 0.0f; angle<=360.0f; angle++)
    {
        float angle_in_radian = angle*2*3.14/180.0;
        float x = cx+r*cos(angle);
        float y = cy+r*sin(angle);
        glVertex3f(x , y, 0.0f);
    }
    glEnd();
}

void drawWave(float horizontalShift, float verticalShift) {

	glBegin(GL_LINE_STRIP);

	for (float x = -1.0f; x <= 1.0f; x += 0.01f)
	{
		float y = (0.03f * sin(2.0f * PI * x - horizontalShift)) + verticalShift;

		glVertex2f(x, y);
	}

	glEnd();
}

void drawEllipse(float cx, float cy, float rx, float ry, float angle_start, float angle_end) {

	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy); //center
	for (float angle = angle_start; angle <= angle_end; angle += 0.2f)
	{
		float angle_rad = angle * PI / 180.0f;
		float x = cx + rx * cos(angle_rad);
		float y = cy + ry * sin(angle_rad);
		glVertex2f(x, y);	 //circumference points
	}
	glEnd();
}

void drawOval(float centerX, float centerY, float radiusX, float radiusY, int segments) {
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(centerX, centerY);
	for (int i = 0; i <= segments; i++) {
		float angle = i * ( PI / segments);
		float x = centerX + (radiusX * cos(angle));
		float y = centerY + (radiusY * sin(angle));

		glVertex2f(x, y);
	}
	glEnd();
}

void drawPoint(float x, float y, float size, float r, float g, float b)
{
	glPointSize(size);
	glBegin(GL_POINTS);
		glColor3f(r, g, b);
		glVertex2f(x, y);
	glEnd();
}

void drawLine(float x1, float y1, float x2, float y2, float thickness, float r, float g, float b)
{
	glColor3f(r, g, b);
	glLineWidth(thickness);
	glBegin(GL_LINES);
		glVertex2f(x1, y1);
		glVertex2f(x2, y2);
	glEnd();
}

void drawOval(float x1, float y1, float xR, float yR)
{
	int segments = 128;
	float theta;
	float x, y;

	glBegin(GL_LINE_LOOP);
	for (int i =0; i<=segments; i++)
	{
		theta = (2.0f * PI * i) / segments;

		x = xR * cosf(theta);
		y = yR * sinf(theta);

		glVertex2f(x+x1 ,y+y1);
	}
	glEnd();

}

void drawSolidOval(float x1, float y1, float xR, float yR, float cr, float cg, float cb, float r, float g, float b)
{
	int segments = 128;
	float theta;
	float x, y;

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(cr, cg, cb);
	glVertex2f(x1, y1);
	for (int i = 0; i <= segments; i++)
	{
		theta = (2.0f * PI * i) / segments;

		x = xR * cosf(theta);
		y = yR * sinf(theta);

		glColor3f(r,g,b);
		glVertex2f(x + x1, y + y1);
	}
	glEnd();
}

void drawSolidTriangle(float x1, float y1,
						float x2, float y2,
						float x3, float y3,
						float v1R, float v1G, float v1B,
						float v2R, float v2G, float v2B,
						float v3R, float v3G, float v3B
						)
{
	glBegin(GL_TRIANGLE_FAN);
		glColor3f( v1R,  v1G,  v1B); glVertex2f(x1, y1);
		glColor3f( v2R,  v2G,  v2B); glVertex2f(x2, y2);
		glColor3f( v3R,  v3G,  v3B); glVertex2f(x3, y3);
	glEnd();
}


void drawSolidSemiCircle(float x1, float y1, float xR, float yR, float cr, float cg, float cb, float r, float g, float b)
{
	int segments = 128;
	float theta;
	float x, y;

	glBegin(GL_TRIANGLE_FAN);
	glColor3f(cr, cg, cb);
	glVertex2f(x1, y1);
	for (int i = 0; i <= segments; i++)
	{
		theta = (PI * i) / segments;

		x = xR * cosf(theta);
		y = yR * sinf(theta);

		glColor3f(r, g, b);
		glVertex2f(x + x1, y + y1);
	}
	glEnd();
}

void drawBezierCurve(float x1, float y1,float xc1, float yc1, float xc2, float yc2, float x2, float y2, float thickness, float r, float g, float b)
{
	int i;
	float t, tt, ttt;
	float coefficient, coffSqr, coffCb;
	float x, y;
	int lineSegments = 128;

	glLineWidth(thickness);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i <= lineSegments; i++)
	{
		t = (float)i / float(lineSegments);
		tt = t * t;
		ttt = tt * t;

		coefficient = 1.0f - t;
		coffSqr = coefficient * coefficient;
		coffCb = coffSqr * coefficient;

		x = coffCb * x1 + 3 * coffSqr * t * xc1 + 3 * coefficient * tt * xc2 + ttt * x2;
		y = coffCb * y1 + 3 * coffSqr * t * yc1 + 3 * coefficient * tt * yc2 + ttt * y2;

		glColor3f(r,g,b);
		glVertex2f(x, y);

	}
	glEnd();
}

void getBazierCurvePoints(float x1, float y1,float xc1, float yc1, float xc2, float yc2, float x2, float y2, float r, float g, float b)
{
	int i;
	float t, tt, ttt;
	float coefficient, coffSqr, coffCb;
	float x, y;
	int lineSegments = 128;

	for (i = 0; i <= lineSegments; i++)
	{
		t = (float)i / float(lineSegments);
		tt = t * t;
		ttt = tt * t;

		coefficient = 1.0f - t;
		coffSqr = coefficient * coefficient;
		coffCb = coffSqr * coefficient;

		x = coffCb * x1 + 3 * coffSqr * t * xc1 + 3 * coefficient * tt * xc2 + ttt * x2;
		y = coffCb * y1 + 3 * coffSqr * t * yc1 + 3 * coefficient * tt * yc2 + ttt * y2;

		glColor3f(r,g,b);
		glVertex2f(x, y);

	}
}

void drawFace()
{
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(37.00f, 0.0f, 0.0f, 1.0f);
	drawSolidOval(0.34f, 0.13f, 0.07f, 0.08f, 0.945f, 0.761f, 0.490f, 0.945f, 0.761f, 0.490f);		// face

	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(71.00, 0.0f, 0.0f, 1.0f);
	drawSolidOval(0.18, -0.30, 0.04f, 0.06f, 1.000f, 0.765f, 0.667f, 1.000f, 0.765f, 0.667f);		//mouth
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(31.00f, 0.0f, 0.0f, 1.0f);
	drawSolidSemiCircle(0.28f, 0.22f, 0.10f, 0.11f, 0.878f, 0.667f, 0.243f, 0.878f, 0.667f, 0.243f);	// mukut
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-1.00f, 0.0f, 0.0f, 1.0f);
	drawSolidTriangle(0.17f, 0.38f,								// mukut triangle
		0.21f, 0.38f,
		0.19f, 0.44f,
		0.878f, 0.667f, 0.243f,
		0.878f, 0.667f, 0.243f,
		0.878f, 0.667f, 0.243f);
	glPopMatrix();

	///--------------------------------------------------------

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-50.00f, 0.0f, 0.0f, 1.0f);
	drawLine(-0.15, 0.41, -0.13, 0.41, 4.0f, 1.0f, 0.0f, 0.0f);				// Tilak
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(34.00f, 0.0f, 0.0f, 1.0f);
	drawSolidSemiCircle(0.31, 0.31, 0.02f, 0.05f, 0.878f, 0.667f, 0.243f, 0.878f, 0.667f, 0.243f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(32.00f, 0.0f, 0.0f, 1.0f);
	drawSolidSemiCircle(0.29f, 0.31f, 0.05f, 0.04f, 0.878f, 0.667f, 0.243f, 0.878f, 0.667f, 0.243f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(31.00f, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(0.33f, 0.19f,
		0.35f, 0.20f,
		0.35, 0.20,
		0.37f, 0.19f,
		3.0f,
		0.0f, 0.0f, 0.0f);//	eyebrow
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(12.00f, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(
						0.26f, 0.28f,
						0.27f, 0.28f,
						0.27f, 0.28f,
						0.28f, 0.28f,
						2.0f,
						0.0f, 0.0f, 0.0f);//	eyelid
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(32.00f, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(
						0.33f, 0.09f,
						0.36f, 0.08f,
						0.36f, 0.08f,
						0.39f, 0.09f,
						3.0f,
						0.0f, 0.0f, 0.0f);//	lips
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(90.00f, 0.0f, 0.0f, 1.0f);
	drawLine(0.24, -0.23, 0.26, -0.23, 1.0f, 0.0f, 0.0f, 0.0f);
	glPopMatrix();

	// hairs
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(44.00f, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(
		0.21, 0.15,
		0.28, 0.09,
		0.28, 0.17,
		0.35, 0.15,
		4.0f,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(37.00f, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(
		0.12, 0.17,
		0.20, 0.07,
		0.20, 0.23,
		0.28, 0.17,
		6.0f,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-29.00f, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(
		-0.04, 0.29,
		0.01, 0.31,
		0.05, 0.25,
		0.10, 0.29,
		4.0f,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-37.00f, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(
		-0.08, 0.26,
		-0.04, 0.29,
		0.01, 0.25,
		0.06, 0.26,
		4.0f,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	// code for hair ends here
	//-------------------------------------------------------

}

void drawGada()
{
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-22.0, 0.0f, 0.0f, 1.0f);
	drawLine(
				-0.26, 0.15,
				0.32, 0.15,
				6.00,
				0.878f, 0.667f, 0.243f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(0.0, 0.0f, 0.0f, 1.0f);
	drawSolidOval(-0.23, 0.27, 0.11, 0.11, 0.878f, 0.667f, 0.243f, 0.878f, 0.667f, 0.243f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-57.0, 0.0f, 0.0f, 1.0f);
	drawSolidTriangle(
		-0.49, -0.11,
		-0.37, -0.11,
		-0.43, -0.03,
		0.878f, 0.667f, 0.243f,
		0.878f, 0.667f, 0.243f,
		0.878f, 0.667f, 0.243f
	);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(53.0, 0.0f, 0.0f, 1.0f);
	// gada top curv
	drawBezierCurve(
		-0.01, 0.41,
		0.05, 0.43,
		0.12, 0.43,
		0.17, 0.41,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(59.0, 0.0f, 0.0f, 1.0f);
	// gada bottom curv
	drawBezierCurve(
		0.03, 0.26,
		0.13, 0.25,
		0.10, 0.25,
		0.19, 0.26,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	// VERTICAL LINES

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-30.0, 0.0f, 0.0f, 1.0f);
	// MIDDLE
	drawBezierCurve(
		-0.41, 0.14,
		-0.37, 0.13,
		-0.30, 0.13,
		-0.25, 0.14,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();
	

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-30.0, 0.0f, 0.0f, 1.0f);
	// lower
	drawBezierCurve(
		-0.41, 0.05,
		- 0.37, 0.03,
		- 0.31, 0.03,
		- 0.25, 0.05,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-30.0, 0.0f, 0.0f, 1.0f);
	
	drawBezierCurve(
		-0.41, 0.10, 
		-0.37, 0.08, 
		-0.30, 0.08, 
		-0.25, 0.10,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-29.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(
		 -0.41, 0.18,
		 -0.37, 0.19, 
		 -0.30, 0.19, 
		 -0.25, 0.18,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(0.0, 0.0f, 0.0f, 1.0f);
	drawSolidOval(0.35, 0.02, 0.02, 0.02, 0.878f, 0.667f, 0.243f, 0.878f, 0.667f, 0.243f);
	glPopMatrix();
	
}

void drawUpperBody()
{
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(15.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(		// back curv
		-0.27, 0.15, 
		-0.18, 0.13, 
		-0.01, 0.16, 
		0.01, 0.15,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(0.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(		// stomach curv
		-0.29, -0.08,
		-0.17, -0.10,
		-0.09, -0.10,
		0.03, -0.08,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-90.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(					// waist curv
		-0.08, -0.29,
		-0.03, -0.33,
		0.03, -0.31,
		0.08, -0.29,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();



	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-43.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(					//right hand lower curv
		-0.04, 0.00, 0.01, -0.02, 0.07, -0.02, 0.12, 0.00,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-44.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(					//right hand upper curv
		-0.02, 0.08, 0.03, 0.09, 0.06, 0.09, 0.10, 0.08,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();



	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(18.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(					//right forearm lower curv
		0.06, -0.10, 0.10, -0.13, 0.20, -0.13, 0.26, -0.10,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(18.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(					//right forearm upper curv
		0.09, -0.05, 0.15, -0.03, 0.23, -0.05, 0.21, -0.05,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(63.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(					//righ palm inner curv
		0.11, -0.19, 0.14, -0.18, 0.16, -0.17, 0.19, -0.19,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(70.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(					//righ palm outer curv
		0.07, -0.25, 0.06, -0.28, 0.12, -0.27, 0.17, -0.25,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();
	

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-15.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(					//righ fist curv
		0.21, 0.14, 0.23, 0.15, 0.26, 0.15, 0.27, 0.14,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();



	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(47.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(					//righ 1st figer line curv
		0.20, -0.14, 0.22, -0.14, 0.22, -0.13, 0.24, -0.14,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(47.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(					//right 2nd figer line curv
		0.21, -0.15, 0.23, -0.15, 0.23, -0.15, 0.25, -0.15,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(45.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(					//right 3rd figer line curv
		0.22, -0.16, 0.24, -0.16, 0.24, -0.14, 0.26, -0.16,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(35.0, 0.0f, 0.0f, 1.0f);
	drawSolidOval(0.19, -0.14, 0.01, 0.06, 0.878f, 0.667f, 0.243f, 0.878f, 0.667f, 0.243f);  // right hand kada
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-52.0, 0.0f, 0.0f, 1.0f);
	drawSolidOval(0.03, 0.04, 0.01, 0.06, 0.878f, 0.667f, 0.243f, 0.878f, 0.667f, 0.243f);  // right hand biceps kada
	glPopMatrix();


	//----- left hand --------------

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-9.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// left hand upper curve
		0.12, 0.21, 0.25, 0.24, 0.34, 0.19, 0.38, 0.21,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-11.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// left hand lower curve
		0.22, 0.14, 0.27, 0.12, 0.34, 0.13, 0.36, 0.14,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-2.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// left hand forearm upper curve
		0.38, 0.16, 0.49, 0.19, 0.55, 0.16, 0.72, 0.16,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(0.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// left hand forearm upper curve
		0.38, 0.07, 0.48, 0.05, 0.61, 0.07, 0.72, 0.07,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(20.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// left hand fist
		0.71, -0.12, 0.76, -0.12, 0.76, -0.12, 0.81, -0.12,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-11.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// left hand fist lower
		0.66, 0.21, 0.70, 0.21, 0.70, 0.21, 0.74, 0.21,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-37.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// left hand fist lower
		0.53, 0.61, 0.56, 0.61, 0.56, 0.61, 0.59, 0.61,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(42.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// left hand fist lower
		0.60, -0.47, 0.66, -0.47, 0.66, -0.47, 0.72, -0.47,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-14.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// left hand fist lower
		0.69, 0.27, 0.71, 0.27, 0.71, 0.27, 0.73, 0.27,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(22.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// left hand fist lower
		0.75, -0.17, 0.77, -0.17, 0.77, -0.17, 0.79, -0.17,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(37.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// left hand fist lower
		0.66, -0.37, 0.69, -0.39, 0.69, -0.39, 0.72, -0.37,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();



	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(0.0, 0.0f, 0.0f, 1.0f);
	drawSolidOval(0.70, 0.10, 0.01, 0.06, 0.878f, 0.667f, 0.243f, 0.878f, 0.667f, 0.243f);		// left hand kada
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-18.0, 0.0f, 0.0f, 1.0f);
	drawSolidOval(0.28, 0.21, 0.01, 0.06, 0.878f, 0.667f, 0.243f, 0.878f, 0.667f, 0.243f);		// left hand bicep kada
	glPopMatrix();

	//--------------------------------

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-10.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										//red cloth on back
		-0.25, 0.04, -0.17, -0.13, 0.07, 0.21, 0.17, 0.04,
		8.0,
		1.0f, 0.5f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(19.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										//red cloth on back
		-0.24, -0.14, -0.01, -0.10, -0.14, -0.20, 0.08, -0.14,
		8.0,
		1.0f, 0.5f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(18.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										//red cloth on back
		-0.23, -0.12, 0.01, -0.07, -0.13, -0.21, 0.09, -0.12,
		8.0,
		1.0f, 0.5f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(41.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										//red cloth on back
		-0.01, -0.14, 0.03, -0.21, 0.03, -0.15, 0.07, -0.14,
		6.0,
		1.0f, 0.5f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(10.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										//red cloth on back
		0.13, 0.12, 0.20, 0.08, 0.27, 0.14, 0.35, 0.12,
		4.0,
		1.0f, 0.5f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(39.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										//red cloth on back
		-0.02, -0.19, 0.12, -0.24, 0.29, -0.38, 0.34, -0.19	,
		11.0,
		1.0f, 0.5f, 0.0f);
	glPopMatrix();


}

void drawLowerBody()
{
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(7.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										//leg upper curv
		-0.60, 0.11, -0.50, 0.10, -0.37, 0.19, -0.28, 0.11,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(6.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										//leg calf upper curv
		-0.97, 0.10, -0.74, 0.09, -0.85, 0.10, -0.55, 0.10,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(0.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										//leg lower curv
		-0.63, -0.08, -0.53, -0.09, -0.46, -0.10, -0.29, -0.08,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(0.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										//leg lower shin
		-0.93, -0.08, -0.81, -0.08, -0.74, -0.11, -0.61, -0.08,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-90.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										//leg sole
		-0.00, -0.97, 0.04, -1.02, 0.11, -0.96, 0.16, -0.97,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-90.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										//leg sole
		0.08, -0.92, 0.13, -0.91, 0.16, -0.92, 0.18, -0.92,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-15.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										//leg nails
		-0.90, -0.40, -0.89, -0.42, -0.85, -0.40, -0.84, -0.40,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(0.0, 0.0f, 0.0f, 1.0f);
	drawSolidOval(-0.89, -0.04, 0.01, 0.07, 0.878f, 0.667f, 0.243f, 0.878f, 0.667f, 0.243f);		// leg kada
	glPopMatrix();


	// ------------- left leg ----------------------------

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-61.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// left leg outer curve
		-0.60, -0.60, -0.49, -0.60, -0.45, -0.58, -0.36, -0.60,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-65.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// left leg inner curve
		-0.54, -0.71, -0.44, -0.71, -0.41, -0.73, -0.34, -0.71,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-4.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// left leg SOLE curve
		-0.97, 0.18, -0.95, 0.17, -0.82, 0.23, -0.83, 0.18,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-2.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// left leg SOLE curve
		-0.97, 0.16, -0.90, 0.15, -0.96, 0.16, -0.85, 0.16,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-90.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// left leg finger curve
		-0.25, -0.95, -0.24, -0.96, -0.20, -0.96, -0.19, -0.95,
		1.0,
		0.0f, 0.0f, 0.0f);
	glPopMatrix();



	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-48.0, 0.0f, 0.0f, 1.0f);
	drawSolidOval(-0.69, -0.49, 0.01, 0.07, 0.878f, 0.667f, 0.243f, 0.878f, 0.667f, 0.243f);		// left leg kada
	glPopMatrix();
	

	// ------------------------------------------------

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-90.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// cloth on thigh golden border
		-0.07, -0.49, -0.02, -0.52, 0.04, -0.53, 0.09, -0.49,
		20.0,
		0.878f, 0.667f, 0.243f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(22.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// cloth
		-0.54, 0.03, -0.45, 0.06, -0.38, 0.00, -0.30, 0.03,
		20.0,
		1.0f, 0.5f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(34.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// cloth
		-0.52, 0.09, -0.45, 0.19, -0.38, 0.02, -0.32, 0.09,
		20.0,
		1.0f, 0.5f, 0.0f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(39.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// cloth
		-0.50, 0.12, -0.44, 0.21, -0.37, 0.02, -0.30, 0.12,
		22.0,
		1.0f, 0.5f, 0.0f);
	glPopMatrix();


	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-46.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// cloth diagonal golder border
		-0.37, -0.26, -0.26, -0.29, -0.26, -0.29, -0.15, -0.26,
		10.0,
		0.878f, 0.667f, 0.243f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(0.0, 0.0f, 0.0f, 1.0f);
	drawSolidOval(-0.30, -0.08, 0.02, 0.02, 0.878f, 0.667f, 0.243f, 0.878f, 0.667f, 0.243f);		// draw knot
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glRotatef(-90.0, 0.0f, 0.0f, 1.0f);
	drawBezierCurve(										// tail
		-0.60, -0.45, -0.42, -0.74, -0.25, -0.29, -0.08, -0.45,
		8.0,
		0.675f, 0.451f, 0.224f);
	glPopMatrix();
}

void drawFlyingHanuman()
{
    glPushMatrix(); 

	drawFace();
	drawGada();
	drawUpperBody();
	drawLowerBody();

	glPopMatrix();
}

void drawLanka(void) 
{
	// code
    // Sky
	glBegin(GL_QUADS);
	glColor3f(0.55f, 0.75f, 0.95f); //bottom
	glVertex2f(-1.0f, -0.1f);
	glVertex2f(1.0f, -0.1f);
	glColor3f(0.15f, 0.35f, 0.75f); // top
	glVertex2f(1.0f, 1.0f);
	glVertex2f(-1.0f, 1.0f);
	glEnd();

	// Sun
	glColor3f(1.0f, 0.9f, 0.4f);
	drawCircleWithSlices(0.7f, 0.75f, 0.07f, 40);

	// Sea
	glColor3f(0.05f, 0.4f, 0.7f);
	drawQuad(-1.0f, -1.0f, 2.0f, 0.9f);

	//Waves
	glColor3f(0.0f, 0.35f, 0.65f); //1st from bottom
	drawWave(0.0f, -0.9f);
	glColor3f(0.0f, 0.35f, 0.65f); //2nd from bottom
	drawWave(0.5f, -0.85f);
	glColor3f(0.0f, 0.35f, 0.65f); //3rd from bottom
	drawWave(0.0f, -0.8f);
	glColor3f(0.08f, 0.5f, 0.78f); //4rth from bottom
	drawWave(0.5f, -0.75f);
	glColor3f(0.08f, 0.5f, 0.78f); //5th from bottom
	drawWave(0.0f, -0.7f);
	glColor3f(0.08f, 0.5f, 0.78f); //6th from bottom
	drawWave(0.5f, -0.65f);

	// Beach
	glColor3f(0.9f, 0.8f, 0.55f);
	drawEllipse(0.0f, -0.1f, 0.95f, 0.2f, 180.0f, 360.0f);

	// Green mountains
	glColor3f(0.45f, 0.65f, 0.25f);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(0.0f, -0.15f);
	glVertex2f(-0.95f, -0.1f);
	glVertex2f(-0.6f, 0.05f);
	glVertex2f(-0.2f, -0.02f);
	glVertex2f(0.2f, -0.02f);
	glVertex2f(0.6f, 0.05f);
	glVertex2f(0.95f, -0.1f);
	glEnd();

	// Boat
	// drawBoat(-0.7f, -0.4f, 2.0f);
	// drawBoat(0.5f, -0.27f, 1.0f);
	// drawBoat(0.85f, -0.3f, 1.5f);

	//Base for temples
	glColor3f(0.7f, 0.6f, 0.4f);
	drawQuad(-0.7f, -0.15f, 1.4f, 0.05f);

	//Gold Temples
	drawTemple(-0.12f, -0.10f, 0.24f, 0.34f, 5);    // center big temple
	drawTemple(-0.31f, -0.10f, 0.16f, 0.24f, 4);    // left side temple
	drawTemple(0.15f, -0.10f, 0.16f, 0.24f, 4);     // right side temple
	drawTemple(-0.51f, -0.10f, 0.11f, 0.16f, 3);    // left smallest temple
	drawTemple(0.4f, -0.10f, 0.11f, 0.16f, 3);      // right smallest temple

	//Trees
	drawTree(-0.8f, -0.1f, 0.06f, 0.01f);      // saglyat left side ch jhad
	drawTree(-0.7f, -0.05f, 0.04f, 0.0064f);   // varchya tree chya right cha tree
	drawTree(-0.55f, -0.07f, 0.05f, 0.008f);   // varchya tree chya right cha tree
	drawTree(-0.35f, -0.1f, 0.06f, 0.01f);     // tree between left temples
	drawTree(0.35f, -0.05f, 0.04f, 0.0064f);   // tree between right temples
	drawTree(0.6f, -0.1f, 0.06f, 0.01f);       // tree right side of temple
	drawTree(0.75f, -0.07f, 0.05f, 0.008f);   // rightest tree

	// drawRamSetu();
}

void update(void) 
{
    // code 
    if(scene_number == 1) 
    {
		static float timer = 800; 
		if(timer > 0) 
			timer -= 0.2; 
		else 
			scene_number += 1; 

		birdTx -= 0.00006; 
		birdTy += 0.00006; 
    } 
    else if(scene_number == 2) 
    {
		if(bDrawRTR == true) 
		{
			if(rtrScale < 1.0f) 
				rtrScale += 0.001; 
			else
				bDrawVector = true; 
		} 
		if(bDrawVector == true) 
		{
			if(vectorScale < 1.0f)
				vectorScale += 0.001; 
			else  
				bDrawGroup = true; 
		} 
		if(bDrawGroup == true) 
		{
			if(groupScale < 1.0f) 
				groupScale += 0.001; 
			else 
			{
				static float waitTimer = 100; 
				if(waitTimer > 0.0) 
					waitTimer -= 0.05; 
				else 
					scene_number += 1;
			} 
		} 
    }
    else if(scene_number == 3) 
    {
        flyingHanumanTx += 0.001; 
		if(flyingHanumanTx >= 0.0) 
		{
			flyingHanumanSx -= 0.00015; 
			flyingHanumanSy -= 0.00015; 
		} 

		if(flyingHanumanTx > 1.2f) 
			scene_number += 1;
    } 
    else if(scene_number == 4) 
    {
		static float scene4Timer = 100.0f; 
		if(scene4Timer > 0.0f) 
			scene4Timer -= 0.01f; 
		else 
			scene_number += 1; 

		if(scene4HanumaanTy > 0.01) 
			scene4HanumaanTy -= 0.0004; 
    } 
    else if(scene_number == 5) 
    {
		static float scene5Timer = 200; 
		if(scene5Timer > 0.0) 
			scene5Timer -= 0.01; 
		else 
			scene_number += 1; 

        if(hanumaanSx <= 1.2f) 
        {
            hanumaanTy += 0.0005; 
            hanumaanSx += 0.001; 
            hanumaanSy += 0.001; 
        } 
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
            
        case 52:
            tx -= 0.01f;
            display();
            break;
        case 54:
            tx += 0.01f;
            display();
            break;
        case 50:
            ty -= 0.01f;
            display();
            break;
        case 56:
            ty += 0.01f;
            display();
            break;
        case 'x':
            sx += 0.1f;
            display();
            break;
        case 'X':
            sx -= 0.1f;
            display();
            break;
        case 'y':
            sy += 0.1f;
            display();
            break;
        case 'Y':
            sy -= 0.1f;
            display();
            break;
        case 'p':
            fprintf(logFile, "glTranslatef(%.2f, %.2f, %.2f);\nglScalef(%.2f, %.2f, %.2f);\n", tx, ty, tz, sx, sy, sz);
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
    if(logFile != NULL) 
    {
        fclose(logFile); 
        logFile = NULL; 
    } 
} 

