#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>
#include <iostream>
#include <SOIL/SOIL.h>


using namespace std;

float cc[20][2];
GLuint skyTexture;
GLuint roadTexture;
// Textura pentru trunchiul copacului





void init(void);
void display(void);
void keyboard(unsigned char, int, int);
void resize(int, int);
void draw_star(float, float);
void house(float, float, float);
void apart(float, float, float);
void circle1(float);
void stand(float, float, float);
float  h = 5, h1 = 6, d1 = 4, g = 1, g1 = 2;


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

	glutMainLoop();
	return 0;
}
GLuint buildingTexture;

// În funcția de inițializare a texturii:
void loadSkyTexture() {
	skyTexture = SOIL_load_OGL_texture("path_to_sky_texture.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	if (skyTexture == 0) {
		std::cout << "Eroare la încărcarea texturii cerului!" << std::endl;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void init(void)
{
	glEnable(GL_DEPTH_TEST); // Activează testul de adâncime
	glMatrixMode(GL_MODELVIEW); // Activează modul de vizualizare a obiectelor

	// Încarcă textura cerului
	glGenTextures(1, &skyTexture);
	glBindTexture(GL_TEXTURE_2D, skyTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	   
	int width, height;
	unsigned char* image = SOIL_load_image("sky.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	if (image == NULL) {
		std::cout << "Eroare la încărcarea imaginii cerului!" << std::endl;
		return;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);

	// Încarcă textura pentru clădiri
	glGenTextures(1, &buildingTexture);
	glBindTexture(GL_TEXTURE_2D, buildingTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	image = SOIL_load_image("building.jpg", &width, &height, 0, SOIL_LOAD_RGB);
	if (image == NULL) {
		std::cout << "Eroare la încărcarea imaginii clădirii!" << std::endl;
		return;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);

	// Încarcă textura pentru drum
	loadRoadTexture();
	loadTreeTexture();
  // Încărcăm textura trunchiului copacului
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

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Aplicarea texturii cerului
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, skyTexture);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-200.0, 200.0, -200.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(200.0, 200.0, -200.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(200.0, 200.0, 200.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-200.0, 200.0, 200.0);
	glEnd();
	glPopMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/* draw the floor */
	glBegin(GL_QUADS);
	/*glColor3f(0.8, 0.6, 0.4);
	glVertex3f(-200.0, 0.0, -200.0);
	glColor3f(0.8, 0.6, 0.4);
	glVertex3f(-200.0, 0.0, 200.0);
	glColor3f(0.8, 0.6, 0.4);
	glVertex3f(200.0, 0.0, 200.0);
	glColor3f(0.8, 0.6, 0.4);
	glVertex3f(200.0, 0.0, -200.0);*/
	glEnd();
	glFlush();
	// Adaugă un copac
 // Poziție (50, 0, 0), înălțime trunchi 10, rază trunchi 2, rază frunze 5

	// Desenează cerul texturat
	drawSky();
	drawRoad(); // Desenează drumul
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



	// Copac 1 (înlocuiește cubul + sfera)
	drawTree(-25, 1, 50, 20, 2.0, 5.0); // x, y, z, înălțime_trunchi, raza_trunchi, raza_frunze

	// Copac 2
	drawTree(-25, 1, -40, 20, 2.0, 5.0);

	// Copac 3
	drawTree(-25, 1, 20, 20, 2.0, 5.0);

	// Copac 4
	drawTree(-25, 1, -10, 20, 2.0, 5.0);

	// Copac 5
	drawTree(-25, 1, -80, 20, 2.0, 5.0);

	// Copac 6
	drawTree(-25, 1, -120, 20, 2.0, 5.0);

	// Copac 7
	drawTree(-25, 1, -160, 20, 2.0, 5.0);

	glColor3f(0.3, 0.015, 0.13);
	glPushMatrix();
	glTranslatef(-15, 1, 50);
	glutSolidCube(7);
	glPopMatrix();

	glColor3f(0.3, 0.015, 0.13);
	glPushMatrix();
	glTranslatef(-15, 1, -40);
	glutSolidCube(7);
	glPopMatrix();

	glColor3f(0.3, 0.015, 0.13);
	glPushMatrix();
	glTranslatef(-15, 1, 20);
	glutSolidCube(7);
	glPopMatrix();

	glColor3f(0.3, 0.015, 0.13);
	glPushMatrix();
	glTranslatef(-15, 1, -10);
	glutSolidCube(7);
	glPopMatrix();

	glColor3f(0.3, 0.015, 0.13);
	glPushMatrix();
	glTranslatef(-15, 1, -80);
	glutSolidCube(7);
	glPopMatrix();

	glColor3f(0.3, 0.015, 0.13);
	glPushMatrix();
	glTranslatef(-15, 1, -120);
	glutSolidCube(7);
	glPopMatrix();

	glColor3f(0.3, 0.015, 0.13);
	glPushMatrix();
	glTranslatef(-15, 1, -160);
	glutSolidCube(7);
	glPopMatrix();




	glColor3f(0.015, 0.3, 0.13);
	glPushMatrix();
	glTranslatef(-15, 24, 50);
	glScalef(1.1, 2.3, 0.8);
	glutSolidSphere(10, 15, 6);
	glFlush();
	glPopMatrix();

	glColor3f(0.015, 0.3, 0.13);
	glPushMatrix();
	glTranslatef(-15, 24, -40);
	glScalef(1.1, 2.3, 0.8);
	glutSolidSphere(10, 15, 4);
	glFlush();
	glPopMatrix();

	glColor3f(0.015, 0.3, 0.13);
	glPushMatrix();
	glTranslatef(-15, 24, 20);
	glScalef(1.1, 2.3, 0.8);
	glutSolidSphere(10, 15, 4);
	glFlush();
	glPopMatrix();

	glColor3f(0.015, 0.3, 0.13);
	glPushMatrix();
	glTranslatef(-15, 24, -10);
	glScalef(1.1, 2.3, 0.8);
	glutSolidSphere(10, 15, 4);
	glFlush();
	glPopMatrix();

	glColor3f(0.015, 0.3, 0.13);
	glPushMatrix();
	glTranslatef(-15, 24, -80);
	glScalef(1.1, 2.3, 0.8);
	glutSolidSphere(10, 15, 4);
	glFlush();
	glPopMatrix();

	glColor3f(0.015, 0.3, 0.13);
	glPushMatrix();
	glTranslatef(-15, 24, -120);
	glScalef(1.1, 2.3, 0.8);
	glutSolidSphere(10, 15, 4);
	glFlush();
	glPopMatrix();

	glColor3f(0.015, 0.3, 0.13);
	glPushMatrix();
	glTranslatef(-15, 24, -160);
	glScalef(1.1, 2.3, 0.8);
	glutSolidSphere(10, 15, 4);
	glFlush();
	glPopMatrix();


	glBegin(GL_QUADS);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(0, 0.01, -200); /* road */
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(0, 0.01, 200);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(18, 0.01, 200);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(18, 0.01, -200);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0, 1.0, 1.0);
	glVertex3f(8.5, 0.02, -200); /* road */
	glColor3f(1.0, 1.0, 1.0);
	glVertex3f(8.5, 0.02, 200);
	glColor3f(1.0, 1.0, 1.0);
	glVertex3f(9.5, 0.02, 200);
	glColor3f(1.0, 1.0, 1.0);
	glVertex3f(9.5, 0.02, -200);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(44, 0.01, -200); /* road */
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(44, 0.01, 160);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(54, 0.01, 160);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(54, 0.01, -200);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(54, 0.01, -180); /* road */
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(54, 0.01, -160);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(200, 0.01, -160);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(200, 0.01, -180);
	glEnd();


	glBegin(GL_QUADS);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(18, 0.01, 160); /* road */
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(18, 0.01, 170);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(200, 0.01, 170);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(200, 0.01, 160);
	glEnd();


	glBegin(GL_QUADS);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(-200, 0.01, 90); /* road */
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(-200, 0.01, 100);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(0, 0.01, 100);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(0, 0.01, 90);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(-90, 0.01, -200); /* road */
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(-90, 0.01, 90);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(-80, 0.01, 90);
	glColor3f(0.2, 0.2, 0.2);
	glVertex3f(-80, 0.01, -200);
	glEnd();
	house1();
	glFlush();
	glutSwapBuffers();


}


void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
	case 'A':
		glTranslatef(5.0, 0.0, 0.0);
		break;
	case 'd':
	case 'D':
		glTranslatef(-5.0, 0.0, 0.0);
		break;
	case 'w':
	case 'W':
		glTranslatef(0.0, 0.0, 5.0);
		break;
	case 's':
	case 'S':
		glTranslatef(0.0, 0.0, -5.0);
		break;
	case 'q':
	case 'Q':
		glRotatef(-2, 1.0, 0.0, 0.0);

	case 'e':
	case 'E':
		glRotatef(2, 0.0, 1.0, 0.0);

	}
	display();
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
