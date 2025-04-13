#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>
#include <iostream>
#include <SOIL/SOIL.h>

#define PI 3.14159265358979323846

using namespace std;

float cc[20][2];
GLuint skyTexture;
GLuint roadTexture;

// Declarații funcții
void init(void);
void display(void);
void keyboard(unsigned char, int, int);
void resize(int, int);
void draw_star(float, float);
void house(float, float, float);
void apart(float, float, float);
void circle1(float);
void stand(float, float, float);
void drawBuildingsWithShadows(void);
void generateShadowMatrix(GLfloat shadowMat[16], GLfloat groundPlane[4], GLfloat lightPos[4]);
void house1(void);
void drawSky(void);
void drawRoad(void);
void loadTreeTexture(void);
void loadRoadTexture(void);
void loadSkyTexture(void);
void drawTree(float x, float y, float z, float trunkHeight, float trunkRadius, float leavesRadius);
void displayInstructions(void);
void renderText(float x, float y, const char* text);
void loadBuildingTexture(void);
void timer(int value);  // Declarația funcției timer

float  h = 5, h1 = 6, d1 = 4, g = 1, g1 = 2;

// Textura pentru trunchiul copacului

GLuint treeTextureTrunk;   // Textura pentru trunchi
GLuint treeTextureLeaves;  // Textura pentru frunze

void loadTreeTexture() {
	// Încarcă textura pentru trunchi
	treeTextureTrunk = SOIL_load_OGL_texture(
		"tree_trunk.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
	);
	if (treeTextureTrunk == 0) {
		std::cout << "Eroare la încărcarea texturii trunchiului!" << std::endl;
	}

	// Încarcă textura pentru frunze
	treeTextureLeaves = SOIL_load_OGL_texture(
		"tree_leaves.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
	);
	if (treeTextureLeaves == 0) {
		std::cout << "Eroare la încărcarea texturii frunzelor!" << std::endl;
	}

	// Setează parametri pentru texturi (opțional)
	glBindTexture(GL_TEXTURE_2D, treeTextureTrunk);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, treeTextureLeaves);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void loadRoadTexture() {
	roadTexture = SOIL_load_OGL_texture("road.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (roadTexture == 0) {
		std::cout << "Eroare la încărcarea texturii drumului!" << std::endl;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// După declarațiile globale existente, adăugăm:
float sunAngle = 0.0f;  // Unghiul soarelui
float sunRadius = 200.0f;  // Raza cercului pe care se mișcă soarele
float sunHeight = 200.0f;  // Înălțimea soarelui

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1100, 1100);
	glutInitWindowPosition(100, 10);
	glutCreateWindow("3D map");
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	glutReshapeFunc(resize);
	glutTimerFunc(0, timer, 0);  // Adăugăm timer-ul

	glutMainLoop();
	return 0;
}

GLuint buildingTexture;

// În funcția de inițializare a texturii:
void loadSkyTexture() {
	skyTexture = SOIL_load_OGL_texture("sky.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (skyTexture == 0) {
		std::cout << "Eroare la încărcarea texturii cerului!" << std::endl;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// Variabile pentru cameră
float cameraX = 0.0f;
float cameraY = 5.0f;
float cameraZ = 20.0f;
float cameraRotX = 0.0f;
float cameraRotY = 0.0f;
float cameraRotZ = 0.0f;
float moveSpeed = 2.0f;

// Proprietăți lumină
GLfloat lightPosition[] = { 50.0f, 200.0f, 50.0f, 1.0f };  // Am mutat soarele mai sus
GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

void init(void)
{
	// Setăm culoarea fundalului la albastru închis pentru cer nocturn
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f);
	
	// Activăm testul de adâncime și normalizarea normalelor
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
    
	// Activăm și configurăm iluminarea
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
	// Setăm proprietățile luminii
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	
	// Activăm materiale și culori
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
	// Setăm modelul de iluminare pentru două fețe
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	
	// Setăm lumina ambientală globală
	GLfloat globalAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
	   
    // Încarcă toate texturile
    loadSkyTexture();
    loadRoadTexture();
    loadTreeTexture();
    loadBuildingTexture();

    // Activăm texturarea
    glEnable(GL_TEXTURE_2D);
}

void drawTree(float x, float y, float z, float trunkHeight, float trunkRadius, float leavesRadius) {
	// Desenează trunchiul (cilindru vertical)
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, treeTextureTrunk);

	GLUquadricObj* trunkQuadric = gluNewQuadric();
	gluQuadricTexture(trunkQuadric, GL_TRUE);

	glPushMatrix();
	glTranslatef(x, y, z);
	glRotatef(-90, 1.0, 0.0, 0.0); // Rotire 90° pentru verticalitate
	gluCylinder(trunkQuadric, trunkRadius, trunkRadius, trunkHeight, 20, 20);
	glPopMatrix();

	gluDeleteQuadric(trunkQuadric);
	glDisable(GL_TEXTURE_2D);

	// Desenează coroana (sferă mai mare cu textură)
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, treeTextureLeaves);

	GLUquadricObj* leavesQuadric = gluNewQuadric();
	gluQuadricTexture(leavesQuadric, GL_TRUE); // Activează textura pe sferă
	gluQuadricNormals(leavesQuadric, GLU_SMOOTH); // Pentru iluminare mai bună

	glPushMatrix();
	glTranslatef(x, y + trunkHeight, z);
	glScalef(1.5, 1.5, 1.5); // Mărește sfera cu 50%
	gluSphere(leavesQuadric, leavesRadius, 30, 30); // Rezoluție mai mare (30 segmente)
	glPopMatrix();

	gluDeleteQuadric(leavesQuadric);
	glDisable(GL_TEXTURE_2D);
}


// Funcție pentru a desena o sferă pe care se aplică textura cerului
void drawSky()
{
	// Activăm texturarea
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skyTexture);  // Leagă textura "sky.jpg"

	// Setează dimensiunea dreptunghiului cerului
	GLfloat width = 1000.0f;  // Lățimea dreptunghiului
	GLfloat height = 1000.0f; // Înălțimea dreptunghiului

	// Desenăm dreptunghiul care reprezintă cerul
	glBegin(GL_QUADS);

	// Coordonate pentru texturare
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, height / 2, -200.0f);  // Colțul stânga sus (aproape de cameră)
	glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, height / 2, -200.0f);   // Colțul dreapta sus (aproape de cameră)
	glTexCoord2f(1.0f, 1.0f); glVertex3f(width / 2, -height / 2, -200.0f);  // Colțul dreapta jos (aproape de cameră)
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-width / 2, -height / 2, -200.0f); // Colțul stânga jos (aproape de cameră)

	glEnd();

	// Dezactivăm texturarea
	glDisable(GL_TEXTURE_2D);
}

void drawRoad()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, roadTexture);  // Leagă textura "road.jpg"

	GLfloat width = 500.0f;  // Lățimea drumului
	GLfloat length = 500.0f; // Lungimea drumului

	// Desenăm dreptunghiul pentru drum
	glBegin(GL_QUADS);

	// Coordonate pentru texturare
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-width / 2, 0.0f, -length / 2);  // Colțul stânga jos
	glTexCoord2f(1.0f, 0.0f); glVertex3f(width / 2, 0.0f, -length / 2);   // Colțul dreapta jos
	glTexCoord2f(1.0f, 1.0f); glVertex3f(width / 2, 0.0f, length / 2);    // Colțul dreapta sus
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-width / 2, 0.0f, length / 2);   // Colțul stânga sus

	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void apart(float x, float y, float z)
{
	int i;
	int j;
	glColor3f(0.7, 0.7, 0.7);
	glBegin(GL_POLYGON);
	glVertex3f(x, y, z + 0.5);
	glVertex3f(x + 45, y, z + 0.5);
	glVertex3f(x + 45, y + 100, z + 0.5);
	glVertex3f(x, y + 100, z + 0.5);
	glEnd();  glColor3f(0.8, 0.8, 0.8);
	for (j = 0; j < 8; j++)
	{

		glPushMatrix();
		glTranslatef(0, -12 * j, 0);
		for (i = 0; i < 4; i++)
		{
			glPushMatrix();
			glTranslatef(11 * i, 0, 0);
			glBegin(GL_POLYGON);
			glVertex3f(x + 2, y + 98, z);
			glVertex3f(x + 10, y + 98, z);
			glVertex3f(x + 10, y + 88, z);
			glVertex3f(x + 2, y + 88, z);
			glEnd();
			glPopMatrix();
		}glPopMatrix();
	}glColor3f(0, 0, 0);
	for (j = 0; j < 8; j++)
	{

		glPushMatrix();
		glTranslatef(0, -12 * j, 0);
		for (i = 0; i < 4; i++)
		{
			glPushMatrix();
			glTranslatef(11 * i, 0, 0);
			glBegin(GL_LINE_LOOP);
			glVertex3f(x + 2, y + 98, z);
			glVertex3f(x + 10, y + 98, z);
			glVertex3f(x + 10, y + 88, z);
			glVertex3f(x + 2, y + 88, z);
			glEnd();
			glPopMatrix();
		}glPopMatrix();
	}

	glColor3f(0.6, 0.6, 0.6);
	glBegin(GL_POLYGON);
	glVertex3f(x, y, z + 45 - 0.5);
	glVertex3f(x + 45, y, z + 45 - 0.5);
	glVertex3f(x + 45, y + 100, z + 45 - 0.5);
	glVertex3f(x, y + 100, z + 45 - 0.5);
	glEnd();  	glColor3f(0.8, 0.8, 0.8);

	for (j = 0; j < 8; j++)
	{

		glPushMatrix();
		glTranslatef(0, -12 * j, 0);
		for (i = 0; i < 4; i++)
		{
			glPushMatrix();
			glTranslatef(11 * i, 0, 0);
			glBegin(GL_POLYGON);
			glVertex3f(x + 2, y + 98, z + 45);
			glVertex3f(x + 10, y + 98, z + 45);
			glVertex3f(x + 10, y + 88, z + 45);
			glVertex3f(x + 2, y + 88, z + 45);
			glEnd();
			glPopMatrix();
		}glPopMatrix();
	}glColor3f(0, 0, 0);

	for (j = 0; j < 8; j++)
	{

		glPushMatrix();
		glTranslatef(0, -12 * j, 0);
		for (i = 0; i < 4; i++)
		{
			glPushMatrix();
			glTranslatef(11 * i, 0, 0);
			glBegin(GL_LINE_LOOP);
			glVertex3f(x + 2, y + 98, z + 45);
			glVertex3f(x + 10, y + 98, z + 45);
			glVertex3f(x + 10, y + 88, z + 45);
			glVertex3f(x + 2, y + 88, z + 45);
			glEnd();
			glPopMatrix();
		}glPopMatrix();
	}

	glColor3f(0.6, 0.6, 0.6);
	glBegin(GL_POLYGON);
	glVertex3f(x + 0.5, y, z);
	glVertex3f(x + 0.5, y, z + 45);
	glVertex3f(x + 0.5, y + 100, z + 45);
	glVertex3f(x + 0.5, y + 100, z);
	glEnd();
	glColor3f(0.8, 0.8, 0.8);
	for (j = 0; j < 8; j++)
	{

		glPushMatrix();
		glTranslatef(0, -12 * j, 0);
		for (i = 0; i < 4; i++)
		{
			glPushMatrix();
			glTranslatef(0, 0, 11 * i);
			glBegin(GL_POLYGON);
			glVertex3f(x, y + 98, z + 2);
			glVertex3f(x, y + 98, z + 10);
			glVertex3f(x, y + 88, z + 10);
			glVertex3f(x, y + 88, z + 2);
			glEnd();
			glPopMatrix();
		}glPopMatrix();
	}glColor3f(0, 0, 0);
	for (j = 0; j < 8; j++)
	{

		glPushMatrix();
		glTranslatef(0, -12 * j, 0);
		for (i = 0; i < 4; i++)
		{
			glPushMatrix();
			glTranslatef(0, 0, 11 * i);
			glBegin(GL_LINE_LOOP);
			glVertex3f(x, y + 98, z + 2);
			glVertex3f(x, y + 98, z + 10);
			glVertex3f(x, y + 88, z + 10);
			glVertex3f(x, y + 88, z + 2);
			glEnd();
			glPopMatrix();
		}glPopMatrix();
	}

	glColor3f(0.6, 0.6, 0.6);

	glBegin(GL_POLYGON);
	glVertex3f(x + 45 - 0.5, y, z - 0.5);
	glVertex3f(x + 45 - 0.5, y, z + 45 - 0.5);
	glVertex3f(x + 45 - 0.5, y + 100, z + 45 - 0.5);
	glVertex3f(x + 45 - 0.5, y + 100, z - 0.5);
	glEnd(); glColor3f(0.8, 0.8, 0.8);
	for (j = 0; j < 8; j++)
	{

		glPushMatrix();
		glTranslatef(0, -12 * j, 0);
		for (i = 0; i < 4; i++)
		{
			glPushMatrix();
			glTranslatef(0, 0, 11 * i);
			glBegin(GL_POLYGON);
			glVertex3f(x + 45, y + 98, z + 2);
			glVertex3f(x + 45, y + 98, z + 10);
			glVertex3f(x + 45, y + 88, z + 10);
			glVertex3f(x + 45, y + 88, z + 2);
			glEnd();
			glPopMatrix();
		}glPopMatrix();
	}
	glColor3f(0, 0, 0);
	for (j = 0; j < 8; j++)
	{

		glPushMatrix();
		glTranslatef(0, -12 * j, 0);
		for (i = 0; i < 4; i++)
		{
			glPushMatrix();
			glTranslatef(0, 0, 11 * i);
			glBegin(GL_LINE_LOOP);
			glVertex3f(x + 45, y + 98, z + 2);
			glVertex3f(x + 45, y + 98, z + 10);
			glVertex3f(x + 45, y + 88, z + 10);
			glVertex3f(x + 45, y + 88, z + 2);
			glEnd();
			glPopMatrix();
		}glPopMatrix();
	}

	glColor3f(0.5, 0.5, 0.5);
	glBegin(GL_POLYGON);
	glVertex3f(x, y, z - 0.5);
	glVertex3f(x + 45, y, z - 0.5);
	glVertex3f(x + 45, y, z + 45 - 0.5);
	glVertex3f(x, y, z + 45 - 0.5);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(x, y + 100, z);
	glVertex3f(x + 45, y + 100, z);
	glVertex3f(x + 45, y + 100, z + 45);
	glVertex3f(x, y + 100, z + 45);
	glEnd();
}

void house(float x, float y, float z)
{
	glEnable(GL_TEXTURE_2D); // Activează utilizarea texturilor
	glBindTexture(GL_TEXTURE_2D, buildingTexture); // Leagă textura clădirii

	// Fața frontală
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 45, y, z);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 45, y + 100, z);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + 100, z);
	glEnd();

	// Fața din spate
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z + 45);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 45, y, z + 45);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 45, y + 100, z + 45);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + 100, z + 45);
	glEnd();

	// Fațele laterale
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x + 45, y, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 45, y, z + 45);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 45, y + 100, z + 45);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x + 45, y + 100, z);
	glEnd();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y, z + 45);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + 100, z + 45);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + 100, z);
	glEnd();

	glDisable(GL_TEXTURE_2D); // Dezactivează utilizarea texturilor
}


void house1()
{

	house(120, 0.1, 50);

	house(120, 0.1, 90);

	house(160, 0.1, 90);

	house(80, 0.1, 90);

	house(160, 0.1, 50);

	house(80, 0.1, 50);

	house(-130, 0.1, 120);

	house(-130, 0.1, 160);

	house(-90, 0.1, 120);

	house(-60, 0.1, 120);

	house(-90, 0.1, 160);

	stand(-10, 6, 130);
	stand(30, 6, 130);

	house(-60, 0.1, 160);
	apart(150, 0, -150);
	apart(80, 0, -90);

	apart(150, 0, -90);
	apart(150, 0, -30);

	apart(-150, 0, -30);
	apart(-150, 0, -110);

	apart(-150, 0, -180);

}
void circle1(float c[20][2])
{
	float x = 0, y = 03;
	int i;
	for (i = 0; i < 20; i++)
	{
		c[i][0] = x * cos((i + 1) * 36 * (3.142 / 360)) - y * sin((i + 1) * 36 * (3.142 / 360));
		c[i][1] = x * sin((i + 1) * 36 * (3.142 / 360)) + y * cos((i + 1) * 36 * (3.142 / 360));
	}

	for (i = 0; i < 19; i++)
	{
		glBegin(GL_POLYGON);
		glVertex3f(c[i][0], c[i][1], 0);
		glVertex3f(c[i][0], c[i][1], 40);
		glVertex3f(c[i + 1][0], c[i + 1][1], 40);
		glVertex3f(c[i + 1][0], c[i + 1][1], 0);
		glEnd();
	}

	glBegin(GL_POLYGON);
	glVertex3f(c[0][0], c[0][1], 0);
	glVertex3f(c[0][0], c[0][1], 40);
	glVertex3f(c[19][0], c[19][1], 40);
	glVertex3f(c[19][0], c[19][1], 0);
	glEnd();

}




void draw_star(GLfloat x, GLfloat y)
{
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);
	glVertex2f(x, y);
	glVertex2f(x + 1.5, y - 4);
	glVertex2f(x + 6.5, y - 5.5);
	glVertex2f(x + 2.5, y - 9);
	glVertex2f(x + 4.5, y - 14);
	glVertex2f(x, y - 11.5);
	glVertex2f(x - 4.5, y - 14);
	glVertex2f(x - 3, y - 9);
	glVertex2f(x - 6.5, y - 5.5);
	glVertex2f(x - 1.5, y - 5);
	glVertex2f(x, y);
	glEnd();
}


void stand(float x, float y, float z)
{
	glColor3f(1, 0.8, 0);
	glBegin(GL_POLYGON);
	glVertex3f(x, y, z);
	glVertex3f(x, y - h1, z + d1);
	glVertex3f(x + h, y - h1, z + d1);
	glVertex3f(x + h, y, z);
	glEnd();
	glColor3f(1, 0.5, 0);
	glBegin(GL_POLYGON);
	glVertex3f(x, y, z);
	glVertex3f(x + h, y, z);
	glVertex3f(x + h, y - h1, z - d1);
	glVertex3f(x, y - h1, z - d1);
	glEnd();

	glColor3f(0.6, 0.12, 0.4);

	glBegin(GL_POLYGON);
	glVertex3f(x, y - h1, z + d1);
	glVertex3f(x, y - h1 - 2, z + d1);
	glVertex3f(x + 1, y - h1 - 2, z + d1);
	glVertex3f(x + 1, y - h1, z + d1);
	glEnd();

	glColor3f(0.6, 0.12, 0.4);
	glBegin(GL_POLYGON);
	glVertex3f(x + h, y - h1, z + d1);
	glVertex3f(x + h, y - h1 - 2, z + d1);
	glVertex3f(x + h - 1, y - h1 - 2, z + d1);
	glVertex3f(x + h - 1, y - h1, z + d1);
	glEnd();

	glColor3f(0.6, 0.12, 0.4);

	glBegin(GL_POLYGON);
	glVertex3f(x, y - h1, z - d1);
	glVertex3f(x, y - h1 - 2, z - d1);
	glVertex3f(x + 1, y - h1 - 2, z - d1);
	glVertex3f(x + 1, y - h1, z - d1);
	glEnd();

	glColor3f(0.6, 0.12, 0.4);

	glBegin(GL_POLYGON);
	glVertex3f(x + h, y - h1, z - d1);
	glVertex3f(x + h, y - h1 - 2, z - d1);
	glVertex3f(x + h - 1, y - h1 - 2, z - d1);
	glVertex3f(x + h - 1, y - h1, z - d1);
	glEnd();
}

// Funcție pentru afișarea textului pe ecran
void renderText(float x, float y, const char* text) {
    glDisable(GL_LIGHTING);  // Dezactivăm iluminarea pentru text
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, glutGet(GLUT_WINDOW_WIDTH), 0.0, glutGet(GLUT_WINDOW_HEIGHT));
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    glColor3f(1.0f, 1.0f, 1.0f);  // Text alb
    glRasterPos2f(x, y);
    
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
    
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);  // Reactivăm iluminarea
}

void displayInstructions() {
    float startX = 10.0f;
    float startY = glutGet(GLUT_WINDOW_HEIGHT) - 20.0f;
    float lineSpacing = 15.0f;
    
    renderText(startX, startY, "Instructiuni control camera:");
    renderText(startX, startY - lineSpacing, "W/S - Deplasare inainte/inapoi");
    renderText(startX, startY - 2 * lineSpacing, "A/D - Deplasare stanga/dreapta");
    renderText(startX, startY - 3 * lineSpacing, "Q/E - Deplasare sus/jos");
    renderText(startX, startY - 4 * lineSpacing, "I/K - Rotatie sus/jos");
    renderText(startX, startY - 5 * lineSpacing, "J/L - Rotatie stanga/dreapta");
    renderText(startX, startY - 6 * lineSpacing, "U/O - Rotatie in sensul/invers acelor de ceasornic");
    renderText(startX, startY - 7 * lineSpacing, "R - Resetare pozitie camera");
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    
    // Aplicăm transformările camerei
    glRotatef(cameraRotX, 1.0f, 0.0f, 0.0f);
    glRotatef(cameraRotY, 0.0f, 1.0f, 0.0f);
    glRotatef(cameraRotZ, 0.0f, 0.0f, 1.0f);
    glTranslatef(-cameraX, -cameraY, -cameraZ);
    
    // Actualizăm poziția luminii
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    
    // Desenăm soarele
    glPushMatrix();
    glDisable(GL_LIGHTING);
    glTranslatef(lightPosition[0], lightPosition[1], lightPosition[2]);
    glColor3f(1.0f, 1.0f, 0.0f);
    glutSolidSphere(15.0f, 32, 32);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    
    // Desenăm stelele
    glDisable(GL_LIGHTING);
    draw_star(20, 240);
    draw_star(180, 220);
    draw_star(-30, 280);
    draw_star(190, 280);
    draw_star(100, 220);
    draw_star(-230, 250);
    draw_star(-190, 210);
    draw_star(-88, 260);
    draw_star(88, 270);
    draw_star(-170, 280);
    glEnable(GL_LIGHTING);
    
    // Desenăm clădirile cu umbre
    drawBuildingsWithShadows();
    
    // Desenăm drumul
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    drawRoad();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    
    // Desenăm copacii
    drawTree(-25, 1, 50, 20, 2.0, 5.0);
    drawTree(-25, 1, -40, 20, 2.0, 5.0);
    drawTree(-25, 1, 20, 20, 2.0, 5.0);
    drawTree(-25, 1, -10, 20, 2.0, 5.0);
    drawTree(-25, 1, -80, 20, 2.0, 5.0);
    drawTree(-25, 1, -120, 20, 2.0, 5.0);
    drawTree(-25, 1, -160, 20, 2.0, 5.0);
    
    // Desenăm drumurile
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    glColor3f(0.2, 0.2, 0.2);
    glVertex3f(0, 0.01, -200);
    glVertex3f(0, 0.01, 200);
    glVertex3f(18, 0.01, 200);
    glVertex3f(18, 0.01, -200);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);
    glVertex3f(8.5, 0.02, -200);
    glVertex3f(8.5, 0.02, 200);
    glVertex3f(9.5, 0.02, 200);
    glVertex3f(9.5, 0.02, -200);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.2, 0.2, 0.2);
    glVertex3f(44, 0.01, -200);
    glVertex3f(44, 0.01, 160);
    glVertex3f(54, 0.01, 160);
    glVertex3f(54, 0.01, -200);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.2, 0.2, 0.2);
    glVertex3f(54, 0.01, -180);
    glVertex3f(54, 0.01, -160);
    glVertex3f(200, 0.01, -160);
    glVertex3f(200, 0.01, -180);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.2, 0.2, 0.2);
    glVertex3f(18, 0.01, 160);
    glVertex3f(18, 0.01, 170);
    glVertex3f(200, 0.01, 170);
    glVertex3f(200, 0.01, 160);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.2, 0.2, 0.2);
    glVertex3f(-200, 0.01, 90);
    glVertex3f(-200, 0.01, 100);
    glVertex3f(0, 0.01, 100);
    glVertex3f(0, 0.01, 90);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.2, 0.2, 0.2);
    glVertex3f(-90, 0.01, -200);
    glVertex3f(-90, 0.01, 90);
    glVertex3f(-80, 0.01, 90);
    glVertex3f(-80, 0.01, -200);
    glEnd();
    glEnable(GL_LIGHTING);
    
    // Afișăm instrucțiunile
    displayInstructions();
    
    glutSwapBuffers();
    
    // Incrementăm unghiul soarelui
    sunAngle += 0.01f;
    if (sunAngle > 2 * PI) sunAngle -= 2 * PI;
    
    // Actualizăm poziția soarelui
    lightPosition[0] = sunRadius * cos(sunAngle);
    lightPosition[1] = sunHeight;
    lightPosition[2] = sunRadius * sin(sunAngle);
    
    glutPostRedisplay();
}

// Funcție nouă pentru desenarea clădirilor cu umbre
void drawBuildingsWithShadows()
{
    // Mai întâi desenăm clădirile normale
    glEnable(GL_LIGHTING);
    house1();
    
    // Apoi desenăm umbrele
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    
    // Activăm blending pentru transparență
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Matricea de proiecție pentru umbre
    GLfloat shadowMat[16];
    GLfloat groundPlane[] = { 0.0f, 1.0f, 0.0f, 0.0f }; // Planul Y = 0
    
    // Calculăm matricea de umbre
    generateShadowMatrix(shadowMat, groundPlane, lightPosition);
    
    glPushMatrix();
    glMultMatrixf(shadowMat);
    
    // Dezactivăm textura și setăm culoarea la negru complet
    glDisable(GL_TEXTURE_2D);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);  // Negru cu opacitate 50%
    
    // Desenăm umbrele
    house1();
    
    glPopMatrix();
    
    // Restaurăm starea OpenGL
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// Funcție pentru generarea matricei de umbre
void generateShadowMatrix(GLfloat shadowMat[16], GLfloat groundPlane[4], GLfloat lightPos[4])
{
	GLfloat dot = groundPlane[0] * lightPos[0] +
				  groundPlane[1] * lightPos[1] +
				  groundPlane[2] * lightPos[2] +
				  groundPlane[3] * lightPos[3];
	
	shadowMat[0]  = dot - lightPos[0] * groundPlane[0];
	shadowMat[4]  = 0.f - lightPos[0] * groundPlane[1];
	shadowMat[8]  = 0.f - lightPos[0] * groundPlane[2];
	shadowMat[12] = 0.f - lightPos[0] * groundPlane[3];
	
	shadowMat[1]  = 0.f - lightPos[1] * groundPlane[0];
	shadowMat[5]  = dot - lightPos[1] * groundPlane[1];
	shadowMat[9]  = 0.f - lightPos[1] * groundPlane[2];
	shadowMat[13] = 0.f - lightPos[1] * groundPlane[3];
	
	shadowMat[2]  = 0.f - lightPos[2] * groundPlane[0];
	shadowMat[6]  = 0.f - lightPos[2] * groundPlane[1];
	shadowMat[10] = dot - lightPos[2] * groundPlane[2];
	shadowMat[14] = 0.f - lightPos[2] * groundPlane[3];
	
	shadowMat[3]  = 0.f - lightPos[3] * groundPlane[0];
	shadowMat[7]  = 0.f - lightPos[3] * groundPlane[1];
	shadowMat[11] = 0.f - lightPos[3] * groundPlane[2];
	shadowMat[15] = dot - lightPos[3] * groundPlane[3];
}

void keyboard(unsigned char key, int x, int y)
{
    float rotationSpeed = 2.0f;
    
    // Calculăm direcția de deplasare bazată pe rotația camerei
    float radY = cameraRotY * PI / 180.0f;
    float forwardX = sin(radY);
    float forwardZ = -cos(radY);
    float rightX = cos(radY);
    float rightZ = sin(radY);
    
    switch(key) {
        // Deplasare înainte/înapoi
        case 'w':
            cameraX += forwardX * moveSpeed;
            cameraZ += forwardZ * moveSpeed;
            break;
        case 's':
            cameraX -= forwardX * moveSpeed;
            cameraZ -= forwardZ * moveSpeed;
            break;
            
        // Deplasare stânga/dreapta
        case 'a':
            cameraX -= rightX * moveSpeed;
            cameraZ -= rightZ * moveSpeed;
            break;
        case 'd':
            cameraX += rightX * moveSpeed;
            cameraZ += rightZ * moveSpeed;
            break;
            
        // Deplasare sus/jos
        case 'q': cameraY += moveSpeed; break;
        case 'e': cameraY -= moveSpeed; break;
            
        // Rotație sus/jos
        case 'i': cameraRotX += rotationSpeed; break;
        case 'k': cameraRotX -= rotationSpeed; break;
            
        // Rotație stânga/dreapta
        case 'j': cameraRotY -= rotationSpeed; break;
        case 'l': cameraRotY += rotationSpeed; break;
            
        // Rotație în jurul axei Z
        case 'u': cameraRotZ += rotationSpeed; break;
        case 'o': cameraRotZ -= rotationSpeed; break;
            
        // Resetare cameră
        case 'r':
            cameraX = 0.0f;
            cameraY = 5.0f;
            cameraZ = 20.0f;
            cameraRotX = 0.0f;
            cameraRotY = 0.0f;
            cameraRotZ = 0.0f;
            break;
    }
    
    glutPostRedisplay();
}


void resize(int width, int height)
{
    if (height == 0) height = 1;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(80.0, width / height, 1.0, 600.0);
    glTranslatef(0.0, -15.0, -320.0);

    glMatrixMode(GL_MODELVIEW);
}

void loadBuildingTexture() {
    buildingTexture = SOIL_load_OGL_texture(
        "building.jpg",
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
    );
    if (buildingTexture == 0) {
        std::cout << "Eroare la încărcarea texturii clădirii!" << std::endl;
    }

    glBindTexture(GL_TEXTURE_2D, buildingTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

// Implementarea funcției timer
void timer(int value) {
    // Incrementăm unghiul soarelui
    sunAngle += 0.01f;
    if (sunAngle > 2 * PI) {
        sunAngle -= 2 * PI;
    }
    
    // Actualizăm poziția soarelui
    lightPosition[0] = sunRadius * cos(sunAngle);
    lightPosition[1] = sunHeight;
    lightPosition[2] = sunRadius * sin(sunAngle);
    
    glutPostRedisplay();  // Solicităm redesenarea scenei
    glutTimerFunc(16, timer, 0);  // Programăm următorul apel (aproximativ 60 FPS)
}
