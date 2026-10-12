#include<GL/freeglut.h>
#include <cmath>
#include "global.h" 

//--------------------------------------------------------
// Draw Football
//--------------------------------------------------------
void DrawFootball(float cx, float cy, float radius)
{
	const int segments = 20;

	// Ball
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_POLYGON);
	for (int i = 0; i < segments; i++)
	{
		float angle = 2.0f * PI * i / segments;

		glVertex2f(
			cx + radius * cosf(angle),
			cy + radius * sinf(angle));
	}
	glEnd();

	// Outline
	glColor3f(0.0f, 0.0f, 0.0f);

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < segments; i++)
	{
		float angle = 2.0f * PI * i / segments;

		glVertex2f(
			cx + radius * cosf(angle),
			cy + radius * sinf(angle));
	}
	glEnd();

	// Center Pentagon
	glColor3f(0.0f, 0.0f, 0.0f);

	glBegin(GL_POLYGON);
	for (int i = 0; i < 5; i++)
	{
		float angle = 2.0f * PI * i / 5.0f - PI / 2.0f;

		glVertex2f(
			cx + radius * 0.35f * cosf(angle),
			cy + radius * 0.35f * sinf(angle));
	}
	glEnd();
}

//--------------------------------------------------------
// Ground
//--------------------------------------------------------
void DrawGround()
{
	glColor3f(0.1f, 0.7f, 0.1f);

	glBegin(GL_QUADS);

	glVertex2f(-1.0f, -1.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(1.0f, -0.5f);
	glVertex2f(-1.0f, -0.5f);

	glEnd();
}

//--------------------------------------------------------
// Goal Post
//--------------------------------------------------------
void DrawGoalPost(float x, float y, float width, float height)
{
	glColor3f(1.0f, 1.0f, 1.0f);

	glLineWidth(5.0f);

	glBegin(GL_LINES);

	// Left
	glVertex2f(x, y);
	glVertex2f(x, y + height);

	// Right
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);

	// Top
	glVertex2f(x, y + height);
	glVertex2f(x + width, y + height);

	glEnd();
}

//--------------------------------------------------------
// Goal Net
//--------------------------------------------------------
void DrawGoalNet(float x, float y, float width, float height)
{
	glColor3f(0.85f, 0.85f, 0.85f);

	// Vertical lines
	for (float i = x; i <= x + width; i += width / 8.0f)
	{
		glBegin(GL_LINES);
		glVertex2f(i, y);
		glVertex2f(i, y + height);
		glEnd();
	}

	// Horizontal lines
	for (float j = y; j <= y + height; j += height / 6.0f)
	{
		glBegin(GL_LINES);
		glVertex2f(x, j);
		glVertex2f(x + width, j);
		glEnd();
	}
}

