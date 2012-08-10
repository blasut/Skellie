
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <math.h>       //Include math.h to get access to sin() and cos()
#include <stdlib.h>
#include <iostream>

using namespace std;

struct Vector3
{

    float x;
    float y;
    float z;

};
struct BoundingBox
{

    Vector3 max;
    Vector3 min;

};

/******************Prototypes*********************/
void initScene();               //Initialiazing
void drawSnowMan();             //Drawing of 1 snowman
GLuint createDL();              // Creation if displaylist that calls drawSnowman() and is initialized in initScene
void renderScene(void);         // Call-back function for drawing the scene
void changeSize(int w, int h);  // Calculating resizing - standard

void inputSpecialKey(int key, int x, int y); //Reading Special key keypresses
void processNormalKeys(unsigned char key, int x, int y); //reading normal keypresses
void orientMe(float ang);       //Funtion for calculating now camera look-at position when turning
void moveMeFlat(int direction); // Calculating new cameraposition when walking forward/backward

void processMousePassiveMotion(int mouse_x, int mouse_y);   //Callback function for handling mouse movement

bool checkBounds(float x, float y, float z);
void drawbox(BoundingBox b);

bool Collision(BoundingBox b, GLfloat x, GLfloat y, GLfloat z);
bool CollisionTest(float x, float y, float z);


/*******************Globals***********************/
static float angle=0.0,ratio;           //angle for calculating right and left turns, ratio for use in changeSize()
float dist = 5.0f;
static float x=0.0f,y=1.25f,z=dist;     //Camera positions
static float cx=0.0f,cy=0.0f,cz=2.0f;     //Character positions
static float lx=0.0f,ly=0.0f,lz=-1.0f;  //Camera look-at point
static GLint snowman_display_list;      //The displaylist used for drawing snowmen

/* NYA */
float lastx=320, lasty=280;             //Center point at which mousemovement will start from
static float y_angle = 0.0f;            //angle for calculating camera lookat point in y-axis

BoundingBox boxes[36];

/********************Main*************************/
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(640,360);
	glutCreateWindow("I love candy, dumdumdumdmudmud.");

	initScene();

	glutSpecialFunc(inputSpecialKey);
    glutKeyboardFunc(processNormalKeys);
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);  //Callback for function to be called when program is idle

	glutReshapeFunc(changeSize);

	glutPassiveMotionFunc(processMousePassiveMotion); //Callback funktion f�r mouse

	glutMainLoop();

	return(0);
}

/******************Functions**********************/
//Check for out of bounds
bool checkBounds(float x, float y, float z){

    if(x < 50.0-0.5 && x > -50.0+0.5 && z < 50.0-0.5 && z > -50.0+0.5){
        return false;
    }else{
        return true;
    }
}

// Collision check
bool Collision(BoundingBox b, GLfloat x, GLfloat y, GLfloat z)
{

    return x <= b.max.x && x >= b.min.x &&

        y <= b.max.y && y >= b.min.y &&
        z <= b.max.z && z >= b.min.z ;

}

//Function to call when checking after movement
bool CollisionTest(float x, float y, float z)
{

    bool rez = false;
    for (int i=0; i<36;i++)

        rez = rez || Collision(boxes[i],x,y,z);

    return rez;

}

void drawbox(BoundingBox b)
{

    glColor3f(1,1,1);
    glBegin (GL_LINE_LOOP);
    glVertex3f(b.max.x,b.max.y,b.min.z); // 0
    glVertex3f(b.min.x,b.max.y,b.min.z); // 1
    glVertex3f(b.min.x,b.min.y,b.min.z); // 2
    glVertex3f(b.max.x,b.min.y,b.min.z); // 3
    glEnd();

    glBegin (GL_LINE_LOOP);
    glVertex3f(b.max.x,b.min.y,b.max.z); // 4
    glVertex3f(b.max.x,b.max.y,b.max.z); // 5
    glVertex3f(b.min.x,b.max.y,b.max.z); // 6
    glVertex3f(b.min.x,b.min.y,b.max.z); // 7
    glEnd();

    glBegin (GL_LINE_LOOP);
    glVertex3f(b.max.x,b.max.y,b.min.z); // 0
    glVertex3f(b.max.x,b.max.y,b.max.z); // 5
    glVertex3f(b.min.x,b.max.y,b.max.z); // 6
    glVertex3f(b.min.x,b.max.y,b.min.z); // 1
    glEnd();

    glBegin (GL_LINE_LOOP);
    glVertex3f(b.max.x,b.min.y,b.max.z); // 4
    glVertex3f(b.min.x,b.min.y,b.max.z); // 7
    glVertex3f(b.min.x,b.min.y,b.min.z); // 2
    glVertex3f(b.max.x,b.min.y,b.min.z); // 3

    glEnd();

}

// Init with creation of displaylist
void initScene() {

	glEnable(GL_DEPTH_TEST);
	snowman_display_list = createDL();
}

// Creation if displaylist that calls drawSnowman() and is initialized in initScene
GLuint createDL() {
	GLuint snowManDL;

	// Create the id for the list
	snowManDL = glGenLists(1);

	// start list
	glNewList(snowManDL,GL_COMPILE);

	// call the function that contains
	// the rendering commands
		drawSnowMan();

	// endList
	glEndList();

	return(snowManDL);
}


// Call-back function for drawing the scene
void renderScene(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw ground
//glRotatef(-angle, 0.0f, 1.0, 0.0f);
	glColor3f(0.9f, 0.9f, 0.9f);
	glBegin(GL_QUADS);
		glVertex3f(-50.0f, 0.0f, -50.0f);
		glVertex3f(-50.0f, 0.0f,  50.0f);
		glVertex3f( 50.0f, 0.0f,  50.0f);
		glVertex3f( 50.0f, 0.0f, -50.0f);
	glEnd();


    //Draw character
    glPushMatrix();
        //glRotatef(angle, 0.0f, 1.0, 0.0f);
        glTranslatef(cx, cy, cz);

        glColor3f(1.0f, 0.0f, 0.0f);
        glutSolidCube(1); //Our character to follow
        glTranslated(-x,0.0f,-z);
    glPopMatrix();
	// Draw 36 SnowMen
    int nr = 0;
	for(int i = -3; i < 3; i++){
		for(int j=-3; j < 3; j++) {
			glPushMatrix();
			glTranslatef(i*10.0,0,j * 10.0);
			glCallList(snowman_display_list);
			glPopMatrix();

			//set max boundries
			boxes[nr].max.x = i*10.0 + 0.75;
			boxes[nr].max.y = 1.5;
			boxes[nr].max.z = j*10.0 + 0.75;

			//set min boundries
			boxes[nr].min.x = i*10.0 - 0.75;
			boxes[nr].min.y = 0.0;
			boxes[nr].min.z = j*10.0 - 0.75;
			nr++;
		}
	}
	for(int i = 0; i<36; i++){

	    /*
	    cout<<"Box nr: " << i <<"\n";
	    cout<<"min x: " << boxes[i].min.x <<"\n";
	    cout<<"min y: " << boxes[i].min.y <<"\n";
	    cout<<"min z: " << boxes[i].min.z <<"\n";

	    cout<<"max x: " << boxes[i].max.x <<"\n";
	    cout<<"max y: " << boxes[i].max.y <<"\n";
	    cout<<"max z: " << boxes[i].max.z <<"\n"<<"\n";
	    */
        drawbox(boxes[i]);
	}
	glutSwapBuffers();
}
// Func for handling passive mouse movements, ie movement of mouse
void processMousePassiveMotion(int mouse_x, int mouse_y) {


    int diffx=mouse_x-lastx; //check the difference between the current x and the last x position
    int diffy=mouse_y-lasty; //check the difference between the current y and the last y position

   // varDump(x,y, diffx, diffy);

    lastx=mouse_x; //set lastx to the current x position
    lasty=mouse_y; //set lasty to the current y position


    angle+= (float) diffx/10; //set the xrot to xrot with the addition of the difference in the x position
    y_angle += (float) diffy/-100;// set the xrot to yrot with the addition of the difference in the y position

    if(angle>=360 or angle<=-360){
        angle=0;
    }
    if(y_angle>=360 or y_angle<=-360){
        y_angle=0;
    }
    orientMe(angle);

}


// Calculating new cameraposition when walking forward/backward
void moveMeFlat(int direction) {
    float old_cx = cx;
    float old_cz = cz;

    cx = cx + direction*(-x)*0.1;
    cz = cz + direction*(-z)*0.1;

    if(checkBounds(cx, cy, cz) || CollisionTest(cx, cy, cz)){
        cx = old_cx;
        cz = old_cz;
    }

    //Anropa orientMe d�r loadIndentity och gluLookAt ligger
	orientMe(angle);

}



//Funtion for calculating now camera look-at position when turning
void orientMe(float ang) {

	x = -sin(ang)* dist;
	z = cos(ang)*dist;
//	varDump();
//	glutPostRedisplay();
	glLoadIdentity();
	gluLookAt(x + cx, y, z +cz,
		  	cx,cy,cz,
			0.0f,1.0f,0.0f);
}


//Callback-function for handling Special key keypresses
void inputSpecialKey(int key, int x, int y) {

	switch (key) {
		case GLUT_KEY_LEFT :
			angle -= 0.01f;
			orientMe(angle);break;
		case GLUT_KEY_RIGHT :
			angle +=0.01f;
			orientMe(angle);break;
		case GLUT_KEY_UP :
			moveMeFlat(1);break;
		case GLUT_KEY_DOWN :
			moveMeFlat(-1);break;
	}
}
//Callback-function for handling normal key keypresses
void processNormalKeys(unsigned char key, int x, int y) {

	if (key == 27)
		exit(0);
}

// Function for drawing a snowman
void drawSnowMan() {

	glColor3f(1.0f, 1.0f, 1.0f);

// Draw Body
	glTranslatef(0.0f ,0.75f, 0.0f);
	glutSolidSphere(0.75f,20,20);


// Draw Head
	glTranslatef(0.0f, 1.0f, 0.0f);
	glutSolidSphere(0.25f,20,20);

// Draw Eyes
	glPushMatrix();
	glColor3f(0.0f,0.0f,0.0f);
	glTranslatef(0.05f, 0.10f, 0.18f);
	glutSolidSphere(0.05f,10,10);
	glTranslatef(-0.1f, 0.0f, 0.0f);
	glutSolidSphere(0.05f,10,10);
	glPopMatrix();

// Draw Nose
	glColor3f(1.0f, 0.5f , 0.5f);
	glRotatef(0.0f,1.0f, 0.0f, 0.0f);
	glutSolidCone(0.08f,0.5f,10,2);
}

//Standard function for calculating resizing of window
void changeSize(int w, int h)
	{

	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	ratio = 1.0f * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// Set the viewport to be the entire window
    	glViewport(0, 0, w, h);

	// Set the clipping volume
	gluPerspective(45,ratio,1,1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(x, y, z,
		  	cx,cy,cz,
			0.0f,1.0f,0.0f);
	}

