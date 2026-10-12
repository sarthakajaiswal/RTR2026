#include<GL/freeglut.h>
#include<math.h>
#include "global.h" 

void Cake(void)
{
	//Top and Bottom Part of cake
	glBegin(GL_QUADS);

	glColor3f(0.621, 0.676, 0.690);
	glVertex2f(0.65f, 0.225f);
	glVertex2f(0.3f, 0.225f);
	glVertex2f(0.3f, 0.2f);
	glVertex2f(0.65f, 0.2f);

	glColor3f(0.936f, 0.718f, 0.970f);
	glVertex2f(0.6f, 0.3f);
	glVertex2f(0.35f, 0.3f);
	glColor3f(0.914f, 0.545f, 0.969f);
	glVertex2f(0.35f, 0.225f);
	glVertex2f(0.6f, 0.225f);

	glColor3f(0.936f, 0.718f, 0.970f);
	glVertex2f(0.55f, 0.4f);
	glVertex2f(0.4f, 0.4f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.4f, 0.3f);
	glVertex2f(0.55f, 0.3f);

	glEnd();

	//Candle
	glBegin(GL_QUADS);
	glColor3f(53.0f / 255.0f, 128.0f / 255.0f, 187.0f / 255.0f);
	glVertex2f(0.485f, 0.5f);
	glVertex2f(0.475f, 0.5f);
	glVertex2f(0.475f, 0.4f);
	glVertex2f(0.485f, 0.4f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(255.0f / 255.0f, 254.0f / 255.0f, 145.0f / 255.0f);
	glVertex2f(0.480f, 0.525f);
	glVertex2f(0.475f, 0.5f);
	glVertex2f(0.485f, 0.5f);
	glEnd();
	
}

void Circle(float cx, float cy, float r, float i, float Angle)
{
	
	glBegin(GL_TRIANGLE_FAN);
	

	glVertex3f(cx, cy, 0.0f); // Center point

	for (i; i <= Angle; i += 0.5f)
	{
		float angle = i * (PI / 180.0f);
		float x = cx + r * cos(angle);
		float y = cy + r * sin(angle);
		glVertex3f(x, y, 0.0f);
	}

	glEnd();
}

void Lady(void)
{

	glBegin(GL_QUADS);

	//Face
	glColor3f(70.0f/255.0f, 40.0f/255.0f, 12.0f/255.0f);
	glVertex2f(0.1f, 0.6f);
	glVertex2f(-0.1f, 0.6f);
	glVertex2f(-0.2f, 0.35f);
	glVertex2f(0.2f, 0.35f);

	glEnd();

	//Face
	glColor3f(0.96, 0.86, 0.73);
	Circle(0.0f, 0.45f, 0.15f,0.0f,360.0f);

	//Left Eye
	glColor3f(0, 0, 0);
	Circle(0.037f, 0.475f, 0.015f,0.0f, 360.0f);

	//Right Eye
	glColor3f(0, 0, 0);
	Circle(-0.037f, 0.475f, 0.015f, 0.0f, 360.0f);

	//Smile

	glColor3f(246.0f / 255.0f, 156.0f / 255.0f, 175.0f / 255.0f);
	Circle(0.0f, 0.4f, 0.05f, 180.0f, 360.0f);

	//Dimple letf
	glColor3f(246.0f / 255.0f, 184.0f / 255.0f, 155.0f / 255.0f);
	Circle(0.1f, 0.425f, 0.017f, 0.0f, 360.0f);

	//Dimple Right
	glColor3f(246.0f / 255.0f, 184.0f / 255.0f, 155.0f / 255.0f);
	Circle(-0.1f, 0.425f, 0.017f, 0.0f, 360.0f);

	//Hair

	glBegin(GL_TRIANGLES);
	glColor3f(70.0f / 255.0f, 40.0f / 255.0f, 12.0f / 255.0f);
	glVertex2f(0.1, 0.6);
	glVertex2f(-0.1, 0.6);
	glVertex2f(0.17, 0.45);

	glVertex2f(0.0, 0.6);
	glVertex2f(-0.1, 0.6);
	glVertex2f(-0.17, 0.45);

	glEnd();

	//*******mid part********

	glBegin(GL_TRIANGLES);
	glColor3f(240.0f / 255.0f, 155.0f / 255.0f, 89.0f / 255.0f);


	glVertex2f(0.0, 0.3);
	glVertex2f(-0.4, -0.5);
	glVertex2f(0.4, -0.5);

	//left hand 
	glColor3f(53.0f / 255.0f, 128.0f / 255.0f, 187.0f / 255.0f);
	glVertex2f(0.1, 0.1);
	glVertex2f(0.3, -0.125);
	glVertex2f(0.5, 0.2);

	//Right hand 
	glColor3f(53.0f / 255.0f, 128.0f / 255.0f, 187.0f / 255.0f);
	glVertex2f(-0.1, 0.1);
	glVertex2f(-0.35, -0.125);
	glVertex2f(-0.3, 0.05);


	glEnd();



	/////////Bottom Part of Skirt
	glBegin(GL_QUADS);
	glColor3f(53.0f / 255.0f, 128.0f / 255.0f, 187.0f / 255.0f);
	glVertex2f(0.3, -0.5);
	glVertex2f(-0.3, -0.5);
	glVertex2f(-0.4, -0.7);
	glVertex2f(0.4, -0.7);

	glEnd();


	///Legs
	glBegin(GL_QUADS);

	//left Leg
	glColor3f(0.96, 0.86, 0.73);
	glVertex2f(0.15, -0.7);
	glVertex2f(0.05, -0.7);
	glVertex2f(0.05, -0.9);
	glVertex2f(0.1, -0.9);


	glColor3f(70.0f / 255.0f, 40.0f / 255.0f, 12.0f / 255.0f);
	glVertex2f(0.1, -0.9);
	glVertex2f(0.05, -0.9);
	glVertex2f(0.05, -1.0);
	glVertex2f(0.2, -1.0);


	//Right Leg

	glColor3f(0.96, 0.86, 0.73);
	glVertex2f(-0.15, -0.7);
	glVertex2f(-0.05, -0.7);
	glVertex2f(-0.05, -0.9);
	glVertex2f(-0.1, -0.9);

	glColor3f(70.0f / 255.0f, 40.0f / 255.0f, 12.0f / 255.0f);
	glVertex2f(-0.1, -0.9);
	glVertex2f(-0.05, -0.9);
	glVertex2f(-0.05, -1.0);
	glVertex2f(-0.2, -1.0);
	glEnd();

}

void displayLadyWithCake(void)
{
	// code
	Lady();
	Cake();
}


