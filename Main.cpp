#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>
#include <iostream>
#include <SOIL/SOIL.h>
#include <vector>

#define PI 3.14159265358979323846

using namespace std;

float cc[20][2];
GLuint skyTexture;
GLuint roadTexture;

// Declarații funcții
void init(void);
void display(void);
void keyboard(unsigned char, int, int);
void specialKeys(int, int, int);
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
void timer(int value);

// Declarații pentru funcțiile de umbre
void houseShadow(float x, float y, float z);
void apartShadow(float x, float y, float z);
void standShadow(float x, float y, float z);
void house1Shadow(void);

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
	glutSpecialFunc(specialKeys);

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

// === VARIABILE GLOBALE PENTRU MASINA ===
float carX = 0.0f, carY = 0.1f, carZ = 0.0f; // Pozitia masinii
float carLength = 10.0f, carWidth = 6.0f, carHeight = 4.0f;
float carSpeed = 2.0f;
float carRotation = 0.0f; // Unghiul de rotatie al masinii
float wheelRadius = 1.0f; // Raza rotilor
float wheelWidth = 0.5f;  // Latimea rotilor

// === VARIABILE PENTRU MODUL CAMERA ===
bool followCar = false; // Modul urmarire masina

// === FUNCTIE DE DESENARE ROTA ===
void drawWheel(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(90, 0, 1, 0);
    glColor3f(0.2f, 0.2f, 0.2f); // Culoare neagra pentru roata
    
    // Desenam roata ca un cilindru
    GLUquadricObj *quadric = gluNewQuadric();
    gluCylinder(quadric, wheelRadius, wheelRadius, wheelWidth, 32, 32);
    
    // Desenam centrul rotii
    glColor3f(0.5f, 0.5f, 0.5f);
    gluDisk(quadric, 0, wheelRadius, 32, 32);
    glTranslatef(0, 0, wheelWidth);
    gluDisk(quadric, 0, wheelRadius, 32, 32);
    
    gluDeleteQuadric(quadric);
    glPopMatrix();
}

// === FUNCTIE DE DESENARE MASINA ===
void drawCar() {
    glPushMatrix();
    glTranslatef(carX, carY, carZ);
    glRotatef(carRotation, 0, 1, 0); // Rotim masina
    
    // Caroserie principala
    glColor3f(1.0f, 0.0f, 0.0f); // Rosu
    glBegin(GL_QUADS);
    // Baza
    glVertex3f(-carLength/2, 0, -carWidth/2);
    glVertex3f(carLength/2, 0, -carWidth/2);
    glVertex3f(carLength/2, 0, carWidth/2);
    glVertex3f(-carLength/2, 0, carWidth/2);
    // Top
    glVertex3f(-carLength/2, carHeight, -carWidth/2);
    glVertex3f(carLength/2, carHeight, -carWidth/2);
    glVertex3f(carLength/2, carHeight, carWidth/2);
    glVertex3f(-carLength/2, carHeight, carWidth/2);
    // Fata
    glVertex3f(carLength/2, 0, -carWidth/2);
    glVertex3f(carLength/2, carHeight, -carWidth/2);
    glVertex3f(carLength/2, carHeight, carWidth/2);
    glVertex3f(carLength/2, 0, carWidth/2);
    // Spate
    glVertex3f(-carLength/2, 0, -carWidth/2);
    glVertex3f(-carLength/2, carHeight, -carWidth/2);
    glVertex3f(-carLength/2, carHeight, carWidth/2);
    glVertex3f(-carLength/2, 0, carWidth/2);
    // Stanga
    glVertex3f(-carLength/2, 0, -carWidth/2);
    glVertex3f(carLength/2, 0, -carWidth/2);
    glVertex3f(carLength/2, carHeight, -carWidth/2);
    glVertex3f(-carLength/2, carHeight, -carWidth/2);
    // Dreapta
    glVertex3f(-carLength/2, 0, carWidth/2);
    glVertex3f(carLength/2, 0, carWidth/2);
    glVertex3f(carLength/2, carHeight, carWidth/2);
    glVertex3f(-carLength/2, carHeight, carWidth/2);
    glEnd();

    // Geamuri
    glColor3f(0.7f, 0.9f, 1.0f); // Albastru deschis
    glBegin(GL_QUADS);
    // Geam fata
    glVertex3f(carLength/2-0.1, carHeight*0.6, -carWidth/3);
    glVertex3f(carLength/2-0.1, carHeight*0.9, -carWidth/3);
    glVertex3f(carLength/2-0.1, carHeight*0.9, carWidth/3);
    glVertex3f(carLength/2-0.1, carHeight*0.6, carWidth/3);
    // Geam spate
    glVertex3f(-carLength/2+0.1, carHeight*0.6, -carWidth/3);
    glVertex3f(-carLength/2+0.1, carHeight*0.9, -carWidth/3);
    glVertex3f(-carLength/2+0.1, carHeight*0.9, carWidth/3);
    glVertex3f(-carLength/2+0.1, carHeight*0.6, carWidth/3);
    glEnd();

    // Faruri
    glColor3f(1.0f, 1.0f, 0.8f); // Galben deschis
    glBegin(GL_QUADS);
    // Far stanga
    glVertex3f(carLength/2, carHeight*0.3, -carWidth/2);
    glVertex3f(carLength/2, carHeight*0.5, -carWidth/2);
    glVertex3f(carLength/2, carHeight*0.5, -carWidth/3);
    glVertex3f(carLength/2, carHeight*0.3, -carWidth/3);
    // Far dreapta
    glVertex3f(carLength/2, carHeight*0.3, carWidth/3);
    glVertex3f(carLength/2, carHeight*0.5, carWidth/3);
    glVertex3f(carLength/2, carHeight*0.5, carWidth/2);
    glVertex3f(carLength/2, carHeight*0.3, carWidth/2);
    glEnd();

    // Roti
    drawWheel(-carLength/3, wheelRadius, -carWidth/2-wheelWidth/2); // Rota stanga fata
    drawWheel(carLength/3, wheelRadius, -carWidth/2-wheelWidth/2);  // Rota dreapta fata
    drawWheel(-carLength/3, wheelRadius, carWidth/2+wheelWidth/2);  // Rota stanga spate
    drawWheel(carLength/3, wheelRadius, carWidth/2+wheelWidth/2);   // Rota dreapta spate

    glPopMatrix();
}

// === STRUCTURA PENTRU BOUNDING BOX CLADIRE ===
struct Box {
    float xMin, xMax, yMin, yMax, zMin, zMax;
};

// === VECTOR CU BOUNDING BOX-URI CLADIRI ===
std::vector<Box> buildingBoxes;

// === FUNCTIE DE ADAUGARE CLADIRI IN VECTOR ===
void addBuildingBox(float x, float y, float z, float l, float h, float w) {
    Box b;
    b.xMin = x;
    b.xMax = x + l;
    b.yMin = y;
    b.yMax = y + h;
    b.zMin = z;
    b.zMax = z + w;
    buildingBoxes.push_back(b);
}

// === FUNCTIE DE INITIALIZARE BOUNDING BOX-URI CLADIRI ===
void initBuildingBoxes() {
    buildingBoxes.clear();
    // Case (pozitii si dimensiuni din house1)
    addBuildingBox(120, 0.1, 50, 45, 100+15, 45); // +acoperis
    addBuildingBox(120, 0.1, 90, 45, 100+15, 45);
    addBuildingBox(160, 0.1, 90, 45, 100+15, 45);
    addBuildingBox(80, 0.1, 90, 45, 100+15, 45);
    addBuildingBox(160, 0.1, 50, 45, 100+15, 45);
    addBuildingBox(80, 0.1, 50, 45, 100+15, 45);
    addBuildingBox(-130, 0.1, 120, 45, 100+15, 45);
    addBuildingBox(-130, 0.1, 160, 45, 100+15, 45);
    addBuildingBox(-90, 0.1, 120, 45, 100+15, 45);
    addBuildingBox(-60, 0.1, 120, 45, 100+15, 45);
    addBuildingBox(-90, 0.1, 160, 45, 100+15, 45);
    addBuildingBox(-60, 0.1, 160, 45, 100+15, 45);
    // Apartamente
    addBuildingBox(150, 0, -150, 45, 100, 45);
    addBuildingBox(80, 0, -90, 45, 100, 45);
    addBuildingBox(150, 0, -90, 45, 100, 45);
    addBuildingBox(150, 0, -30, 45, 100, 45);
    addBuildingBox(-150, 0, -30, 45, 100, 45);
    addBuildingBox(-150, 0, -110, 45, 100, 45);
    addBuildingBox(-150, 0, -180, 45, 100, 45);
    // Standuri
    addBuildingBox(-10, 6, 130, 5, 6, 8);
    addBuildingBox(30, 6, 130, 5, 6, 8);
}

// === FUNCTIE DE VERIFICARE COLIZIUNE MASINA-CLADIRE ===
bool checkCarCollision(float newX, float newY, float newZ) {
    float xMin = newX - carLength/2;
    float xMax = newX + carLength/2;
    float yMin = newY;
    float yMax = newY + carHeight;
    float zMin = newZ - carWidth/2;
    float zMax = newZ + carWidth/2;
    for (const auto& b : buildingBoxes) {
        if (xMax > b.xMin && xMin < b.xMax &&
            yMax > b.yMin && yMin < b.yMax &&
            zMax > b.zMin && zMin < b.zMax) {
            return true;
        }
    }
    return false;
}

void init(void)
{
	// Setăm culoarea fundalului la albastru deschis
	glClearColor(0.529f, 0.808f, 0.922f, 1.0f); // Culoare albastru cer deschis
	
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
	GLfloat globalAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };  // Creștem lumina ambientală
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
	   
    // Încarcă toate texturile
    loadSkyTexture();
    loadRoadTexture();
    loadTreeTexture();
    loadBuildingTexture();

    // Activăm texturarea
    glEnable(GL_TEXTURE_2D);

    initBuildingBoxes();
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
	glPushMatrix();
	glDisable(GL_LIGHTING);
	
	// Activăm texturarea și legăm textura cerului
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skyTexture);
	
	// Deplasăm cerul în spatele scenei
	glTranslatef(0, 0, -200);
	
	// Desenăm un dreptunghi mare pentru cer
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-500, -200, 0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(500, -200, 0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(500, 500, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-500, 500, 0);
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopMatrix();
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
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, buildingTexture);
    
    // Reset color to white to prevent any tinting of the texture
    glColor3f(1.0, 1.0, 1.0);

    // Fața frontală
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z + 0.5);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 45, y, z + 0.5);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 45, y + 100, z + 0.5);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + 100, z + 0.5);
    glEnd();

    // Perete spate
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z + 45 - 0.5);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 45, y, z + 45 - 0.5);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 45, y + 100, z + 45 - 0.5);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + 100, z + 45 - 0.5);
    glEnd();

    // Perete lateral stânga
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + 0.5, y, z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 0.5, y, z + 45);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 0.5, y + 100, z + 45);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + 0.5, y + 100, z);
    glEnd();

    // Perete lateral dreapta
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + 45 - 0.5, y, z - 0.5);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 45 - 0.5, y, z + 45 - 0.5);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 45 - 0.5, y + 100, z + 45 - 0.5);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + 45 - 0.5, y + 100, z - 0.5);
    glEnd();

    // Acoperiș
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y + 100, z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 45, y + 100, z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 45, y + 100, z + 45);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y + 100, z + 45);
    glEnd();

    // Bază
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z - 0.5);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 45, y, z - 0.5);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 45, y, z + 45 - 0.5);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y, z + 45 - 0.5);
    glEnd();

    glDisable(GL_TEXTURE_2D);
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

    // Adăugăm acoperișul
    float roofHeight = 15.0f; // Înălțimea acoperișului

    // Partea din față a acoperișului
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y + 100, z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 45, y + 100, z);
    glTexCoord2f(0.5f, 1.0f); glVertex3f(x + 22.5, y + 100 + roofHeight, z + 22.5);
    glEnd();

    // Partea din spate a acoperișului
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y + 100, z + 45);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 45, y + 100, z + 45);
    glTexCoord2f(0.5f, 1.0f); glVertex3f(x + 22.5, y + 100 + roofHeight, z + 22.5);
    glEnd();

    // Partea stângă a acoperișului
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y + 100, z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y + 100, z + 45);
    glTexCoord2f(0.5f, 1.0f); glVertex3f(x + 22.5, y + 100 + roofHeight, z + 22.5);
    glEnd();

    // Partea dreaptă a acoperișului
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + 45, y + 100, z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 45, y + 100, z + 45);
    glTexCoord2f(0.5f, 1.0f); glVertex3f(x + 22.5, y + 100 + roofHeight, z + 22.5);
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
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, buildingTexture);
    glColor3f(1.0, 1.0, 1.0);  // Reset la alb pentru a evita tenta de culoare

    // Fața frontală
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y - h1, z + d1);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + h, y - h1, z + d1);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + h, y, z);
    glEnd();

    // Fața laterală
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y, z);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + h, y, z);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + h, y - h1, z - d1);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y - h1, z - d1);
    glEnd();

    // Stâlpi de susținere
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y - h1, z + d1);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y - h1 - 2, z + d1);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 1, y - h1 - 2, z + d1);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 1, y - h1, z + d1);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + h, y - h1, z + d1);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + h, y - h1 - 2, z + d1);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + h - 1, y - h1 - 2, z + d1);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + h - 1, y - h1, z + d1);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y - h1, z - d1);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y - h1 - 2, z - d1);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + 1, y - h1 - 2, z - d1);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + 1, y - h1, z - d1);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x + h, y - h1, z - d1);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x + h, y - h1 - 2, z - d1);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + h - 1, y - h1 - 2, z - d1);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + h - 1, y - h1, z - d1);
    glEnd();

    glDisable(GL_TEXTURE_2D);
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
    
    if (followCar) {
        // Camera urmareste masina de sus
        float height = 30.0f;   // Inaltimea camerei (mai sus)
        
        // Pozitionam camera deasupra masinii
        cameraX = carX;
        cameraY = carY + height;
        cameraZ = carZ;
        
        // Privim in jos spre masina
        gluLookAt(cameraX, cameraY, cameraZ,
                 carX, carY, carZ,  // Privim direct in jos
                 0, 0, -1);         // Vector up este spre -Z pentru a mentine orientarea corecta
    } else {
        // Camera normala
        glRotatef(cameraRotX, 1.0f, 0.0f, 0.0f);
        glRotatef(cameraRotY, 0.0f, 1.0f, 0.0f);
        glRotatef(cameraRotZ, 0.0f, 0.0f, 1.0f);
        glTranslatef(-cameraX, -cameraY, -cameraZ);
    }
    
    // Desenăm cerul primul
    drawSky();
    
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
    
    drawCar();
    
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

// Funcție pentru desenarea obiectelor fără textură
void drawWithoutTexture(void (*drawFunction)()) {
    // Dezactivăm și blocăm textura
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Forțăm culoarea neagră
    glColor3f(0.0f, 0.0f, 0.0f);
    
    // Apelăm funcția de desenare
    drawFunction();
}

void drawBlackShadow(void (*drawFunction)()) {
    // Salvăm toate atributele OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    // Dezactivăm toate efectele care ar putea afecta culoarea umbrei
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    
    // Forțăm culoarea neagră
    glColor3f(0.0f, 0.0f, 0.0f);
    
    // Aplicăm matricea de umbre
    GLfloat shadowMat[16];
    GLfloat groundPlane[] = { 0.0f, 1.0f, 0.0f, 0.01f };
    generateShadowMatrix(shadowMat, groundPlane, lightPosition);
    
    glPushMatrix();
    glMultMatrixf(shadowMat);
    
    // Desenăm obiectul folosind wrapper-ul care forțează dezactivarea texturii
    drawWithoutTexture(drawFunction);
    
    glPopMatrix();
    
    // Restaurăm starea OpenGL
    glPopAttrib();
}

void drawBuildingsWithShadows()
{
    // Salvăm starea curentă OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    
    // Mai întâi desenăm clădirile normale
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glColor3f(1.0, 1.0, 1.0);
    house1();
    
    // Apoi desenăm umbrele
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND); // Dezactivăm blending pentru umbre opace
    
    // Aplicăm matricea de umbre
    GLfloat shadowMat[16];
    GLfloat groundPlane[] = { 0.0f, 1.0f, 0.0f, 0.01f };
    generateShadowMatrix(shadowMat, groundPlane, lightPosition);
    
    glPushMatrix();
    glMultMatrixf(shadowMat);
    
    // Setăm culoarea neagră opacă pentru umbre
    glColor3f(0.0f, 0.0f, 0.0f);
    
    // Desenăm umbrele folosind funcțiile speciale pentru umbre
    house1Shadow();
    
    glPopMatrix();
    
    // Restaurăm starea OpenGL folosind stiva de atribute
    glPopAttrib();
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

// === FUNCTIE PENTRU TASTE SPECIALE (SAGETI) ===
void specialKeys(int key, int x, int y) {
    switch(key) {
        // Control masina cu sageti
        case GLUT_KEY_UP: {
            float rad = carRotation * PI / 180.0f;
            float nx = carX + sin(rad) * carSpeed;
            float nz = carZ - cos(rad) * carSpeed;
            if (!checkCarCollision(nx, carY, nz)) {
                carX = nx;
                carZ = nz;
            }
            break;
        }
        case GLUT_KEY_DOWN: {
            float rad = carRotation * PI / 180.0f;
            float nx = carX - sin(rad) * carSpeed;
            float nz = carZ + cos(rad) * carSpeed;
            if (!checkCarCollision(nx, carY, nz)) {
                carX = nx;
                carZ = nz;
            }
            break;
        }
        case GLUT_KEY_LEFT: {
            carRotation += 5.0f; // Rotire stanga
            break;
        }
        case GLUT_KEY_RIGHT: {
            carRotation -= 5.0f; // Rotire dreapta
            break;
        }
    }
    glutPostRedisplay();
}

// === MODIFICARE FUNCTIE KEYBOARD ===
void keyboard(unsigned char key, int x, int y) {
    float rotationSpeed = 2.0f;
    
    // Calculăm direcția de deplasare bazată pe rotația camerei
    float radY = cameraRotY * PI / 180.0f;
    float forwardX = sin(radY);
    float forwardZ = -cos(radY);
    float rightX = cos(radY);
    float rightZ = sin(radY);
    
    switch(key) {
        // Control camera cu WASD
        case 'w':
            cameraX += forwardX * moveSpeed;
            cameraZ += forwardZ * moveSpeed;
            break;
        case 's':
            cameraX -= forwardX * moveSpeed;
            cameraZ -= forwardZ * moveSpeed;
            break;
        case 'a':
            cameraX -= rightX * moveSpeed;
            cameraZ -= rightZ * moveSpeed;
            break;
        case 'd':
            cameraX += rightX * moveSpeed;
            cameraZ += rightZ * moveSpeed;
            break;
            
        // Resetare masina
        case 't':
            carX = 0.0f; carY = 0.1f; carZ = 0.0f;
            carRotation = 0.0f;
            break;
            
        // Moduri camera
        case 'm':
            followCar = true;
            break;
        case 'n':
            followCar = false;
            break;
            
        // Control camera (sus/jos)
        case 'q': cameraY += moveSpeed; break;
        case 'e': cameraY -= moveSpeed; break;
            
        // Rotație camera
        case 'i': cameraRotX += rotationSpeed; break;
        case 'k': cameraRotX -= rotationSpeed; break;
        case 'j': cameraRotY -= rotationSpeed; break;
        case 'l': cameraRotY += rotationSpeed; break;
        case 'u': cameraRotZ += rotationSpeed; break;
        case 'o': cameraRotZ -= rotationSpeed; break;
            
        // Resetare camera
        case 'r':
            cameraX = 0.0f;
            cameraY = 5.0f;
            cameraZ = 20.0f;
            cameraRotX = 0.0f;
            cameraRotY = 0.0f;
            cameraRotZ = 0.0f;
            followCar = false;
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

// Funcții pentru desenarea umbrelor fără texturi
void houseShadow(float x, float y, float z)
{
    // Fața frontală
    glBegin(GL_QUADS);
    glVertex3f(x, y, z);
    glVertex3f(x + 45, y, z);
    glVertex3f(x + 45, y + 100, z);
    glVertex3f(x, y + 100, z);
    glEnd();

    // Fața din spate
    glBegin(GL_QUADS);
    glVertex3f(x, y, z + 45);
    glVertex3f(x + 45, y, z + 45);
    glVertex3f(x + 45, y + 100, z + 45);
    glVertex3f(x, y + 100, z + 45);
    glEnd();

    // Fațele laterale
    glBegin(GL_QUADS);
    glVertex3f(x + 45, y, z);
    glVertex3f(x + 45, y, z + 45);
    glVertex3f(x + 45, y + 100, z + 45);
    glVertex3f(x + 45, y + 100, z);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(x, y, z);
    glVertex3f(x, y, z + 45);
    glVertex3f(x, y + 100, z + 45);
    glVertex3f(x, y + 100, z);
    glEnd();

    // Acoperișul
    float roofHeight = 15.0f;
    glBegin(GL_TRIANGLES);
    glVertex3f(x, y + 100, z);
    glVertex3f(x + 45, y + 100, z);
    glVertex3f(x + 22.5, y + 100 + roofHeight, z + 22.5);
    
    glVertex3f(x, y + 100, z + 45);
    glVertex3f(x + 45, y + 100, z + 45);
    glVertex3f(x + 22.5, y + 100 + roofHeight, z + 22.5);
    
    glVertex3f(x, y + 100, z);
    glVertex3f(x, y + 100, z + 45);
    glVertex3f(x + 22.5, y + 100 + roofHeight, z + 22.5);
    
    glVertex3f(x + 45, y + 100, z);
    glVertex3f(x + 45, y + 100, z + 45);
    glVertex3f(x + 22.5, y + 100 + roofHeight, z + 22.5);
    glEnd();
}

void apartShadow(float x, float y, float z)
{
    // Fața frontală
    glBegin(GL_QUADS);
    glVertex3f(x, y, z + 0.5);
    glVertex3f(x + 45, y, z + 0.5);
    glVertex3f(x + 45, y + 100, z + 0.5);
    glVertex3f(x, y + 100, z + 0.5);
    glEnd();

    // Perete spate
    glBegin(GL_QUADS);
    glVertex3f(x, y, z + 45 - 0.5);
    glVertex3f(x + 45, y, z + 45 - 0.5);
    glVertex3f(x + 45, y + 100, z + 45 - 0.5);
    glVertex3f(x, y + 100, z + 45 - 0.5);
    glEnd();

    // Pereți laterali
    glBegin(GL_QUADS);
    glVertex3f(x + 0.5, y, z);
    glVertex3f(x + 0.5, y, z + 45);
    glVertex3f(x + 0.5, y + 100, z + 45);
    glVertex3f(x + 0.5, y + 100, z);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(x + 45 - 0.5, y, z - 0.5);
    glVertex3f(x + 45 - 0.5, y, z + 45 - 0.5);
    glVertex3f(x + 45 - 0.5, y + 100, z + 45 - 0.5);
    glVertex3f(x + 45 - 0.5, y + 100, z - 0.5);
    glEnd();
}

void standShadow(float x, float y, float z)
{
    glBegin(GL_QUADS);
    glVertex3f(x, y, z);
    glVertex3f(x, y - h1, z + d1);
    glVertex3f(x + h, y - h1, z + d1);
    glVertex3f(x + h, y, z);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(x, y, z);
    glVertex3f(x + h, y, z);
    glVertex3f(x + h, y - h1, z - d1);
    glVertex3f(x, y - h1, z - d1);
    glEnd();

    // Stâlpi
    glBegin(GL_QUADS);
    glVertex3f(x, y - h1, z + d1);
    glVertex3f(x, y - h1 - 2, z + d1);
    glVertex3f(x + 1, y - h1 - 2, z + d1);
    glVertex3f(x + 1, y - h1, z + d1);
    glEnd();

    glBegin(GL_QUADS);
    glVertex3f(x + h, y - h1, z + d1);
    glVertex3f(x + h, y - h1 - 2, z + d1);
    glVertex3f(x + h - 1, y - h1 - 2, z + d1);
    glVertex3f(x + h - 1, y - h1, z + d1);
    glEnd();
}

void house1Shadow()
{
    houseShadow(120, 0.1, 50);
    houseShadow(120, 0.1, 90);
    houseShadow(160, 0.1, 90);
    houseShadow(80, 0.1, 90);
    houseShadow(160, 0.1, 50);
    houseShadow(80, 0.1, 50);
    houseShadow(-130, 0.1, 120);
    houseShadow(-130, 0.1, 160);
    houseShadow(-90, 0.1, 120);
    houseShadow(-60, 0.1, 120);
    houseShadow(-90, 0.1, 160);
    houseShadow(-60, 0.1, 160);

    standShadow(-10, 6, 130);
    standShadow(30, 6, 130);

    apartShadow(150, 0, -150);
    apartShadow(80, 0, -90);
    apartShadow(150, 0, -90);
    apartShadow(150, 0, -30);
    apartShadow(-150, 0, -30);
    apartShadow(-150, 0, -110);
    apartShadow(-150, 0, -180);
}

// === STRUCTURI SI VARIABILE PENTRU PIETONI ===
struct Pedestrian {
    float x, y, z;
    float speed;
    float direction; // unghi in grade
    float timeToChangeDirection;
    float size;
};
std::vector<Pedestrian> pedestrians;
const int NUM_PEDESTRIANS = 10;
float pedestrianSize = 1.0f;
void initPedestrians();
void drawPedestrian(const Pedestrian& p);
void updatePedestrians(float deltaTime);

// === STRUCTURI SI VARIABILE PENTRU STALPI DE ILUMINAT ===
struct StreetLight {
    float x, y, z;
    float height;
    bool isOn;
};
std::vector<StreetLight> streetLights;
const int NUM_STREET_LIGHTS = 20;
void initStreetLights();
void drawStreetLight(const StreetLight& sl);

void initStreetLights() {
    streetLights.clear();
    // Stâlpi de-a lungul drumurilor principale
    for (int i = -200; i <= 200; i += 50) {
        StreetLight sl;
        sl.x = i;
        sl.y = 0.0f;
        sl.z = 0.0f;
        sl.height = 15.0f;
        sl.isOn = true;
        streetLights.push_back(sl);
        sl.x = 0.0f;
        sl.z = i;
        streetLights.push_back(sl);
    }
    // Stâlpi suplimentari la pozițiile copacilor
    streetLights.push_back({-25.0f, 0.0f, 50.0f, 15.0f, true});
    streetLights.push_back({-25.0f, 0.0f, -40.0f, 15.0f, true});
    streetLights.push_back({-25.0f, 0.0f, 20.0f, 15.0f, true});
    streetLights.push_back({-25.0f, 0.0f, -10.0f, 15.0f, true});
    streetLights.push_back({-25.0f, 0.0f, -80.0f, 15.0f, true});
    streetLights.push_back({-25.0f, 0.0f, -120.0f, 15.0f, true});
    streetLights.push_back({-25.0f, 0.0f, -160.0f, 15.0f, true});
}
