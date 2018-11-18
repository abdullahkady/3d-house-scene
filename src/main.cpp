//	Includes
#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sstream>

#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)
double clockRotation = 0;
bool shouldChangeColor = false;

double redShade = 0;
double greenShade = 0;
double blueShade = 0;

double extraRedShade = 0;
double extraGreenShade = 0;
double extraBlueShade = 0;

void resetColors()
{
	redShade = 0;
	greenShade = 0;
	blueShade = 0;
	extraRedShade = 0;
	extraGreenShade = 0;
	extraBlueShade = 0;
	glutPostRedisplay();
}

class Vector3f
{
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f const &v)
	{
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f &v)
	{
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n)
	{
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n)
	{
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit()
	{
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v)
	{
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera
{
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 1.0f, float eyeY = 1.0f, float eyeZ = 1.0f, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f)
	{
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
		Vector3f view = (center - eye).unit();
		eye = eye + view * -0.7;
		center = center + view * -0.7;
	}

	void moveX(float d)
	{
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d)
	{
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d)
	{
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a)
	{
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a)
	{
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look()
	{
		gluLookAt(
				eye.x, eye.y, eye.z,
				center.x, center.y, center.z,
				up.x, up.y, up.z);
	}
};

Camera camera;

void setDefaultColor()
{
	glColor3f(0.8, 0.8, 0.8);
	glutPostRedisplay();
}

void setupLights()
{
	GLfloat ambient[] = {0.7f, 0.7f, 0.7, 1.0f};
	GLfloat diffuse[] = {0.6f, 0.6f, 0.6, 1.0f};
	GLfloat specular[] = {1.0f, 1.0f, 1.0, 1.0f};
	GLfloat shininess[] = {50};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	GLfloat lightIntensity[] = {0.7f, 0.7f, 1, 1.0f};
	// GLfloat lightPosition[] = { -7.0f, 6.0f,3.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightIntensity);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightIntensity);
}

void setupCamera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, 640 / 480, 0.001, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

void drawWall(double thickness)
{
	glPushMatrix();
	glTranslated(0.5, 0.5 * thickness, 0.5);
	glScaled(1.0, thickness, 1.0);
	glColor3f(0.9 + redShade, 0.9 + extraGreenShade, 1 - blueShade);
	glutSolidCube(1);
	glPopMatrix();
	setDefaultColor();
}

void drawTableLeg(double thick, double len)
{
	glPushMatrix();
	glTranslated(0, len / 2, 0);
	glScaled(thick, len, thick);
	glutSolidCube(1.0);
	glPopMatrix();
}

void drawTable(double topWid, double topThick, double legThick, double legLen)
{
	glPushMatrix();
	glTranslated(0, legLen, 0);
	glScaled(topWid, topThick, topWid);
	glutSolidCube(1.0);
	glPopMatrix();

	double dist = 0.95 * topWid / 2.0 - legThick / 2.0;
	glPushMatrix();
	glTranslated(dist, 0, dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(-2 * dist, 0, 2 * dist);
	drawTableLeg(legThick, legLen);
	glTranslated(0, 0, -2 * dist);
	drawTableLeg(legThick, legLen);
	glPopMatrix();
}

void drawChair()
{
	glPushMatrix();

	// CHAIR BACK
	glPushMatrix();
	glColor3f(0.6 + greenShade, 0 + blueShade, 0.8 - extraRedShade);
	glTranslated(-.25, 1.03, -0.1);
	glScaled(0.8, 1.1, 0.08);
	glutSolidCube(1);
	glPopMatrix();
	// CHAIR BACK

	// CHAIR BOTTOM
	glPushMatrix();
	glColor3f(0.6 + blueShade, 0.8 - extraGreenShade, 0 + redShade + extraBlueShade);
	glTranslated(-.25, 0.5, 0.25);
	glScaled(0.8, 0.08, 0.8);
	glutSolidCube(1);
	glPopMatrix();
	// CHAIR BOTTOM

	// 4 LEGS
	glPushMatrix();
	glScaled(0.1, 1, 0.1);
	glColor3f(0.6 + extraGreenShade, 0.3 + extraRedShade - blueShade, 0.1 + greenShade + redShade);
	glPushMatrix();
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-5, 0, 0);
	glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
	glTranslated(-5, 0, 5);
	glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
	glTranslated(0, 0, 5);
	glutSolidCube(1);
	glPopMatrix();

	glPopMatrix();
	// 4 LEGS

	glPopMatrix();
}

void drawFirstRoom()
{
	glPushMatrix();

	glTranslated(0.25, 0.13, 0.7);
	glScaled(0.15, 0.15, 0.15);
	glRotated(45, 0, 1, 0);
	drawChair();
	glPopMatrix();
	// CLOCK
	glPushMatrix();
	glColor3f(0 + redShade, 0 + greenShade, 0 + extraBlueShade);
	glTranslated(0.4, 0.55, 0);
	glScaled(0.15, 0.15, 0.15);
	// CLOCK ARM
	glPushMatrix();
	glRotated(clockRotation, 0, 0, 1);
	glTranslated(0.5, 0.1, 0);
	glScaled(1, 0.1, 0.1);
	glutSolidCube(1);
	glPopMatrix();
	// CLOCK ARM

	glutSolidTorus(0.1, 1, 25, 25);
	glPopMatrix();
	// CLOCK

	glPushMatrix();
	setDefaultColor();
	glScaled(0.8, 0.8, 0.8);
	glTranslated(0.4, 0, 0.4);

	glPushMatrix();
	glTranslated(0.6, 0.38, 0.5);
	glRotated(30, 0, 1, 0);
	glColor3f(0.3 + extraRedShade, 0.5 - greenShade + extraGreenShade, 0.5 - extraBlueShade);
	glutSolidTeapot(0.08);
	glPopMatrix();

	// ORANGE THING
	glPushMatrix();

	glTranslated(0.25, 0.35, 0.35);
	// ROOT THING
	glPushMatrix();
	glColor3f(0 + redShade + extraRedShade, 1 - greenShade, 0 + extraBlueShade);
	glScaled(0.4, 1.3, 0.4);
	glTranslated(0, 0.04, 0);
	glRotated(-90, 1, 0, 0);
	glutSolidCube(0.01);
	glPopMatrix();
	// ROOT THING

	glColor3f(1 - redShade, 0.6 + greenShade - extraGreenShade, 0 + blueShade);
	glutSolidSphere(0.05, 15, 15);
	glPopMatrix();

	setDefaultColor();

	glPushMatrix();
	glTranslated(0.4, 0.0, 0.4);
	glColor3f(1 - extraRedShade, 1 - greenShade, 0.7 + extraBlueShade - blueShade);
	drawTable(0.6, 0.02, 0.02, 0.3);
	glPopMatrix();

	glPopMatrix();

	// ============ WALLS ============ //
	drawWall(0.02);
	glPushMatrix();
	glRotated(90, 0, 0, 1.0);
	drawWall(0.02);
	glPopMatrix();
	glPushMatrix();
	glRotated(-90, 1.0, 0.0, 0.0);
	drawWall(0.02);
	glPopMatrix();
	// ============ WALLS ============ //
}

void drawCloset()
{
	glPushMatrix();
	// Closet handle
	glPushMatrix();
	glColor3f(1 - redShade, 0.9 - extraGreenShade, 1 - blueShade);
	glTranslated(0.2, 0, 0.5);
	glScaled(0.15, 0.65, 0.15);
	glScaled(.5, .5, .5);
	glutSolidSphere(1, 15, 15);
	glPopMatrix();
	// Closet handle

	// Closet corner line decoration thing
	glPushMatrix();
	glColor3f(0.5 + extraRedShade - redShade, 1 - greenShade - extraGreenShade, 0.8 + blueShade - extraBlueShade);
	glTranslated(0, 0.7, 0.5);
	glScaled(1, 0.1, 0.05);
	glutSolidCube(1);
	glPopMatrix();
	glPushMatrix();
	glColor3f(0.5 + extraRedShade - redShade, 1 - greenShade - extraGreenShade, 0.8 + blueShade - extraBlueShade);
	glTranslated(0, 0.9, 0.5);
	glScaled(1, 0.1, 0.05);
	glutSolidCube(1);
	glPopMatrix();

	// Closet corner line decoration thing

	glScaled(1, 3, 1);
	glColor3f(0.6 + extraRedShade, 0.4 - greenShade + extraGreenShade, 0 - extraBlueShade);
	glutSolidCube(1);
	glPopMatrix();
}

void drawCoatHanger()
{
	glPushMatrix();
	glutSolidCone(1, 3, 15, 15);
	glTranslated(0, 0, 3);
	glutSolidSphere(1, 15, 15);
	glPopMatrix();
}

void drawBed()
{
	glPushMatrix();
	// BACKSIDE
	glPushMatrix();
	glColor3f(0.7 + blueShade - extraGreenShade, 0.3 + extraRedShade, 0.2 + extraBlueShade);
	glScaled(1, 1, 0.1);
	glutSolidCube(1);
	glPopMatrix();

	// BODY
	glPushMatrix();
	glTranslated(0, -0.25, 2);
	glPushMatrix();
	glColor3f(.6 + greenShade, 1 - redShade, 0.5 + extraRedShade); // Mattress green(ish) color
	glScaled(1, 0.5, 4);
	glutSolidCube(1);
	glPopMatrix();
	// END PART THING ?
	glPushMatrix();
	glColor3f(0.7 + blueShade - extraGreenShade, 0.3 + extraRedShade, 0.2 + extraBlueShade);
	glTranslated(0, 0.1, 2.1);
	glScaled(1, 0.7, 0.3);
	glutSolidCube(1);
	glPopMatrix();
	glPopMatrix();

	glPopMatrix();
}

void drawCounter()
{
	glPushMatrix();

	// LAMP/DECORATION THING
	glPushMatrix();
	glTranslated(0, 0.55, 0);
	glScaled(0.2, 0.2, 0.2);
	// STAND BASE
	glPushMatrix();
	glColor3f(0 + redShade, 0 + blueShade, 0 + extraBlueShade);
	glScaled(0.3, 4, 0.3);
	glutSolidCube(1);
	glPopMatrix();
	// STAND BASE
	glPushMatrix();
	glColor3f(1 - extraGreenShade, 1 - extraBlueShade, 0.1 + blueShade);
	glTranslated(0, 2.75, 0);
	glutSolidIcosahedron();
	glPopMatrix();
	glPopMatrix();
	// LAMP/DECORATION THING

	// TOP PART
	glPushMatrix();
	glColor3f(0.6 + extraGreenShade, 1 - redShade, 0.7 + blueShade - extraBlueShade);
	glTranslated(0, 0.5, 0);
	glScaled(1.2, 0.1, 1.2);
	glutSolidCube(1);
	glPopMatrix();
	// TOP PART

	// BODY
	glPushMatrix();
	glColor3f(0.8 - extraBlueShade, 0.4 + extraGreenShade - redShade, 0 + greenShade + blueShade);
	glutSolidCube(1);
	glPopMatrix();
	// BODY

	// LEGS
	glPushMatrix();
	glColor3f(0.8 - extraBlueShade, 0.4 + extraGreenShade - redShade, 0 + greenShade + blueShade);
	glTranslated(0.3, -0.5, -0.3);
	glScaled(0.2, 1, 0.2);
	glPushMatrix();
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-3, 0, 0);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(-3, 0, 3);
	glutSolidCube(1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, 0, 3);
	glutSolidCube(1);
	glPopMatrix();

	glPopMatrix();
	// LEGs

	glPopMatrix();
}

void drawSecondRoom()
{

	// COUNTER
	glPushMatrix();
	glTranslated(-0.15, 0.17, 0.8);
	glScaled(0.15, 0.15, 0.15);
	drawCounter();
	glPopMatrix();
	// COUNTER

	// TRASH BIN
	glPushMatrix();
	glTranslated(-0.9, 0, -0.1);
	glColor3f(0 + redShade + extraRedShade, 0.2 + greenShade, 0 + blueShade + extraBlueShade);
	glTranslated(0.15, 0.2, 0.2);
	glRotated(90, 1, 0, 0);
	glScaled(0.04, 0.04, 0.04);
	gluCylinder(gluNewQuadric(), 1, 1, 5, 20, 20);
	glPopMatrix();
	// TRASH BIN

	glPushMatrix();
	glRotated(-90, 0, 1, 0);
	glScaled(0.25, 0.25, 0.15);
	glTranslated(2, 0.6, 0.3);
	drawBed();
	glPopMatrix();

	// CLOSET
	glPushMatrix();
	glTranslated(-0.3, 0.3, 0.1);
	glScaled(0.2, 0.2, 0.2);
	drawCloset();
	glPopMatrix();
	// CLOSET

	// HANGERS
	glPushMatrix();
	glColor3f(0.7 + extraRedShade, 0.5 - greenShade, 0 + extraBlueShade + blueShade);
	glTranslated(-0.7, 0.6, 0);
	glScaled(0.015, 0.015, 0.015);
	drawCoatHanger();
	glPushMatrix();
	glTranslated(-3, 0, 0);
	drawCoatHanger();
	glPushMatrix();
	glTranslated(-3, 0, 0);
	drawCoatHanger();
	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
	// HANGERS

	// Back wall
	glPushMatrix();
	glRotated(-90, 1.0, 0.0, 0.0);
	glRotated(-90, 0.0, 1.0, 0.0);
	drawWall(0.02);
	glPopMatrix();

	// Floor
	glPushMatrix();
	glTranslated(-1.0, 0.0, 0.0);
	drawWall(0.02);
	glPopMatrix();
}

void Display()
{
	setupCamera();
	setupLights();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawFirstRoom();
	drawSecondRoom();

	glFlush();
}

void setLeftView()
{
	camera.up.x = -0.00405836;
	camera.up.y = 0.999989;
	camera.up.z = 0.00230124;

	camera.center.x = -0.713009;
	camera.center.y = 0.448415;
	camera.center.z = 0.426624;

	camera.eye.x = -1.71246;
	camera.eye.y = 0.444435;
	camera.eye.z = 0.393797;
}

void setTopView()
{
	camera.up.x = 0.0226687;
	camera.up.y = -0.0143616;
	camera.up.z = -0.99964;

	camera.center.x = -0.00778855;
	camera.center.y = 1.79373;
	camera.center.z = 0.516674;

	camera.eye.x = -0.00758377;
	camera.eye.y = 2.79363;
	camera.eye.z = 0.502314;
}

void setRightView()
{
	camera.up.x = -0.108454;
	camera.up.y = 0.994058;
	camera.up.z = -0.00928754;

	camera.center.x = 0.952321;
	camera.center.y = 0.526236;
	camera.center.z = 0.454693;

	camera.eye.x = 1.94623;
	camera.eye.y = 0.634482;
	camera.eye.z = 0.434158;
}

void setFrontView()
{
	camera.up.x = -0.00171101;
	camera.up.y = 0.995962;
	camera.up.z = -0.0897583;

	camera.center.x = 0.0095462;
	camera.center.y = 0.491564;
	camera.center.z = 1.5339;

	camera.eye.x = 0.0304264;
	camera.eye.y = 0.590316;
	camera.eye.z = 2.62926;
}

void onKeyPress(unsigned char key, int x, int y)
{
	float d = 0.01 * 10;

	switch (key)
	{
	case 'c':
		shouldChangeColor = !shouldChangeColor;
		if (!shouldChangeColor)
			resetColors();
		break;
	case 'w':
		camera.moveY(d);
		break;
	case 's':
		camera.moveY(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveZ(d);
		break;
	case 'e':
		camera.moveZ(-d);
		break;
	case 'h':
		setRightView();
		break;
	case 'g':
		setLeftView();
		break;
	case 'y':
		setTopView();
		break;
	case 'f':
		setFrontView();
		break;
	case GLUT_KEY_ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}

void onSpecialKey(int key, int x, int y)
{
	float a = 3.0;

	switch (key)
	{
	case GLUT_KEY_UP:
		camera.rotateX(a);
		break;
	case GLUT_KEY_DOWN:
		camera.rotateX(-a);
		break;
	case GLUT_KEY_LEFT:
		camera.rotateY(a);
		break;
	case GLUT_KEY_RIGHT:
		camera.rotateY(-a);
		break;
	}

	glutPostRedisplay();
}

void colorDurationHandler(int x)
{
	if (shouldChangeColor)
	{
		redShade = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		greenShade = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		blueShade = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

		extraRedShade = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		extraGreenShade = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		extraBlueShade = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}

	glutTimerFunc(500, colorDurationHandler, 0);
}

void idleCallback()
{
	if (clockRotation < 0)
	{
		clockRotation = 360;
	}
	clockRotation -= 0.05;
}

int main(int argc, char **argv)
{
	srand(time(NULL)); // Random seed
	glutInit(&argc, argv);

	glutInitWindowSize(640, 480);
	glutInitWindowPosition(50, 50);

	glutCreateWindow("3d-house-scene");
	glutDisplayFunc(Display);
	glutKeyboardFunc(onKeyPress);
	glutSpecialFunc(onSpecialKey);
	colorDurationHandler(0);
	glutIdleFunc(idleCallback);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
	return 0;
}