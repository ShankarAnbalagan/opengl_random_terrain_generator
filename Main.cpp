#include <iostream>
#include <random>
#include <ctime>
#include <windows.h>
#include <GL/glut.h>
#include <time.h>
#include <math.h>

using namespace std;

#define SIZE 16
#define LOW 75
#define MEDIUM 100
#define WIDTH 400
#define ZOOMINC 0.05

mt19937 generatorseed;
uniform_int_distribution<int> distribution(0, 30);



struct index {   //Structure to hold index values
	int i, j;
};

struct point {   //Structure to hold point attributes.
	float x, y, z;
	float pointColor[3];
};

int flagRegenerate = 1;   //Used to generate a new terrain
int flagSplash = 1;       //Flag for splash screen.
int flagMesh = 0;         //Flag to switch between mesh and terrain view

//Data Structures
point mesh[SIZE + 1][SIZE + 1];
int angleX = 0, angleY = 0, angleZ = 0;
double zoom = 1;

//Function declarations
void display();
void myReshape(int w, int h);
void keyboard(unsigned char key, int mousex, int mousey);
void drawMesh();
void generatePoints(float);
void diamondSquare(index, index, index, index, int);
void regenMesh(float);
void setColor(index);
float getRandomF(int mod);
void splashScreen();
void drawString(float x, float y, float z, const char* String);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowPosition(10, 10);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Landscape!");

	//callback registration
	glutDisplayFunc(display);
	glutReshapeFunc(myReshape);
	glutKeyboardFunc(keyboard);
	glEnable(GL_DEPTH_TEST);

	//begin
	glutMainLoop();
	return 0;
}


//Reshape Function
//This fucntion prevents the image/objects from being distorted when display window is reshaped
void myReshape(int w, int h)
{
	float winsize = w < h ? w : h;
	glViewport(0, 0, winsize, winsize);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-600.0, 600.0, -600.0, 600.0, -1600.0, 1600.0);
	glMatrixMode(GL_MODELVIEW);
}

//Keyboard function
//This function is used for rotating the terrain and zooming functions
void keyboard(unsigned char key, int mousex, int mousey)
{
	if (key == 'D' || key == 'd')
		angleY = (angleY + 5) % 360;
	else if (key == 'A' || key == 'a')
		angleY = (angleY - 5) % 360;
	else if (key == 'W' || key == 'w')
		angleX = (angleX + 5) % 360;
	else if (key == 'S' || key == 's')
		angleX = (angleX - 5) % 360;
	else if (key == 'R' || key == 'r')
		angleZ = (angleZ + 5) % 360;
	else if (key == 'F' || key == 'f')
		angleZ = (angleZ - 5) % 360;
	else if (key == 'q')
		exit(0);
	else if (key == 'g' || key == 'G')
	{
		flagRegenerate = 1;
		flagSplash = 0;
	}
	else if (key == 'z' || key == 'Z')
	{
		zoom += ZOOMINC;
	}
	else if (key == 'x' || key == 'X')
	{
		zoom -= ZOOMINC;
	}
	else if (key == 'm' || key == 'M')
	{
		flagMesh = !flagMesh;
	}
	glutPostRedisplay();
}

//DrawString Function
//This function prints string on the display window
void drawString(float x, float y, float z, const char* String)
{
	glRasterPos3f(x, y, z);
	int i;
	for (i = 0; i < strlen(String); i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, String[i]);
	}
}

//Splash Screen Function
void splashScreen()
{
	int x;
	glutSwapBuffers();
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glColor3f(1.0, 1.0, 1.0);
	drawString(-410, 150, 300, "R A N D O M  T E R R A I N    G E N E R A T O R");
	drawString(-430, 40, 300, "S H A N A K R   A N B A L A G A N - 1 R N 1 6 C S 0 9 3");
	drawString(-370, 0, 300, "S H I V A N K   K U M A R - 1 R N 1 6 C S 1 0 0");
	glColor3f(1.0, 0.0, 1.0);
	drawString(-380, -300, 300, "P R E S S   G   T O   P R O C E D U R A L L Y");
	drawString(-300, -340, 300, "G E N E R A T E   A   T E R R A I N");
	glutSwapBuffers();
}

//Display Function
void display()
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	if (flagSplash)
	{
		splashScreen();
	}
	else
	{
		glLoadIdentity();
		glClearColor(0.0, 0.0, 0.0, 1.0);
		gluLookAt(200, 200, 200, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		glColor4f(253 / 255.0, 227 / 255.0, 167 / 255.0, 0.5);

		glScaled(zoom, zoom, zoom);
		glRotated(angleX, 1.0, 1.0, 0.0);
		glRotated(angleY, 0.0, 1.0, 0.0);
		glRotated(angleZ, 0.0, 0.0, 1.0);

		//outer boundary
		glBegin(GL_LINE_LOOP);
		glVertex3f(-WIDTH, 0.0, -WIDTH);
		glVertex3f(-WIDTH, 0.0, WIDTH);
		glVertex3f(WIDTH, 0.0, WIDTH);
		glVertex3f(WIDTH, 0.0, -WIDTH);
		glEnd();
		if (flagRegenerate)
			regenMesh(WIDTH);
		drawMesh();     //Draw the mesh
		glutSwapBuffers();
	}
}

//Function to generate points
void generatePoints(float width)
{
	int i = 0, j = 0;
	float zVal = -width, xVal = -width, incrementVal = (2.0 * width) / SIZE;

	for (i = 0; i < (SIZE + 1); i++)
	{
		xVal = -width;
		for (j = 0; j < (SIZE + 1); j++)
		{
			mesh[i][j].x = xVal;
			mesh[i][j].y = 0.0;
			mesh[i][j].z = zVal;
			xVal += incrementVal;
		}
		zVal += incrementVal;
	}
}

//Function to generate random value
float getRandomF(int mod)
{
	return distribution(generatorseed);
}

//Function to perform Diamond-Square Algorithm
void diamondSquare(index a, index b, index c, index d, int iteration)
{
	if (iteration != 1)
	{
		index abMid, bcMid, cdMid, daMid, diamondCenter;

		//midpoints to find diamond
		abMid.i = a.i;
		abMid.j = (a.j + b.j) / 2;
		bcMid.i = (b.i + c.i) / 2;
		bcMid.j = b.j;
		cdMid.i = c.i;
		cdMid.j = (c.j + d.j) / 2;
		daMid.i = (a.i + d.i) / 2;
		daMid.j = d.j;

		mesh[abMid.i][abMid.j].y = (mesh[a.i][a.j].y + mesh[b.i][b.j].y) / 2.0 + getRandomF(30);
		mesh[bcMid.i][bcMid.j].y = (mesh[b.i][b.j].y + mesh[c.i][c.j].y) / 2.0 + getRandomF(30);
		mesh[cdMid.i][cdMid.j].y = (mesh[c.i][c.j].y + mesh[d.i][d.j].y) / 2.0 + getRandomF(30);
		mesh[daMid.i][daMid.j].y = (mesh[d.i][d.j].y + mesh[a.i][a.j].y) / 2.0 + getRandomF(30);

		cout << endl << getRandomF(30);
		//Find The Center Of The Diamond
		diamondCenter.i = daMid.i;
		diamondCenter.j = abMid.j;
		srand(time(NULL));

		mesh[diamondCenter.i][diamondCenter.j].y = (mesh[abMid.i][abMid.j].y + mesh[bcMid.i][bcMid.j].y + 
			mesh[cdMid.i][cdMid.j].y + mesh[daMid.i][daMid.j].y) / 4.0 - rand() % 30;

		diamondSquare(a, abMid, diamondCenter, daMid, iteration / 2);
		diamondSquare(abMid, b, bcMid, diamondCenter, iteration / 2);
		diamondSquare(diamondCenter, bcMid, c, cdMid, iteration / 2);
		diamondSquare(daMid, diamondCenter, cdMid, d, iteration / 2);
	}
}

//Function to set color to points
void setColor(index p)
{
	if (mesh[p.i][p.j].y < LOW)//green
	{
		mesh[p.i][p.j].pointColor[0] = 99 / 255.0;
		mesh[p.i][p.j].pointColor[1] = 186 / 255.0;
		mesh[p.i][p.j].pointColor[2] = 86 / 255.0;
	}
	else if (mesh[p.i][p.j].y >= LOW && mesh[p.i][p.j].y < MEDIUM)//brown
	{
		mesh[p.i][p.j].pointColor[0] = 185 / 255.0;
		mesh[p.i][p.j].pointColor[1] = 122 / 255.0;
		mesh[p.i][p.j].pointColor[2] = 87 / 255.0;
	}
	else//white
	{
		mesh[p.i][p.j].pointColor[0] = 1.0;
		mesh[p.i][p.j].pointColor[1] = 1.0;
		mesh[p.i][p.j].pointColor[2] = 1.0;
	}
}

//Function to draw the mesh
void drawMesh()
{
	int i = 0, j = 0;
	index holder;
	for (i = 0; i < SIZE; i++)
	{
		for (j = 0; j < SIZE; j++)
		{
			if (flagMesh == 0)
			{
				glBegin(GL_POLYGON);
				holder.i = i; holder.j = j + 1;
				setColor(holder);
				glColor3fv(mesh[i][j + 1].pointColor);
				glVertex3f(mesh[i][j + 1].x, mesh[i][j + 1].y, mesh[i][j + 1].z);

				holder.i = i + 1; holder.j = j + 1;
				setColor(holder);
				glColor3fv(mesh[i + 1][j + 1].pointColor);
				glVertex3f(mesh[i + 1][j + 1].x, mesh[i + 1][j + 1].y, mesh[i + 1][j + 1].z);


				holder.i = i + 1; holder.j = j;
				setColor(holder);
				glColor3fv(mesh[i + 1][j].pointColor);
				glVertex3f(mesh[i + 1][j].x, mesh[i + 1][j].y, mesh[i + 1][j].z);


				holder.i = i; holder.j = j;
				setColor(holder);
				glColor3fv(mesh[i][j].pointColor);
				glVertex3f(mesh[i][j].x, mesh[i][j].y, mesh[i][j].z);
				glFlush();
				glEnd();

				glColor3f(0.0 / 255.0, 0.0 / 255.0, 0.0 / 255.0);
				glBegin(GL_LINE_LOOP);
				glVertex3f(mesh[i][j + 1].x, mesh[i][j + 1].y + 1, mesh[i][j + 1].z);
				glVertex3f(mesh[i + 1][j + 1].x, mesh[i + 1][j + 1].y + 1, mesh[i + 1][j + 1].z);
				glVertex3f(mesh[i + 1][j].x, mesh[i + 1][j].y + 1, mesh[i + 1][j].z);
				glVertex3f(mesh[i][j].x, mesh[i][j].y + 1, mesh[i][j].z);
				glEnd();
			}
			else if (flagMesh == 1)
			{
				glColor3f(255.0 / 255.0, 255.0 / 255.0, 255.0 / 255.0);
				glBegin(GL_LINE_LOOP);
				glVertex3f(mesh[i][j + 1].x, mesh[i][j + 1].y + 1, mesh[i][j + 1].z);
				glVertex3f(mesh[i + 1][j + 1].x, mesh[i + 1][j + 1].y + 1, mesh[i + 1][j + 1].z);
				glVertex3f(mesh[i + 1][j].x, mesh[i + 1][j].y + 1, mesh[i + 1][j].z);
				glVertex3f(mesh[i][j].x, mesh[i][j].y + 1, mesh[i][j].z);
				glEnd();
			}
		}
	}
}

//Function to regenerate the mesh
void regenMesh(float width)
{
	index a, b, c, d;
	generatePoints(width);

	a.i = 0, a.j = 0;
	b.i = 0, b.j = SIZE;
	c.i = SIZE, c.j = SIZE;
	d.i = SIZE, d.j = 0;

	mesh[a.i][a.j].y = 100;    setColor(a);
	mesh[b.i][b.j].y = 0;	  setColor(b);
	mesh[c.i][c.j].y = 55;    setColor(c);
	mesh[d.i][d.j].y = 20;    setColor(d);

	diamondSquare(a, b, c, d, SIZE);
	flagRegenerate = 0;
}
