#include <GL/freeglut.h>
#include <math.h>
#include "global.h" 

void drawQuad(float x, float y, float w, float h);
void straightLine(float x1, float y1, float x2, float y2);
void board();
void drawTrapezium(float centerX, float centerY, float bottomWidth, float topWidth, float height);
void drawCircle(float cx, float cy, float r, int slices);
void chairDummy(float x, float y, float r, float g, float b);
void chairArgentina(float x, float y);
void drawWindow();

void displayStudentWithChair(void)
{
	// function declarations 
    void displayComputerTable(void); 

	//Background
	glColor3f(1.0f,1.0f,0.7f);
	glBegin(GL_QUADS);
	{
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
	}
	glEnd();
	glColor3f(0.7f, 0.4f, 0.2f);
	glBegin(GL_QUADS);
	{
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);
		glVertex3f(1.0f, -0.1f, 0.0f);
		glVertex3f(-1.0f, -0.1f, 0.0f);
	}
	glEnd();

	board();

	drawWindow();

	displayComputerTable(); 

	//left left chair
	chairDummy(-0.7f, -0.3f, 0.7f, 0.8f, 0.0f);
	//left chair
	chairDummy(-0.35f, -0.3f, 0.9f, 0.2f, 0.4f);
	//right chair
	chairDummy(0.4f, -0.3f, 0.0f, 0.6f, 0.8f);
	//satvik
	chairArgentina(0.0f, -0.3f);
}


void drawQuad(float x, float y, float w, float h) {
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + w, y);
	glVertex2f(x + w, y + h);
	glVertex2f(x, y + h);
	glEnd();
}

void straightLine(float x1, float y1, float x2, float y2) {
	glLineWidth(3.0f);
	glBegin(GL_LINES);

	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(x1, y1);
	glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(x2, y2);

	glEnd();
}

void board() {
	
	//Notice Board
	glColor3f(0.54f, 0.27f, 0.07f);
	drawQuad(-0.7f, 0.2f, 0.9, 0.6f);

	//stickyNotes and pins
	glColor3f(1.0, 1.0, 0.0f);
	drawQuad(-0.6f, 0.4f, 0.1f, 0.15f);
	straightLine(-0.55f, 0.56f, -0.55f, 0.54f);

	glColor3f(0.9f,0.4f,0.5f);
	drawQuad(-0.1f, 0.3f, 0.1f, 0.15f);
	straightLine(-0.05f, 0.46f, -0.05f, 0.44f);

	glColor3f(0.8f, 0.8f, 1.0f);
	drawQuad(0.0f, 0.5f, 0.1f, 0.15f);
	straightLine(0.05f, 0.66f, 0.05f, 0.64f);

	//Notice Board Title
	glColor3f(0.9f, 0.5f, 0.0f);
	drawQuad(-0.4f, 0.72f, 0.3f, 0.08f);
	//L
	glColor3f(0.0f, 0.0f, 0.0f);
	straightLine(-0.32f, 0.78f, -0.32f, 0.73f);
	straightLine(-0.32f, 0.73f, -0.29f, 0.73f);
	//A
	glColor3f(0.0f, 0.0f, 0.0f);
	straightLine(-0.25f,0.78f,-0.27f,0.72f);
	straightLine(-0.25f, 0.78f, -0.23f, 0.72f);
	straightLine(-0.26f, 0.75f, -0.24f, 0.75f);
	//B
	glColor3f(0.0f, 0.0f, 0.0f);
	straightLine(-0.21f, 0.78f, -0.18f, 0.78f);
	straightLine(-0.21f, 0.755f, -0.18f, 0.755f);
	straightLine(-0.21f, 0.73f, -0.18f, 0.73f);
	straightLine(-0.21f, 0.78f, -0.21f, 0.73f);
	straightLine(-0.18f, 0.78f, -0.18f, 0.73f);
	
	//Notice
	glColor3f(1.0f, 1.0f, 1.0f);
	drawQuad(-0.4, 0.25f, 0.2f, 0.3f);
	straightLine(-0.37f, 0.45f, -0.22f, 0.45f);
	straightLine(-0.37f, 0.4f, -0.22f, 0.4f);
	straightLine(-0.37f, 0.35f, -0.22f, 0.35f);
	straightLine(-0.37f, 0.31f, -0.22f, 0.31f);
	straightLine(-0.3f, 0.56f, -0.3f, 0.54f);

}

void drawTrapezium(float centerX, float centerY, float bottomWidth, float topWidth, float height) {
	float halfBottom = bottomWidth / 2.0f;
	float halfTop = topWidth / 2.0f;

	glBegin(GL_QUADS);
	glVertex2f(centerX - halfBottom, centerY - height / 2.0f); // bottom left
	glVertex2f(centerX + halfBottom, centerY - height / 2.0f); // bottom right
	glVertex2f(centerX + halfTop, centerY + height / 2.0f); // top right
	glVertex2f(centerX - halfTop, centerY + height / 2.0f); // top left
	glEnd();
}

void drawCircle(float cx, float cy, float r, int slices) {
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

void chairDummy(float x, float y, float r, float g, float b) {

	//dummy person
	glColor3f(r, g, b);
	drawTrapezium(x+(0.25f/2.0f), y + 0.35f + (0.1f/ 2.0f), 0.15f, 0.1f, 0.1f);
	glColor3f(0.0f, 0.0f, 0.0f);
	drawCircle(x + (0.25f / 2.0f), y + 0.51f, 0.05f, 50);

	//person legs
	glColor3f(0.2f, 0.2f, 0.35f);
	drawQuad(x + 0.08f, y - 0.13f, 0.03f, 0.13f);   // left leg
	drawQuad(x + 0.14f, y - 0.13f, 0.03f, 0.13f);   // right leg

	//foot
	glColor3f(0.15f, 0.10f, 0.05f);
	drawQuad(x + 0.08f, y - 0.13f, 0.03f, 0.03f);   // left leg
	drawQuad(x + 0.14f, y - 0.13f, 0.03f, 0.03f);   // right leg

	//chair
	glColor3f(0.8f, 0.0f, 0.2f);
	drawQuad(x, y, 0.25f, 0.35f);
	glColor3f(1.0f, 0.0f, 0.2f);
	drawQuad(x, y, 0.25f, 0.05f);
	glColor3f(0.7f, 0.7f, 0.7f);
	drawQuad(x, y - 0.23f, 0.03f, 0.23f);
	drawQuad(x + 0.04f, y - 0.16f, 0.027f, 0.16f);
	drawQuad(x + 0.22f, y - 0.23f, 0.03f, 0.23f);
	drawQuad(x + 0.18f, y - 0.16f, 0.027f, 0.16f);

}

void chairArgentina(float x, float y) {

	//dummy person
	glColor3f(0.42f, 0.72f, 0.90f);   // Argentina jersey
	drawTrapezium(x + (0.25f / 2.0f), y + 0.35f + (0.1f / 2.0f), 0.15f, 0.1f, 0.1f);

	// white vertical stripes on jersey (3 stripes drawn over the trapezium)
	glColor3f(1.0f, 1.0f, 1.0f);
	drawQuad(x + 0.068f, y + 0.35f, 0.012f, 0.1f);   // left stripe
	drawQuad(x + 0.093f, y + 0.35f, 0.012f, 0.1f);   // left-center stripe
	drawQuad(x + 0.119f, y + 0.35f, 0.012f, 0.1f);   // center stripe
	drawQuad(x + 0.145f, y + 0.35f, 0.012f, 0.1f);   // center-right stripe
	drawQuad(x + 0.170f, y + 0.35f, 0.012f, 0.1f);   // right stripe

	glColor3f(0.0f, 0.0f, 0.0f);
	drawCircle(x + (0.25f / 2.0f), y + 0.51f, 0.05f, 50);

	//person legs
	glColor3f(0.9f, 0.8f, 0.5f);
	drawQuad(x + 0.08f, y - 0.13f, 0.03f, 0.13f);   // left leg
	drawQuad(x + 0.14f, y - 0.13f, 0.03f, 0.13f);   // right leg

	//foot
	glColor3f(0.15f, 0.10f, 0.05f);
	drawQuad(x + 0.08f, y - 0.13f, 0.03f, 0.03f);   // left leg
	drawQuad(x + 0.14f, y - 0.13f, 0.03f, 0.03f);   // right leg

	//chair
	glColor3f(0.8f, 0.0f, 0.2f);
	drawQuad(x, y, 0.25f, 0.35f);
	glColor3f(1.0f, 0.0f, 0.2f);
	drawQuad(x, y, 0.25f, 0.05f);
	glColor3f(0.7f, 0.7f, 0.7f);
	drawQuad(x, y - 0.23f, 0.03f, 0.23f);
	drawQuad(x + 0.04f, y - 0.16f, 0.027f, 0.16f);
	drawQuad(x + 0.22f, y - 0.23f, 0.03f, 0.23f);
	drawQuad(x + 0.18f, y - 0.16f, 0.027f, 0.16f);
}

void drawWindow() {

	//Frame
	glColor3f(0.8f, 0.8f, 0.8f);
	drawQuad(0.3f, 0.2f, 0.4, 0.6f);

	//window
	glColor3f(0.6f, 0.8f, 1.0f);
	drawQuad(0.33f, 0.24f, 0.34, 0.52f);

	//sun
	glColor3f(0.9f,1.0f,0.3f);
	drawCircle(0.55f, 0.6f, 0.05f, 50);

}