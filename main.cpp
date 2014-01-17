/* 
 * Adam Bysice - 0753709
 * File: main.cpp
 * ---------------------------
 * main program file, contains
 * most of the implementation
 */

#include <gl/freeglut.h>
#include <gl/gl.h>
#include "mathlib3d.h"
//#include "physics3d.h"
#include "camera.h"
#include <time.h>
#include <vector>
#include <random>
#include "textures.h"
#include "parser.h"
#include "pngloader.h"
#include <png.h>
#include "raycast.h"
#include "object.h"
//#include "sixense.h"

//function prototypes
void drawAxis();
void initializeCallbacks();
void drawWireBox(point3D corner, vec3D boxSize, vec3D rotation);



//constants
const int WindowWidth = 600;
const int WindowHeight = 600;
const int MAX_PARTICLES = 5;
const int BOUNDRY = 50;

//global variables
Camera* myCam = NULL;
GLuint* textures = NULL;
mesh* defaultMesh[5]; // default objects
material* defaultMaterial[3];
vector<object3D> olist;
vector<int> hitlist;
int selected = -1;
bool camera = false;
int currentSize = 0;

float light_pos[] = {5.0, 5.0, 5.0, 1.0};
float amb0[4]  = {1, 1, 1, 1};
float diff0[4] = {1, 1, 1, 1};
float spec0[4] = {1, 1, 1, 1};


// display funtion
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (camera) 
		myCam->CamUpdate();
	else
		gluLookAt(myCam->m_pos.x, myCam->m_pos.y,myCam->m_pos.z,
			myCam->m_target.x +myCam->m_pos.x, myCam->m_target.y + myCam->m_pos.y, myCam->m_target.z + myCam->m_pos.z,
			myCam->m_up.x, myCam->m_up.y, myCam->m_up.z);
	glPushMatrix();
		//Draw reference debuging plane
		glPushMatrix();
			drawAxis();
		glPopMatrix();
		//draw mesh
		glPushMatrix();
				// enable textures for the upcoming objects
				glEnable(GL_TEXTURE_2D);
				for(int i = 0; i < olist.size(); i++) { 
					glPushMatrix();
						//enable client state for VBO's
						glEnableClientState(GL_VERTEX_ARRAY);
						glEnableClientState(GL_NORMAL_ARRAY);
						glEnableClientState(GL_COLOR_ARRAY);
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
						glColorMaterial ( GL_FRONT_AND_BACK, GL_DIFFUSE );
						// Material properties
						glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  olist.at(i).mat.ambient);   
						glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  olist.at(i).mat.diffuse);  
						glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  olist.at(i).mat.specular); 
						glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,  olist.at(i).mat.shinyness);
						// Pick textture for model
						glBindTexture(GL_TEXTURE_2D, textures[olist.at(i).textures]);
						//move mesh's around
						glRotatef(olist.at(i).rotation.z,0.0,0.0,1.0);
						glRotatef(olist.at(i).rotation.y,0.0,1.0,0.0);
						glRotatef(olist.at(i).rotation.x,1.0,0.0,0.0);
						glTranslatef(olist.at(i).position.x, olist.at(i).position.y,olist.at(i).position.z);
						//draw wireframe if selected
						glPushMatrix();
						if (selected == i) {
							drawWireBox(point3D(0,0,0), olist.at(i).size*olist.at(i).scale, olist.at(i).rotation);
						}
						glPopMatrix();
						glScalef(olist.at(i).scale.x, olist.at(i).scale.y,olist.at(i).scale.z);
						//Use VBO's to draw Meshs
						glVertexPointer(3,GL_FLOAT, 0, olist.at(i).objmesh.vertexArray);
						glColorPointer(3,GL_FLOAT, 0, olist.at(i).objmesh.colourArray);
						glTexCoordPointer(2,GL_FLOAT,0, olist.at(i).objmesh.UVArray);
						glNormalPointer(GL_FLOAT, 0, olist.at(i).objmesh.normalArray);
						glDrawArrays(GL_TRIANGLES, 0, olist.at(i).objmesh.meshsize);
					glPopMatrix();
				}
				// disable textures so they dont apply to other primitives
				glDisable(GL_TEXTURE_2D);
			
		glPopMatrix();
		
	glPopMatrix();
	
	//draw stuff
	glutSwapBuffers();
	glutPostRedisplay();
}

//mouseclick function
void mousebut(int btn, int state, int x, int y) {
	
	//select object	
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//find which ones are hits
		point3D cam = point3D(myCam->m_pos.x,myCam->m_pos.y,myCam->m_pos.z);
		for(int i =0; i < olist.size(); i++) {
			if (rayCast(x, y, olist.at(i).position, olist.at(i).size*olist.at(i).scale, cam, olist.at(i).rotation)) {
				hitlist.push_back(i);
			}
		}
		// if there is more than 1 hit
		if (hitlist.size() > 1) {
			//find nearest
			int closest = 0;
			for(int i = 0; i< hitlist.size(); i++) {
				if (gdistance(cam, olist.at(hitlist.at(i)).position) < gdistance(cam, olist.at(hitlist.at(closest)).position)) {
					closest = i;
				}
			}
			// selected = nearest
			selected = hitlist.at(closest);
		}
		//if 1 hit
		else if (hitlist.size() == 1) {
			// if only 1 is hit then selected = that
			selected = hitlist.at(0);
		}
		//if no hits
		else 
			selected = -1;
		
		hitlist.clear();
	}
	//destroy object
	else if (btn == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && (olist.size() > 0)) {
		point3D cam = point3D(myCam->m_pos.x,myCam->m_pos.y,myCam->m_pos.z);
		for(int i = 0; i < olist.size(); i++) {
			//if hitbox is hit, erase object.
			if (rayCast(x, y, olist.at(i).position, olist.at(i).size*olist.at(i).scale, cam, olist.at(i).rotation)) {
				olist.erase (olist.begin()+i);
				currentSize--;
				break;
			}
		}

		selected = -1;
	}
}

//keyboard input function
void kbd(unsigned char key, int x, int y)
{
	// pass key commands to camera class
	if (camera) 
		myCam->OnKey(key);
	

	//quit if q is pressed
	if (key == 'q') {
		exit(0);
	}
	//toggles camera
	else if (key == 'c') {
		camera = !camera;
		if (!camera)
			glutSetCursor(GLUT_CURSOR_LEFT_ARROW); 
		else
			glutSetCursor(GLUT_CURSOR_NONE);
	}
	else if(key == 'r') { //reset scene
		olist.erase (olist.begin(),olist.begin()+currentSize);
		currentSize = 0;
		selected = -1;
	}
	else if (key == 'p' && olist.at(selected).scale.x > 0.1) { 		//scale x+
		olist.at(selected).scale.x += 0.1;
	}
	else if (key == 'l' && olist.at(selected).scale.x > 0.1) {      //scale x-
		olist.at(selected).scale.x -= 0.1;
	}
	else if (key == 'o' && olist.at(selected).scale.y > 0.1) { 		//scale y+
		olist.at(selected).scale.y += 0.1;
	}
	else if (key == 'k' && olist.at(selected).scale.y > 0.1) {      //scale y-
		olist.at(selected).scale.y -= 0.1;
	}
	else if (key == 'i' && olist.at(selected).scale.z > 0.1) { 		//scale z+
		olist.at(selected).scale.z += 0.1;
	}
	else if (key == 'j' && olist.at(selected).scale.z > 0.1) {      //scale z-
		olist.at(selected).scale.z -= 0.1;
	}
	else if (key == 'u') { 											//rotate x+
		olist.at(selected).rotation.x += 5;
	}
	else if (key == 'h') {                                         //rotate x-
		olist.at(selected).rotation.x -= 5;
	}
	else if (key == 'y') { 											//rotate y+
		olist.at(selected).rotation.y += 5;
	}
	else if (key == 'g') {
		olist.at(selected).rotation.y -= 5;
	}
	else if (key == 't') { 											//rotate z+
		olist.at(selected).rotation.z += 5;
	}
	else if (key == 'f') {
		olist.at(selected).rotation.z -= 5;
	}
	//change materials
	else if (key == '8') {
		olist.at(selected).mat = *defaultMaterial[0];
	}
	else if (key == '9') {
		olist.at(selected).mat = *defaultMaterial[1];
	}
	else if (key == '0') {
		olist.at(selected).mat = *defaultMaterial[2];
	}

	// create objects
	else if (key == '1') {
		// initialize object: can
		olist.push_back(object3D(0,6,0)); //starting pos
		olist.at(currentSize).size = vec3D(4,6,4); // size of hitbox
		olist.at(currentSize).objmesh = *defaultMesh[0];
		olist.at(currentSize).mat = *defaultMaterial[0];
		olist.at(currentSize).textures = 0;
		currentSize++;
	}
	else if (key == '2') {
		//initialize object: table (starts at 0,0,0 going positive dir)
		olist.push_back(object3D(0,4,0));	//starting pos
		olist.at(currentSize).size = vec3D(6,4,7);// size of hitbox
		olist.at(currentSize).objmesh = *defaultMesh[1];
		olist.at(currentSize).mat = *defaultMaterial[0];
		olist.at(currentSize).textures = 1;
		currentSize++;
	}
	else if (key == '3') {
		//initialize object: table (starts at 0,0,0 going positive dir)
		olist.push_back(object3D(0,3,0));	//starting pos
		olist.at(currentSize).size = vec3D(2,3,3.5);// size of hitbox
		olist.at(currentSize).objmesh = *defaultMesh[2];
		olist.at(currentSize).mat = *defaultMaterial[0];
		olist.at(currentSize).textures = 2;
		currentSize++;
	}
	else if (key == '4') {
		//initialize object: table (starts at 0,0,0 going positive dir)
		olist.push_back(object3D(0,4,0));	//starting pos
		olist.at(currentSize).size = vec3D(3.75,4,3.75);// size of hitbox
		olist.at(currentSize).objmesh = *defaultMesh[3];
		olist.at(currentSize).mat = *defaultMaterial[0];
		olist.at(currentSize).textures = 3;
		currentSize++;
	}
	else if (key == '5') {
		//initialize object: table (starts at 0,0,0 going positive dir)
		olist.push_back(object3D(0,3,0));	//starting pos
		olist.at(currentSize).size = vec3D(4,3,2);// size of hitbox
		olist.at(currentSize).objmesh = *defaultMesh[4];
		olist.at(currentSize).mat = *defaultMaterial[0];
		olist.at(currentSize).textures = 4;
		currentSize++;
	}
}

void speckbd(int key, int x, int y){
	// if something is selected... 
	
	if (selected > -1) {
		// Position changes
		if (key == GLUT_KEY_LEFT) {
			olist.at(selected).position.x++;
		}
		else if (key == GLUT_KEY_RIGHT) {
			olist.at(selected).position.x--;
		}
		else if (key == GLUT_KEY_UP) {
			olist.at(selected).position.y++;
		}
		else if (key == GLUT_KEY_DOWN) {
			olist.at(selected).position.y--;
		}
		else if (key == GLUT_KEY_END) {
			olist.at(selected).position.z++;
		}
		else if (key == GLUT_KEY_INSERT) {
			olist.at(selected).position.z--;
		}


	}
}

// keyboard "upkey" function, used in camera class for smooth movement
void ukbd(unsigned char key, int x, int y)
{
	//pass key up to camera class
	if (camera) 
		myCam->OnKeyUp(key);
}

// mouse func, tracks mouse position when no button is pressed
void pmouse(int x, int y)
{
	// pass mouse position to camera class
	if (camera) 
		myCam->OnMouse(x,y);
}
// runs when doing nothing else
void idle() {
	//do camera calculations
	if (camera) 
		myCam->OnDisplay();
}


// main, program entry point
int main(int argc, char** argv)
{
	// set the window size, display mode, and create the window
	glutInit(&argc, argv);
	glutInitWindowPosition(50,50);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Assignment 3");
	//glutEnterGameMode();

	glClearColor(0.5, 0.5, 0.5, 0);
	//textures
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glEnable (GL_BLEND);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	// materials + lighting
	glEnable(GL_LIGHTING); 
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb0);  
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0); 
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);

	//view stuff
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,1,1,200);
	glMatrixMode(GL_MODELVIEW);
	//culling
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	//register glut callbacks for keyboard and display function
	initializeCallbacks();

	//start the program!
	glutMainLoop();

	return 0;
}

// initializes the glut callbacks + camera stuff
void initializeCallbacks(){
	//callbacks
	glutKeyboardFunc(kbd);
	glutSpecialFunc(speckbd);
	glutPassiveMotionFunc(pmouse);
	glutMouseFunc(mousebut);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardUpFunc(ukbd);
	glutIgnoreKeyRepeat(TRUE);

	//load mesh's
	defaultMesh[0] = new mesh(parse("cantest.obj"));
	defaultMesh[1] = new mesh(parse("goodtable.obj"));
	defaultMesh[2] = new mesh(parse("sponge.obj"));
	defaultMesh[3] = new mesh(parse("Tomato.obj"));
	defaultMesh[4] = new mesh(parse("horse.obj"));
	//load textures
	textures = new GLuint[MAX_PARTICLES];
	glGenTextures(MAX_PARTICLES,textures);

	loadTexture(textures[0],"rustbin.png");
	loadTexture(textures[1],"wood9.png");
	loadTexture(textures[2],"spongbob.png");
	loadTexture(textures[3],"Tomato_texture.png");
	loadTexture(textures[4],"Hourse_Texture.png");
		
	//initialize material
	defaultMaterial[0] = new material;
	defaultMaterial[0]->ambient[0] = 0.3;  defaultMaterial[0]->ambient[1] = 0.3; defaultMaterial[0]->ambient[2] = 0.3; defaultMaterial[0]->ambient[3] = 0.3; defaultMaterial[0]->ambient[4] = 1;
	defaultMaterial[0]->diffuse[0] = 0.3;  defaultMaterial[0]->diffuse[1] = 0.3; defaultMaterial[0]->diffuse[2] = 0.3; defaultMaterial[0]->diffuse[3] = 0.3; defaultMaterial[0]->ambient[4] = 1;
	defaultMaterial[0]->specular[0] = 0.9;  defaultMaterial[0]->specular[1] = 0.9; defaultMaterial[0]->specular[2] = 0.9; defaultMaterial[0]->specular[3] = 0.9; defaultMaterial[0]->specular[4] = 1;
	defaultMaterial[0]->shinyness = 50.0; 
	defaultMaterial[1] = new material;
	defaultMaterial[1]->ambient[0] = 0.4;  defaultMaterial[1]->ambient[1] = 0.1; defaultMaterial[1]->ambient[2] = 0.1; defaultMaterial[1]->ambient[3] = 0.7; defaultMaterial[1]->ambient[4] = 0.1;
	defaultMaterial[1]->diffuse[0] = 0.1;  defaultMaterial[1]->diffuse[1] = 0.5; defaultMaterial[1]->diffuse[2] = 0.7; defaultMaterial[1]->diffuse[3] = 0.8; defaultMaterial[1]->ambient[4] = 0.1;
	defaultMaterial[1]->specular[0] = 0.1;  defaultMaterial[1]->specular[1] = 0.3; defaultMaterial[1]->specular[2] = 0.6; defaultMaterial[1]->specular[3] = 0.7; defaultMaterial[1]->specular[4] = 0.1;
	defaultMaterial[1]->shinyness = 10.0; 
	defaultMaterial[2] = new material;
	defaultMaterial[2]->ambient[0] = 0.1;  defaultMaterial[2]->ambient[1] = 0.1; defaultMaterial[2]->ambient[2] = 0.7; defaultMaterial[2]->ambient[3] = 0.1; defaultMaterial[2]->ambient[4] = .5;
	defaultMaterial[2]->diffuse[0] = 0.5;  defaultMaterial[2]->diffuse[1] = 0.2; defaultMaterial[2]->diffuse[2] = 0.4; defaultMaterial[2]->diffuse[3] = 0.4; defaultMaterial[2]->ambient[4] = .5;
	defaultMaterial[2]->specular[0] = 0.2;  defaultMaterial[2]->specular[1] = 0.8; defaultMaterial[2]->specular[2] = 0.2; defaultMaterial[2]->specular[3] = 0.2; defaultMaterial[2]->specular[4] = .2;
	defaultMaterial[2]->shinyness = 100.0; 
	//input commands
	cout << "Done Loading" << endl;
	printf("------------------------------------------------- \n");
	printf("Key 'c':		toggles camera(moves around scene) \n");
	printf("Key w/s:		move camera forward/back \n");
	printf("Key a/d:		move camera left/right \n");
	printf("Mouse:			FPS-style camera control \n");
	printf("Mouse Left:		select object at cursor \n");
	printf("Mouse Right:	delete object at cursor \n");
	printf("Key 1-5:		create object at origin \n");
	printf("Key 8-0:		changes selected object materials \n");
	printf("Key 'r':		resets the scene \n");
	printf("Key left/right/end: position + X/Y/Z \n");
	printf("Key up/down/insert:	position X/Y/Z \n");
	printf("Key p/o/i:		scale + X/Y/Z \n");
	printf("Key l/k/j:		scale - X/Y/Z \n");
	printf("Key u/y/t:		rotates + X/Y/Z \n");
	printf("Key h/g/f:		rotates - X/Y/Z \n");
	printf("***COMPOUND TRANSFORMATIONS STILL BUGGY*** \n");
	printf("Key 'q':		quits program \n");

	
	//initialize camera
	vec3D eyepos(20,5,-20);
	vec3D lookpos(-1,0,1);
	vec3D up(0,1,0);
	myCam = new Camera(WindowWidth, WindowHeight, eyepos,lookpos,up);

	glutSetCursor(GLUT_CURSOR_LEFT_ARROW); 
	//enable Razer Hydra
	//sixenseInit();	
}

void drawWireBox(point3D corner, vec3D boxSize, vec3D rotation) {

	glBegin(GL_LINES);
		//face 1
		glVertex3f(corner.x, corner.y,corner.z); //p1
		glVertex3f(corner.x - boxSize.x, corner.y, corner.z); //p2
		glVertex3f(corner.x, corner.y,corner.z); //p1
		glVertex3f(corner.x, corner.y - boxSize.y, corner.z); //p3
		glVertex3f(corner.x - boxSize.x, corner.y, corner.z); //p2
		glVertex3f(corner.x - boxSize.x, corner.y - boxSize.y, corner.z); //p4
		glVertex3f(corner.x, corner.y - boxSize.y, corner.z); //p3
		glVertex3f(corner.x - boxSize.x, corner.y - boxSize.y, corner.z); //p4
		glVertex3f(corner.x, corner.y,corner.z); //p1
		glVertex3f(corner.x - boxSize.x, corner.y - boxSize.y, corner.z); //p4
		glVertex3f(corner.x, corner.y - boxSize.y, corner.z); //p3
		glVertex3f(corner.x - boxSize.x, corner.y, corner.z); //p2

		//face 2
		glVertex3f(corner.x, corner.y,corner.z); //p1
		glVertex3f(corner.x, corner.y,corner.z- boxSize.z); // p5
		glVertex3f(corner.x, corner.y - boxSize.y, corner.z); //p3
		glVertex3f(corner.x, corner.y- boxSize.y, corner.z-boxSize.z); //p7
		glVertex3f(corner.x, corner.y,corner.z); //p1
		glVertex3f(corner.x, corner.y- boxSize.y, corner.z-boxSize.z); //p7
		glVertex3f(corner.x, corner.y - boxSize.y, corner.z); //p3
		glVertex3f(corner.x, corner.y- boxSize.y, corner.z-boxSize.z); //p7
		glVertex3f(corner.x, corner.y,corner.z- boxSize.z); // p5
		glVertex3f(corner.x, corner.y- boxSize.y, corner.z-boxSize.z); //p7
		glVertex3f(corner.x, corner.y - boxSize.y, corner.z); //p3
		glVertex3f(corner.x, corner.y,corner.z- boxSize.z); // p5
		
		//face 3
		glVertex3f(corner.x, corner.y,corner.z- boxSize.z); // p5
		glVertex3f(corner.x - boxSize.x, corner.y, corner.z- boxSize.z); //p6
		glVertex3f(corner.x, corner.y- boxSize.y, corner.z-boxSize.z); //p7
		glVertex3f(corner.x- boxSize.x, corner.y - boxSize.y, corner.z-boxSize.z); //p8
		glVertex3f(corner.x - boxSize.x, corner.y, corner.z- boxSize.z); //p6
		glVertex3f(corner.x- boxSize.x, corner.y - boxSize.y, corner.z-boxSize.z); //p8
		glVertex3f(corner.x- boxSize.x, corner.y - boxSize.y, corner.z-boxSize.z); //p8
		glVertex3f(corner.x, corner.y,corner.z- boxSize.z); // p5
		glVertex3f(corner.x - boxSize.x, corner.y, corner.z- boxSize.z); //p6
		glVertex3f(corner.x, corner.y- boxSize.y, corner.z-boxSize.z); //p7
		
		//face 4
		glVertex3f(corner.x - boxSize.x, corner.y, corner.z); //p2
		glVertex3f(corner.x - boxSize.x, corner.y, corner.z- boxSize.z); //p6
		glVertex3f(corner.x - boxSize.x, corner.y - boxSize.y, corner.z); //p4
		glVertex3f(corner.x- boxSize.x, corner.y - boxSize.y, corner.z-boxSize.z); //p8
		glVertex3f(corner.x - boxSize.x, corner.y, corner.z- boxSize.z); //p6
		glVertex3f(corner.x - boxSize.x, corner.y - boxSize.y, corner.z); //p4
		glVertex3f(corner.x- boxSize.x, corner.y - boxSize.y, corner.z-boxSize.z); //p8
		glVertex3f(corner.x - boxSize.x, corner.y, corner.z); //p2

		//top 
		glVertex3f(corner.x - boxSize.x, corner.y, corner.z- boxSize.z); //p6
		glVertex3f(corner.x, corner.y,corner.z); //p1
		glVertex3f(corner.x, corner.y,corner.z- boxSize.z); // p5
		glVertex3f(corner.x - boxSize.x, corner.y, corner.z); //p2
		
		//bottom
		glVertex3f(corner.x- boxSize.x, corner.y - boxSize.y, corner.z-boxSize.z); //p8
		glVertex3f(corner.x, corner.y - boxSize.y, corner.z); //p3
		glVertex3f(corner.x, corner.y- boxSize.y, corner.z-boxSize.z); //p7
		glVertex3f(corner.x - boxSize.x, corner.y - boxSize.y, corner.z); //p4
	glEnd();

}


// draw plane
void drawAxis()
{
	glBegin(GL_QUADS);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,defaultMaterial[0]->ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, defaultMaterial[0]->diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, defaultMaterial[0]->specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,defaultMaterial[0]->shinyness);
		glColor3f(0.1,0.1,0.1);
		glVertex3f(-BOUNDRY,-0.05,-BOUNDRY);
		glVertex3f(-BOUNDRY,-0.05,BOUNDRY);
		glVertex3f(BOUNDRY,-0.05,BOUNDRY);
		glVertex3f(BOUNDRY,-0.05,-BOUNDRY);
	glEnd();
}



