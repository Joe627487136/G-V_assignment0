
// Please use this chunk of code
// to load the correct opengl and glut
// for your platform
#ifdef __APPLE__
#include <GLUT/glut.h>
#elif _WIN32
#include <windows.h>
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif
// --------------------------------

#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>
#include "vecmath.h"
#include <fstream>
#include <algorithm>


// Included some helper macros and functions
// for those who miss the brevity of python.

using namespace std;

// Globals


const GLfloat PI = 3.14;

/// record the state of mouse
GLboolean mouserdown = GL_FALSE;
GLboolean mouseldown = GL_FALSE;
GLboolean mousemdown = GL_FALSE;

/// when a mouse-key is pressed, record current mouse position
GLint mousex = 0, mousey = 0;

GLfloat center[3] = {0.0f, 0.0f, 0.0f}; /// center position
GLfloat eye[3]; /// eye's position

GLfloat yrotate = PI/4; /// angle between y-axis and look direction
GLfloat xrotate = PI/4; /// angle between x-axis and look direction
GLfloat celength = 5.0f;/// lenght between center and eye

// This is the var for counting color loops
int color_count;

// This is the var for transition toggle
bool transition_toggle=0;

// This is the var for transitional steep
float steep = 0.0;

// This is the bool for detecting color fade back to initial one
bool fade_to_begin = 0;

// This is the var as coef for mutiplying the light position, coordinate as [ver,hor]
int light_shift[2] = {0,0};

// This is the list of points (3D vectors)
vector<Vector3f> vecv;

// This is the list of normals (also 3D vectors)
vector<Vector3f> vecn;

// This is the list of faces (indices into vecv and vecn)
vector<vector<unsigned> > vecf;

// Bool to controll rotation
bool rotate_state = 0;

// Starting rotating coeifficient
GLfloat angle_x = 0.0f;

// Color set
GLfloat Colors_set[4][4] = { {0.5, 0.5, 0.9, 1.0},
    {0.9, 0.5, 0.5, 1.0},
    {0.5, 0.9, 0.3, 1.0},
    {0.3, 0.8, 0.9, 1.0} };

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
    if(celength < 5)   celength = 5;

    eye[0] = center[0] + celength * sin(yrotate) * cos(xrotate);
    eye[2] = center[2] + celength * sin(yrotate) * sin(xrotate);
    eye[1] = center[1] + celength * cos(yrotate);
}

/// CALLBACK func for mouse kicks
void MouseFunc(int button, int state, int x, int y)
{
    if(state == GLUT_DOWN)
    {
        if(button == GLUT_RIGHT_BUTTON){
            cout << "mouse right down" << endl;
            mouserdown = GL_TRUE;
        }
        if(button == GLUT_LEFT_BUTTON){
            cout << "mouse left down" << endl;
            mouseldown = GL_TRUE;
        }
        if(button == GLUT_MIDDLE_BUTTON){
            cout << "mouse middle down" << endl;
            mousemdown = GL_TRUE;
        }
    }
    else
    {
        if(button == GLUT_RIGHT_BUTTON) mouserdown = GL_FALSE;
        if(button == GLUT_LEFT_BUTTON) mouseldown = GL_FALSE;
        if(button == GLUT_MIDDLE_BUTTON)mousemdown = GL_FALSE;
    }
    mousex = x, mousey = y;
}

/// CALLBACK func for mouse motions
void MouseMotion(int x, int y)
{
    if(mouserdown == GL_TRUE)
    {
        xrotate += (x - mousex) / 80.0f;
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



// This function is called whenever a "Normal" key press is received.
void keyboardFunc( unsigned char key, int x, int y )
{
    
    
	switch ( key )
	{
		case 27: // Escape key
			exit(0);
			break;
		case 'c':
			// add code to change color here
			cout << "Change color! " << key << "." << endl;
            transition_toggle = 1;
            steep = 0;
            color_count++;
            if (color_count>=1 && color_count<=3){
                fade_to_begin=0;
            }
            if (color_count>=4){
                fade_to_begin=1;
                color_count=0;
            }
			break;
        case 'r':
            if (rotate_state==0){
                cout << "Rotating! " << key << "." << endl;
                rotate_state=1;
            }
            else if (rotate_state==1){
                cout << "Stop rotating! " << key << "." << endl;
                rotate_state=0;
            }
            
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
			cout << "Light up" << endl;
            light_shift[0]++;
			break;
		case GLUT_KEY_DOWN:
			// add code to change light position
			cout << "Light down" << endl;
            light_shift[0]--;
			break;
		case GLUT_KEY_LEFT:
			// add code to change light position
			cout << "Light left" << endl;
            light_shift[1]--;
			break;
		case GLUT_KEY_RIGHT:
			// add code to change light position
			cout << "Light right" << endl;
            light_shift[1]++;
			break;
	}
	
	// this will refresh the screen so that the user sees the light position
	glutPostRedisplay();
}
//New draw function
//draw desired object with vectors vecf, vecv, vecn
void draw(void){
	for(int faceIndex=0; faceIndex < vecf.size(); faceIndex++) {
		vector<unsigned int> v = vecf[faceIndex];
        int a = v[0], b = v[1], c = v[2], d = v[3], e = v[4], f = v[5], g = v[6], h = v[7], i = v[8];
        glBegin(GL_TRIANGLES);
        glNormal3d(vecn[c-1][0], vecn[c-1][1], vecn[c-1][2]);
        glVertex3d(vecv[a-1][0], vecv[a-1][1], vecv[a-1][2]);
        glNormal3d(vecn[f-1][0], vecn[f-1][1], vecn[f-1][2]);
        glVertex3d(vecv[d-1][0], vecv[d-1][1], vecv[d-1][2]);
        glNormal3d(vecn[i-1][0], vecn[i-1][1], vecn[i-1][2]);
        glVertex3d(vecv[g-1][0], vecv[g-1][1], vecv[g-1][2]);
        
        glEnd();
    }
}

// Timer func1 controlling rotation
void TimerCal1(int value){
    if(rotate_state) {
        angle_x += 3.f;
    }
    glutPostRedisplay();
    glutTimerFunc(20, TimerCal1, 0);
}

// Timer func2 controlling smooth transition effect
void TimerCal2(int value){
    if(transition_toggle){
        steep = steep + 0.1;
        cout <<steep<<endl;
        if(steep>=1){
            cout << "Reset!" << endl;
            cout << color_count << endl;
            transition_toggle = 0;
            steep = 1;
            
        }
        
    }
    glutPostRedisplay();
    glutTimerFunc(20, TimerCal2, 0);
}

void LookAt()
{
    CalEyePosition();
    gluLookAt(eye[0], eye[1], eye[2],center[0], center[1], center[2],0, 1, 0);
}

// This function is responsible for displaying the object.
void drawScene(void)
{
	int i;
	
	// Clear the rendering window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Rotate the image
	glMatrixMode( GL_MODELVIEW );  // Current matrix affects objects positions
	glLoadIdentity();              // Initialize to the identity
	
	// Position the camera at [0,0,5], looking at [0,0,0],
	// with [0,1,0] as the up direction.
	//gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    LookAt();
    
    
	// Here are some colors you might use - feel free to add more
	GLfloat diffColors[4][4] = { {0.5, 0.5, 0.9, 1.0},
		{0.9, 0.5, 0.5, 1.0},
		{0.5, 0.9, 0.3, 1.0},
		{0.3, 0.8, 0.9, 1.0} };
    GLfloat out_rgb[4] = {diffColors[color_count][0],diffColors[color_count][1],diffColors[color_count][2],1};
    GLfloat out_rgb_tst[4] = {diffColors[color_count][0],diffColors[color_count][1],diffColors[color_count][2],1};
    if (color_count>0){
        GLfloat r_trans = diffColors[color_count-1][0]*(1-steep)+diffColors[color_count][0]*steep;
        GLfloat g_trans = diffColors[color_count-1][1]*(1-steep)+diffColors[color_count][1]*steep;
        GLfloat b_trans = diffColors[color_count-1][2]*(1-steep)+diffColors[color_count][2]*steep;
        out_rgb[0] = r_trans;
        out_rgb[1] = g_trans;
        out_rgb[2] = b_trans;
    }
    if (color_count==0 && fade_to_begin==1){
        GLfloat r_trans = diffColors[3][0]*(1-steep)+diffColors[color_count][0]*steep;
        GLfloat g_trans = diffColors[3][1]*(1-steep)+diffColors[color_count][1]*steep;
        GLfloat b_trans = diffColors[3][2]*(1-steep)+diffColors[color_count][2]*steep;
        out_rgb[0] = r_trans;
        out_rgb[1] = g_trans;
        out_rgb[2] = b_trans;
    }
	// Here we use the first color entry as the diffuse color
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, out_rgb);
	
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
	GLfloat Lt0pos[] = {1.0f+light_shift[1], 1.0f+light_shift[0], 5.0f, 1.0f};
	
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
	glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);
    glRotatef(angle_x, 0.0, 1.0, 0.0);
	
	// This GLUT method draws a teapot.  You should replace
	// it with code which draws the object you loaded.
	//glutSolidTeapot(1.0);
    draw();
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
    cout << "Loading started" << endl;
    char buffer[100];
    while(cin.getline(buffer, 100)){
        cout << "Loading" << endl;
        stringstream ss(buffer);
        Vector3f v;
        string s;
        ss>>s;
        if(s=="v"){
            ss>>v[0]>>v[1]>>v[2];
            vecv.push_back(v);
        }else if(s=="vn"){
            ss>>v[0]>>v[1]>>v[2];
            vecn.push_back(v);
        }else if(s=="f"){
            vector<unsigned> faces;
            string abc;
            string def;
            string ghi;
            ss>>abc>>def>>ghi;
            string faceIndices = abc + " " + def + " " + ghi;
            replace(faceIndices.begin(), faceIndices.end(), '/', ' ');
            stringstream ssFaces(faceIndices);
            unsigned n;
            while (ssFaces >> n) {
                faces.push_back(n);
            }
            vecf.push_back(faces);
        }else{}
    };
    cout << "Loading finished" << endl;
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
	glutKeyboardFunc(keyboardFunc); // Handles "normal" ascii symbols
	glutSpecialFunc(specialFunc);   // Handles "special" keyboard keys
    
    glutMouseFunc(MouseFunc);
    glutMotionFunc(MouseMotion);
    
	// Set up the callback function for resizing windows
	glutReshapeFunc( reshapeFunc );
	
	// Call this whenever window needs redrawing
	glutDisplayFunc( drawScene );
	
    // Timer Set
    glutTimerFunc(20, TimerCal1, 0);
    glutTimerFunc(20, TimerCal2, 0);
    
	// Start the main loop.  glutMainLoop never returns.
	glutMainLoop( );
	
	return 0;	// This line is never reached.
}
