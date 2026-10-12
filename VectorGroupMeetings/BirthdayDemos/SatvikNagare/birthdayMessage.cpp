#include<GL/freeglut.h>
#include<math.h>
#include<stdio.h>

#include "global.h" 

#define DEG_TO_RAD  3.14/180.0 

float Satviktx = 0.0f, Satvikty = -0.90f;
float Satviksx = 0.43f, Satviksy = 0.80f;

float Bdaytx = 0.13f, Bdayty = -0.50f;
float Bdaysx = 0.54f, Bdaysy = 0.69f;

float Happytx = -0.01f, Happyty = -0.32f;
float Happysx = 0.97f, Happysy = 0.98f;

void createCircle(float originX, float originY, int Segments, float xEyeRadius, float yEyeRadius)
{
	for (int i = 0; i <= Segments; i++)
	{
		float theta = 2.0f * PI * float(i) / float(Segments);

		float x = (originX)+(xEyeRadius * cosf(theta)); // X-component
		float y = (originY)+(yEyeRadius * sinf(theta)); // Y-component

		glVertex3f(x, y, 0.0f);
	}

}

void createHalfCircle(float originX, float originY, int smileStartSegments, int Segments, int smileStopSegment, float xEyeRadius, float yEyeRadius)
{
	for (int i = smileStartSegments; i <= Segments - smileStopSegment; i++)
	{
		float theta = 2.0f * PI * float(i) / float(Segments);

		float x = (originX)+(xEyeRadius * cosf(theta)); // X-component
		float y = (originY)+(yEyeRadius * sinf(theta)); // Y-component

		glVertex3f(x, y, 0.0f);
	}

}

void displayHBD(void)
{
	//function declaration
	void ExictedEmoji(void);
	void drawHappy(void);
	void drawBirthday(void);
	void drawSatvik(void);

	// code 
	glPushMatrix();
	{
		glTranslatef(Happytx, Happyty, 0.0f);
		glScalef(Happysx, Happysy, 0.0f);
		drawHappy();
	}
	glPopMatrix();
	
	glPushMatrix();
	{
		glTranslatef(Bdaytx, Bdayty, 0.0f);
		glScalef(Bdaysx, Bdaysy, 0.0f);
		drawBirthday();
	}
	glPopMatrix();
	
	glPushMatrix();
	{
		glTranslatef(Satviktx, Satvikty, 0.0f);
		glScalef(Satviksx, Satviksy, 0.0f);
		drawSatvik();
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(0.26f, 0.29f, 0.0f);
		glScalef(0.08f, 0.10f, 0.0f);
		ExictedEmoji();
	}
	glPopMatrix();

	glPushMatrix();
	{
		glTranslatef(0.07f, 0.29f, 0.0f);
		glScalef(-0.08, 0.10, 0.0f);
		ExictedEmoji();
	}
	glPopMatrix();
}

void ExictedEmoji()
{
	//Face
	int faceSegments = 64;
	float xfaceRadius = 0.5f;
	float yfaceRadius = 0.5f;

	// right eye
	int rEyeSegments = 32;
	float xrEyeRadius = 0.1f;
	float yrEyeRadius = 0.15f;

	// left eye
	int lEyeSegments = 32;
	float xlEyeRadius = 0.1f;
	float ylEyeRadius = 0.15f;

	// inner right eye
	int rEyeSegmentsInner = 32;
	float xrEyeRadiusInner = 0.05f;
	float yrEyeRadiusInner = 0.09f;

	// inner left eye
	int lEyeSegmentsInner = 32;
	float xlEyeRadiusInner = 0.05f;
	float ylEyeRadiusInner = 0.09f;

	// inner small right eye
	int rEyeSegmentsInnerSmall = 16;
	float xrEyeRadiusInnerSmall = 0.01f;
	float yrEyeRadiusInnerSmall = 0.02f;

	// inner small left eye
	int lEyeSegmentsInnerSmall = 16;
	float xlEyeRadiusInnerSmall = 0.01f;
	float ylEyeRadiusInnerSmall = 0.02f;

	//Excited Smile 1
	int smileStartSegments1 = 34;
	int smileStopSegments1 = 2;
	int Segments1 = 64;
	float xSmileRadius1 = 0.27f;
	float ySmileRadius1 = 0.4f;

	//Excited Smile 2
	int smileStartSegments2 = 42;
	int smileStopSegments2 = 10;
	int Segments2 = 64;
	float xSmileRadius2 = 0.5f;
	float ySmileRadius2 = 0.5f;

	//Excited Smile 3
	int smileStartSegments3 = 3;
	int smileStopSegments3 = 45;
	int Segments3 = 64;
	float xSmileRadius3 = 0.2f;
	float ySmileRadius3 = 0.2f;

	//Excited Smile 4
	int smileStartSegments4 = 39;
	int smileStopSegments4 = 14;
	int Segments4 = 64;
	float xSmileRadius4 = 0.27f;
	float ySmileRadius4 = 0.4f;

	//Face
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.988f, 0.867f, 0.231f);
	createCircle(0, 0, faceSegments, xfaceRadius, yfaceRadius);
	glEnd();

	//Left Eye
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 1.0f, 1.0f);
	createCircle(-0.1, 0.15, lEyeSegments, xlEyeRadius, ylEyeRadius);
	glEnd();

	//Outer Eye Line 
	glBegin(GL_LINE_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	createCircle(-0.1, 0.15, lEyeSegments, xlEyeRadius, ylEyeRadius);
	glEnd();

	//Right Eye
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 1.0f, 1.0f);
	createCircle(0.1, 0.15, rEyeSegments, xrEyeRadius, yrEyeRadius);
	glEnd();

	//Outer Eye line 
	glBegin(GL_LINE_STRIP);
	glColor3f(0.0f, 0.0f, 0.0f);
	createCircle(0.1f, 0.15f, rEyeSegments, xrEyeRadius, yrEyeRadius);
	glEnd();

	//EYE JOINT 

	glBegin(GL_QUADS);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.02f, 0.13f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.02f, 0.17f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.02f, 0.17f, 0.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.02f, 0.13f, 0.0f);

	glEnd();

	//Inner Left Eye
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.0f, 0.0f, 0.0f);
	createCircle(-0.1f, 0.11f, lEyeSegmentsInner, xlEyeRadiusInner, ylEyeRadiusInner);
	glEnd();

	//Inner Right Eye
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.0f, 0.0f, 0.0f);
	createCircle(0.1f, 0.11f, rEyeSegmentsInner, xrEyeRadiusInner, yrEyeRadiusInner);
	glEnd();

	//Small Left Eye Ball
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 1.0f, 1.0f);
	createCircle(-0.08f, 0.15f, lEyeSegmentsInnerSmall, xlEyeRadiusInnerSmall, ylEyeRadiusInnerSmall);
	glEnd();

	//Small Right Eye Ball
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 1.0f, 1.0f);
	createCircle(0.12f, 0.15f, rEyeSegmentsInnerSmall, xrEyeRadiusInnerSmall, yrEyeRadiusInnerSmall);
	glEnd();


	//Excited Smile 1
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.0f, 0.0f, 0.0f);
	createHalfCircle(0.0f, 0.0f, smileStartSegments1, Segments1, smileStopSegments1, xSmileRadius1, ySmileRadius1);
	glEnd();

	//Excited Smile 2
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.988f, 0.867f, 0.231f);
	createHalfCircle(0.0f, 0.35f, smileStartSegments2, Segments2, smileStopSegments2, xSmileRadius2, ySmileRadius2);
	glEnd();

	//Excited Smile 3
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.0f, 0.0f);
	createHalfCircle(-0.15f, -0.45f, smileStartSegments3, Segments3, smileStopSegments3, xSmileRadius3, ySmileRadius3);
	glEnd();

	//Excited Smile 4 to cover red
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 0.0f, 0.0f);
	createHalfCircle(0.0f, 0.0f, smileStartSegments4, Segments4, smileStopSegments4, xSmileRadius4, ySmileRadius4);
	glEnd();

}

void drawHappy(void)
{
	glBegin(GL_QUADS);

	// H
	glColor3f(0.4549f, 0.6745f, 0.8745f);
	glVertex3f(-0.50f, 0.70f, 0.0f);
	glVertex3f(-0.45f, 0.70f, 0.0f);
	glVertex3f(-0.45f, 0.45f, 0.0f);
	glVertex3f(-0.50f, 0.45f, 0.0f);

	glVertex3f(-0.35f, 0.70f, 0.0f);
	glVertex3f(-0.30f, 0.70f, 0.0f);
	glVertex3f(-0.30f, 0.45f, 0.0f);
	glVertex3f(-0.35f, 0.45f, 0.0f);


	glVertex3f(-0.45f, 0.60f, 0.0f);
	glVertex3f(-0.35f, 0.60f, 0.0f);
	glVertex3f(-0.35f, 0.55f, 0.0f);
	glVertex3f(-0.45f, 0.55f, 0.0f);


	//A 
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.25f, 0.45f, 0.0f);
	glVertex3f(-0.18f, 0.70f, 0.0f);
	glVertex3f(-0.13f, 0.70f, 0.0f);
	glVertex3f(-0.20f, 0.45f, 0.0f);

	glVertex3f(-0.18f, 0.70f, 0.0f);
	glVertex3f(-0.10f, 0.45f, 0.0f);
	glVertex3f(-0.05f, 0.45f, 0.0f);
	glVertex3f(-0.13f, 0.70f, 0.0f);

	glVertex3f(-0.20f, 0.57f, 0.0f);
	glVertex3f(-0.10f, 0.57f, 0.0f);
	glVertex3f(-0.10f, 0.52f, 0.0f);
	glVertex3f(-0.20f, 0.52f, 0.0f);

	
	// P 
	glColor3f(0.4549f, 0.6745f, 0.8745f);
	glVertex3f(0.00f, 0.70f, 0.0f);
	glVertex3f(0.05f, 0.70f, 0.0f);
	glVertex3f(0.05f, 0.45f, 0.0f);
	glVertex3f(0.00f, 0.45f, 0.0f);

	glVertex3f(0.05f, 0.70f, 0.0f);
	glVertex3f(0.05f, 0.65f, 0.0f);
	glVertex3f(0.10f, 0.65f, 0.0f);
	glVertex3f(0.10f, 0.70f, 0.0f);

	glVertex3f(0.10f, 0.70f, 0.0f);
	glVertex3f(0.10f, 0.60f, 0.0f);
	glVertex3f(0.15f, 0.60f, 0.0f);
	glVertex3f(0.15f, 0.70f, 0.0f);

	glVertex3f(0.05f, 0.60f, 0.0f);
	glVertex3f(0.05f, 0.55f, 0.0f);
	glVertex3f(0.15f, 0.55f, 0.0f);
	glVertex3f(0.15f, 0.60f, 0.0f);

	//P 
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.20f, 0.70f, 0.0f);
	glVertex3f(0.25f, 0.70f, 0.0f);
	glVertex3f(0.25f, 0.45f, 0.0f);
	glVertex3f(0.20f, 0.45f, 0.0f);

	glVertex3f(0.25f, 0.70f, 0.0f);
	glVertex3f(0.25f, 0.65f, 0.0f);
	glVertex3f(0.30f, 0.65f, 0.0f);
	glVertex3f(0.30f, 0.70f, 0.0f);

	glVertex3f(0.30f, 0.70f, 0.0f);
	glVertex3f(0.30f, 0.60f, 0.0f);
	glVertex3f(0.35f, 0.60f, 0.0f);
	glVertex3f(0.35f, 0.70f, 0.0f);

	glVertex3f(0.25f, 0.60f, 0.0f);
	glVertex3f(0.25f, 0.55f, 0.0f);
	glVertex3f(0.35f, 0.55f, 0.0f);
	glVertex3f(0.35f, 0.60f, 0.0f);

	// Y
	glColor3f(0.4549f, 0.6745f, 0.8745f);
	glVertex3f(0.40f, 0.70f, 0.0f);
	glVertex3f(0.47f, 0.70f, 0.0f);
	glVertex3f(0.54f, 0.56f, 0.0f);
	glVertex3f(0.48f, 0.56f, 0.0f);

	glVertex3f(0.58f, 0.70f, 0.0f);
	glVertex3f(0.65f, 0.70f, 0.0f);
	glVertex3f(0.54f, 0.56f, 0.0f);
	glVertex3f(0.48f, 0.56f, 0.0f);

	glVertex3f(0.48f, 0.56f, 0.0f);
	glVertex3f(0.54f, 0.56f, 0.0f);
	glVertex3f(0.54f, 0.45f, 0.0f);
	glVertex3f(0.48f, 0.45f, 0.0f);

	glEnd();
}

void drawBirthday(void)
{
	glBegin(GL_QUADS);

	//B
	glColor3f(0.4549f, 0.6745f, 0.8745f);
	glVertex3f(-1.0f, 0.7f, 0.0f);
	glVertex3f(-0.95f, 0.7f, 0.0f);
	glVertex3f(-0.95f, 0.45f, 0.0f);
	glVertex3f(-1.0f, 0.45f, 0.0f);

	glVertex3f(-0.95f, 0.7f, 0.0f);
	glVertex3f(-0.95f, 0.65f, 0.0f);
	glVertex3f(-0.85f, 0.65f, 0.0f);
	glVertex3f(-0.85f, 0.7f, 0.0f);

	glVertex3f(-0.9f, 0.65f, 0.0f);
	glVertex3f(-0.85f, 0.65f, 0.0f);
	glVertex3f(-0.85f, 0.55f, 0.0f);
	glVertex3f(-0.9f, 0.55f, 0.0f);

	glVertex3f(-0.95f, 0.6f, 0.0f);
	glVertex3f(-0.9f, 0.6f, 0.0f);
	glVertex3f(-0.9f, 0.55f, 0.0f);
	glVertex3f(-0.95f, 0.55f, 0.0f);

	glVertex3f(-0.9f, 0.55f, 0.0f);
	glVertex3f(-0.85f, 0.55f, 0.0f);
	glVertex3f(-0.85f, 0.45f, 0.0f);
	glVertex3f(-0.9f, 0.45f, 0.0f);

	glVertex3f(-0.95f, 0.5f, 0.0f);
	glVertex3f(-0.9f, 0.5f, 0.0f);
	glVertex3f(-0.9f, 0.45f, 0.0f);
	glVertex3f(-0.95f, 0.45f, 0.0f);

	//I
	glColor3f(1.f, 1.0f, 1.0f);
	glVertex3f(-0.8f, 0.7f, 0.0f);
	glVertex3f(-0.8f, 0.65f, 0.0f);
	glVertex3f(-0.65f, 0.65f, 0.0f);
	glVertex3f(-0.65f, 0.7f, 0.0f);

	glVertex3f(-0.75f, 0.7f, 0.0f);
	glVertex3f(-0.75f, 0.5f, 0.0f);
	glVertex3f(-0.7f, 0.5f, 0.0f);
	glVertex3f(-0.7f, 0.65f, 0.0f);

	glVertex3f(-0.8f, 0.5f, 0.0f);
	glVertex3f(-0.8f, 0.45f, 0.0f);
	glVertex3f(-0.65f, 0.45f, 0.0f);
	glVertex3f(-0.65f, 0.5f, 0.0f);

	//R
	glColor3f(0.4549f, 0.6745f, 0.8745f);
	glVertex3f(-0.6f, 0.7f, 0.0f);
	glVertex3f(-0.55f, 0.7f, 0.0f);
	glVertex3f(-0.55f, 0.45f, 0.0f);
	glVertex3f(-0.6f, 0.45f, 0.0f);

	glVertex3f(-0.55f, 0.7f, 0.0f);
	glVertex3f(-0.55f, 0.65f, 0.0f);
	glVertex3f(-0.5f, 0.65f, 0.0f);
	glVertex3f(-0.5f, 0.7f, 0.0f);

	glVertex3f(-0.5f, 0.7f, 0.0f);
	glVertex3f(-0.5f, 0.6f, 0.0f);
	glVertex3f(-0.45f, 0.6f, 0.0f);
	glVertex3f(-0.45f, 0.7f, 0.0f);

	glVertex3f(-0.55f, 0.6f, 0.0f);
	glVertex3f(-0.55f, 0.55f, 0.0f);
	glVertex3f(-0.45f, 0.55f, 0.0f);
	glVertex3f(-0.45f, 0.6f, 0.0f);

	glVertex3f(-0.55f, 0.55f, 0.0f);
	glVertex3f(-0.5f, 0.45f, 0.0f);
	glVertex3f(-0.45f, 0.45f, 0.0f);
	glVertex3f(-0.5f, 0.55f, 0.0f);

	//T
	glColor3f(1.f, 1.0f, 1.0f);
	glVertex3f(-0.4f, 0.7f, 0.0f);
	glVertex3f(-0.4f, 0.65f, 0.0f);
	glVertex3f(-0.25f, 0.65f, 0.0f);
	glVertex3f(-0.25f, 0.7f, 0.0f);

	glVertex3f(-0.35f, 0.65f, 0.0f);
	glVertex3f(-0.35f, 0.45f, 0.0f);
	glVertex3f(-0.3f, 0.45f, 0.0f);
	glVertex3f(-0.3f, 0.65f, 0.0f);

	// H
	glColor3f(0.4549f, 0.6745f, 0.8745f);
	glVertex3f(-0.2f, 0.7f, 0.0f);
	glVertex3f(-0.15f, 0.7f, 0.0f);
	glVertex3f(-0.15f, 0.45f, 0.0f);
	glVertex3f(-0.2f, 0.45f, 0.0f);

	glVertex3f(-0.05f, 0.7f, 0.0f);
	glVertex3f(0.0f, 0.7f, 0.0f);
	glVertex3f(0.0f, 0.45f, 0.0f);
	glVertex3f(-0.05f, 0.45f, 0.0f);

	glVertex3f(-0.15f, 0.6f, 0.0f);
	glVertex3f(-0.05f, 0.6f, 0.0f);
	glVertex3f(-0.05f, 0.55f, 0.0f);
	glVertex3f(-0.15f, 0.55f, 0.0f);


	// D
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.05f, 0.7f, 0.0f);
	glVertex3f(0.1f, 0.7f, 0.0f);
	glVertex3f(0.1f, 0.45f, 0.0f);
	glVertex3f(0.05f, 0.45f, 0.0f);

	glVertex3f(0.1f, 0.7f, 0.0f);
	glVertex3f(0.22f, 0.7f, 0.0f);
	glVertex3f(0.22f, 0.65f, 0.0f);
	glVertex3f(0.1f, 0.65f, 0.0f);

	glVertex3f(0.17f, 0.65f, 0.0f);
	glVertex3f(0.22f, 0.65f, 0.0f);
	glVertex3f(0.22f, 0.5f, 0.0f);
	glVertex3f(0.17f, 0.5f, 0.0f);

	glVertex3f(0.1f, 0.5f, 0.0f);
	glVertex3f(0.22f, 0.5f, 0.0f);
	glVertex3f(0.22f, 0.45f, 0.0f);
	glVertex3f(0.1f, 0.45f, 0.0f);


	// A
	glColor3f(0.4549f, 0.6745f, 0.8745f);
	glVertex3f(0.25f, 0.45f, 0.0f);
	glVertex3f(0.32f, 0.7f, 0.0f);
	glVertex3f(0.37f, 0.7f, 0.0f);
	glVertex3f(0.3f, 0.45f, 0.0f);

	glVertex3f(0.32f, 0.7f, 0.0f);
	glVertex3f(0.40f, 0.45f, 0.0f);
	glVertex3f(0.45f, 0.45f, 0.0f);
	glVertex3f(0.37f, 0.7f, 0.0f);

	glVertex3f(0.3f, 0.57f, 0.0f);
	glVertex3f(0.4f, 0.57f, 0.0f);
	glVertex3f(0.4f, 0.52f, 0.0f);
	glVertex3f(0.3f, 0.52f, 0.0f);

	// Y
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.43f, 0.70f, 0.0f);
	glVertex3f(0.50f, 0.70f, 0.0f);
	glVertex3f(0.57f, 0.56f, 0.0f);
	glVertex3f(0.51f, 0.56f, 0.0f);

	glVertex3f(0.61f, 0.70f, 0.0f);
	glVertex3f(0.68f, 0.70f, 0.0f);
	glVertex3f(0.57f, 0.56f, 0.0f);
	glVertex3f(0.51f, 0.56f, 0.0f);

	glVertex3f(0.51f, 0.56f, 0.0f);
	glVertex3f(0.57f, 0.56f, 0.0f);
	glVertex3f(0.57f, 0.45f, 0.0f);
	glVertex3f(0.51f, 0.45f, 0.0f);


	glEnd();
}

void drawSatvik(void)
{
	glBegin(GL_QUADS);

	// S
	glColor3f(0.4549f, 0.6745f, 0.8745f);
	glVertex3f(-0.60f, 0.70f, 0.0f);
	glVertex3f(-0.50f, 0.70f, 0.0f);
	glVertex3f(-0.45f, 0.67f, 0.0f);
	glVertex3f(-0.55f, 0.67f, 0.0f);

	glVertex3f(-0.60f, 0.70f, 0.0f);
	glVertex3f(-0.55f, 0.67f, 0.0f);
	glVertex3f(-0.55f, 0.56f, 0.0f);
	glVertex3f(-0.60f, 0.59f, 0.0f);

	glVertex3f(-0.60f, 0.59f, 0.0f);
	glVertex3f(-0.50f, 0.59f, 0.0f);
	glVertex3f(-0.45f, 0.56f, 0.0f);
	glVertex3f(-0.55f, 0.56f, 0.0f);

	glVertex3f(-0.50f, 0.59f, 0.0f);
	glVertex3f(-0.45f, 0.56f, 0.0f);
	glVertex3f(-0.45f, 0.45f, 0.0f);
	glVertex3f(-0.50f, 0.48f, 0.0f);

	glVertex3f(-0.60f, 0.48f, 0.0f);
	glVertex3f(-0.50f, 0.48f, 0.0f);
	glVertex3f(-0.45f, 0.45f, 0.0f);
	glVertex3f(-0.55f, 0.45f, 0.0f);

	//A 
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-0.42f, 0.45f, 0.0f);
	glVertex3f(-0.35f, 0.70f, 0.0f);
	glVertex3f(-0.30f, 0.70f, 0.0f);
	glVertex3f(-0.37f, 0.45f, 0.0f);

	glVertex3f(-0.35f, 0.70f, 0.0f);
	glVertex3f(-0.27f, 0.45f, 0.0f);
	glVertex3f(-0.22f, 0.45f, 0.0f);
	glVertex3f(-0.30f, 0.70f, 0.0f);

	glVertex3f(-0.37f, 0.57f, 0.0f);
	glVertex3f(-0.27f, 0.57f, 0.0f);
	glVertex3f(-0.27f, 0.52f, 0.0f);
	glVertex3f(-0.37f, 0.52f, 0.0f);

	//T
	glColor3f(0.4549f, 0.6745f, 0.8745f);
	glVertex3f(-0.20f, 0.7f, 0.0f);
	glVertex3f(-0.20f, 0.65f, 0.0f);
	glVertex3f(-0.03f, 0.65f, 0.0f);
	glVertex3f(-0.03f, 0.7f, 0.0f);

	glVertex3f(-0.14f, 0.65f, 0.0f);
	glVertex3f(-0.14f, 0.45f, 0.0f);
	glVertex3f(-0.09f, 0.45f, 0.0f);
	glVertex3f(-0.09f, 0.65f, 0.0f);

	// V
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.01f, 0.70f, 0.0f);
	glVertex3f(0.06f, 0.70f, 0.0f);
	glVertex3f(0.14f, 0.45f, 0.0f);
	glVertex3f(0.09f, 0.45f, 0.0f);

	glVertex3f(0.09f, 0.45f, 0.0f);
	glVertex3f(0.14f, 0.45f, 0.0f);
	glVertex3f(0.21f, 0.70f, 0.0f);
	glVertex3f(0.16f, 0.70f, 0.0f);

	//I
	glColor3f(0.4549f, 0.6745f, 0.8745f);
	glVertex3f(0.26f, 0.70f, 0.0f);
	glVertex3f(0.26f, 0.65f, 0.0f);
	glVertex3f(0.39f, 0.65f, 0.0f);
	glVertex3f(0.39f, 0.70f, 0.0f);

	glVertex3f(0.30f, 0.70f, 0.0f);
	glVertex3f(0.30f, 0.50f, 0.0f);
	glVertex3f(0.35f, 0.50f, 0.0f);
	glVertex3f(0.35f, 0.65f, 0.0f);

	glVertex3f(0.26f, 0.50f, 0.0f);
	glVertex3f(0.26f, 0.45f, 0.0f);
	glVertex3f(0.39f, 0.45f, 0.0f);
	glVertex3f(0.39f, 0.50f, 0.0f);

	//K
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.45f, 0.70f, 0.0f);
	glVertex3f(0.49f, 0.70f, 0.0f);
	glVertex3f(0.49f, 0.45f, 0.0f);
	glVertex3f(0.45f, 0.45f, 0.0f);

	glVertex3f(0.54f, 0.70f, 0.0f);
	glVertex3f(0.59f, 0.70f, 0.0f);
	glVertex3f(0.49f, 0.55f, 0.0f);
	glVertex3f(0.49f, 0.62f, 0.0f);
	
	glVertex3f(0.49f, 0.62f, 0.0f);
	glVertex3f(0.49f, 0.55f, 0.0f);
	glVertex3f(0.54f, 0.45f, 0.0f);
	glVertex3f(0.59f, 0.45f, 0.0f);
	
	glEnd();
}

//cl.exe /c /EHsc /I C:\freeglut\include BirthdayMessage.cpp
//link.exe BirthdayMessage.obj /LIBPATH:C:\freeglut\lib\x64 /SUBSYSTEM:CONSOLE

