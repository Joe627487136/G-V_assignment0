#include "GL/freeglut.h"
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include "vecmath.h"
#include <algorithm>
using namespace std;

// Globals
const GLfloat PI = 3.14;
int rotate_enabled = 0;
int color_counter = 0;
int light_pos_lr_counter = 0;
int light_pos_ud_counter = 0;
const int MAX_BUFFER_SIZE = 128;
float angle = 0.0;

GLboolean mouserdown = GL_FALSE;
GLboolean mouseldown = GL_FALSE;
GLboolean mousemdown = GL_FALSE;
/// when a mouse-key is pressed, record current mouse position 
static GLint mousex = 0, mousey = 0;

static GLfloat center[3] = {0.0f, 0.0f, 0.0f}; /// center position
static GLfloat eye[3]; /// eye's position

static GLfloat yrotate = PI/2; /// angle between y-axis and look direction
static GLfloat xrotate = PI/2; /// angle between x-axis and look direction
static GLfloat celength = 5.0f;/// lenght between center and eye

static GLfloat mSpeed = 0.4f; /// center move speed
static GLfloat rSpeed = 0.02f; /// rotate speed
static GLfloat lSpeed = 0.4f; /// reserved

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned int> > vecf;

// You will need more global variables to implement color and position changes


// These are convenience functions which allow us to call OpenGL 
// methods on Vec3d objects
inline void glVertex(const Vector3f &a)
{ glVertex3fv(a); }

inline void glNormal(const Vector3f &a) 
{ glNormal3fv(a); }

// calculate the eye position according to center position and angle,length
void CalEyePosition()
{
	if(yrotate > PI/2.2) yrotate = PI/2.2; 
	if(yrotate < 0.01)  yrotate = 0.01;
	if(xrotate > 2*PI)   xrotate = 0.01;
	if(xrotate < 0)   xrotate = 2 * PI;
	if(celength > 50)  celength = 50;  
	if(celength < 1)   celength = 1;

	eye[0] = center[0] + celength * sin(yrotate) * cos(xrotate);  
	eye[2] = center[2] + celength * sin(yrotate) * sin(xrotate);
	eye[1] = center[1] + celength * cos(yrotate);
}

/// CALLBACK func for mouse kicks
void MouseFunc(int button, int state, int x, int y)
{
if(state == GLUT_DOWN)
{
	if(button == GLUT_RIGHT_BUTTON) mouserdown = GL_TRUE;
	if(button == GLUT_LEFT_BUTTON) mouseldown = GL_TRUE;
	if(button == GLUT_MIDDLE_BUTTON)mousemdown = GL_TRUE;
}
else
{
	if(button == GLUT_RIGHT_BUTTON) mouserdown = GL_FALSE;
	if(button == GLUT_LEFT_BUTTON) mouseldown = GL_FALSE;
	if(button == GLUT_MIDDLE_BUTTON)mousemdown = GL_FALSE;
}
mousex = x, mousey = y;
}

// CALLBACK func for mouse motions
void MouseMotion(int x, int y)
{
	if(mouserdown == GL_TRUE)
	{
	xrotate += (x - mousex) / 80.0f;   // This number adjust rotating speed
	yrotate -= (y - mousey) / 120.0f;
	}

	if(mouseldown == GL_TRUE)
	{
	celength += (y - mousey) / 25.0f;
	}
	mousex = x, mousey = y;
	CalEyePosition();
	glutPostRedisplay();
}

void LookAt()
{
	CalEyePosition();
	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], 0, 1, 0);
}

// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27: // Escape key
        exit(0);
        break;
	case 'r':
		if (rotate_enabled){
			rotate_enabled = 0;
		}
		else {
			rotate_enabled = 1;
		}

		break;
    case 'c':
        // add code to change color here
		color_counter++;
		if (color_counter >= 16){
			color_counter = 0;}
        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;        
    }

	// this will refresh the screen so that the user sees the color change
    glutPostRedisplay();
}

// This function is called whenever a "Special" key press is received.
// Right now, it's handling the arrow keys.
void specialFunc( int key, int x, int y )
{
    switch ( key )
    {
    case GLUT_KEY_UP:
        // add code to change light position
		light_pos_ud_counter++;
		break;
    case GLUT_KEY_DOWN:
        // add code to change light position
		light_pos_ud_counter--;
		break;
    case GLUT_KEY_LEFT:
        // add code to change light position
		light_pos_lr_counter--;
		break;
    case GLUT_KEY_RIGHT:
        // add code to change light position
		light_pos_lr_counter++;
		break;
    }

	// this will refresh the screen so that the user sees the light position
    glutPostRedisplay();
}

// This function is responsible for displaying the object.
void drawScene(void)
{
    // int i;

    // Clear the rendering window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rotate the image
    glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
    glLoadIdentity();              // Initialize to the identity

    // Position the camera at [0,0,5], looking at [0,0,0],
    // with [0,1,0] as the up direction.
	/*
    gluLookAt(0.0, 0.0, 5.0,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);
	*/
	LookAt();

    // Set material properties of object

	// Set rotate function
	glRotatef(angle,1.0f,3.0f,2.0f); //rotating object continuously by 2 degree

	// Here are some colors you might use - feel free to add more
    GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
                                 {0.9, 0.5, 0.5, 1.0},
                                 {0.5, 0.9, 0.3, 1.0},
                                 {0.3, 0.8, 0.9, 1.0} };

	// Calculate smooth transition
	GLfloat dispColor[4];
	int start_color_index = color_counter / 4;
	float trans_level = (float)(color_counter % 4) / 4;
	GLfloat first_color[4];
	GLfloat second_color[4];
	for(int i = 0; i < 4; i ++){
		// copy out two color array that we want to perform smooth transition among
		first_color[i] = diffColors[start_color_index][i];
		second_color[i] = diffColors[(start_color_index + 1) % 4][i];
		dispColor[i] = (1 - trans_level) * first_color[i] + trans_level * second_color[i];
	}

	// Here we use the first color entry as the diffuse color
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, dispColor);

	// Define specular color and shininess
    GLfloat specColor[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat shininess[] = {100.0};

	// Note that the specular color and shininess can stay constant
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specColor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  
    // Set light properties

    // Light color (RGBA)
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
    // Light position
	GLfloat Lt0pos[] = {1.0f + 0.5 * light_pos_lr_counter, 1.0f + 0.5 * light_pos_ud_counter, 5.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
	// glutSolidTeapot(1.0);
	int a, c, d, f, g, i;
	for (unsigned int faceIndex = 0; faceIndex < vecf.size(); faceIndex++) {
		vector<unsigned int> z = vecf[faceIndex];
		a = z[0];
		c = z[2];
		d = z[3];
		f = z[5];
		g = z[6];
		i = z[8];

		glBegin(GL_TRIANGLES);
		glNormal3d(vecn[c-1][0], vecn[c-1][1], vecn[c-1][2]);
		glVertex3d(vecv[a-1][0], vecv[a-1][1], vecv[a-1][2]);
		glNormal3d(vecn[f-1][0], vecn[f-1][1], vecn[f-1][2]);
		glVertex3d(vecv[d-1][0], vecv[d-1][1], vecv[d-1][2]);
		glNormal3d(vecn[i-1][0], vecn[i-1][1], vecn[i-1][2]);
		glVertex3d(vecv[g-1][0], vecv[g-1][1], vecv[g-1][2]);
		glEnd();

	}
    
    // Dump the image to the screen.
    glutSwapBuffers();

}

// Initialize OpenGL's rendering modes
void initRendering()
{
    glEnable(GL_DEPTH_TEST);   // Depth testing must be turned on
    glEnable(GL_LIGHTING);     // Enable lighting calculations
    glEnable(GL_LIGHT0);       // Turn on light #0.
}

// Called when the window is resized
// w, h - width and height of the window in pixels.
void reshapeFunc(int w, int h)
{
    // Always use the largest square viewport possible
    if (w > h) {
        glViewport((w - h) / 2, 0, h, h);
    } else {
        glViewport(0, (h - w) / 2, w, w);
    }

    // Set up a perspective view, with square aspect ratio
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // 50 degree fov, uniform aspect ratio, near = 1, far = 100
    gluPerspective(50.0, 1.0, 1.0, 100.0);
}

void loadInput()
{
	// declare some temporary variables
	vector< unsigned int > vertexIndices, normalIndices;

	char lineHeader[MAX_BUFFER_SIZE];

	while( cin.getline(lineHeader, MAX_BUFFER_SIZE)){
		stringstream ss(lineHeader); 
		string s;
		ss >> s;

		if ( s == "v" ){
			Vector3f v;
			ss >> v[0] >> v[1] >> v[2];
			vecv.push_back(v);
			}
		else if ( s == "vn" ){
			Vector3f v;
			ss >> v[0] >> v[1] >> v[2];
			vecn.push_back(v);
			}
		else if ( s == "f" ){
			int a,b,c,d,e,f,g,h,i;
			string buffer;
			vector<unsigned int> v;
			char delimiter;
			//ss >> v[0] >> delimiter >> v[1] >> delimiter >> v[2] >> v[3] >> delimiter >> v[4] >> delimiter >> v[5]>> v[6] >> delimiter >> v[7] >> delimiter >> v[8];
			ss >> a >> delimiter >> b >> delimiter >> c >> d >> delimiter >> e >> delimiter >> f>> g >> delimiter >> h >> delimiter >> i;
			v.push_back(a);
			v.push_back(b);
			v.push_back(c);
			v.push_back(d);
			v.push_back(e);
			v.push_back(f);
			v.push_back(g);
			v.push_back(h);
			v.push_back(i);
			vecf.push_back(v);
		}
	}
}


void timerFunc(int value){
	// Perform rotate model function
	if (rotate_enabled){
		angle+=2.0f;
		if(angle>360.f){
			angle-=360;
		}
	}
	glutPostRedisplay();
	glutTimerFunc(25,timerFunc,0);
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main( int argc, char** argv )
{

	loadInput();

    glutInit(&argc,argv);

    // We're going to animate it, so double buffer 
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Initial parameters for window position and size
    glutInitWindowPosition( 60, 60 );
    glutInitWindowSize( 360, 360 );
    glutCreateWindow("Assignment 0");

    // Initialize OpenGL parameters.
    initRendering();

    // Set up callback functions for key presses
    glutKeyboardFunc(keyboardFunc);  // Handles "normal" ascii symbols
    glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys

	// Add mouse function
	glutMouseFunc(MouseFunc);
	glutMotionFunc(MouseMotion);

     // Set up the callback function for resizing windows
    glutReshapeFunc( reshapeFunc );

    // Call this whenever window needs redrawing
    glutDisplayFunc( drawScene );


	// Call this for rotating model
	glutTimerFunc(25,timerFunc,0);

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;	// This line is never reached.
}
