/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 1

This project virtually displays 3 of the IKEA Dalfred Stools in a 
10ft by 10ft coordinate grid. There are special features such as 
adjustable seats and fog which can be user controlled.
*/

#include <iostream>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Stool.h"

using namespace std;


//This structure is defined to contain data for setting up the window dimensions
//and managing the wireframe option. 
struct WindowData
{
    GLint height, width;
    GLint handle;
    bool wireframe;
} window, window2;

//Three stool objects to be displayed.
Stool stool1, stool2, stool3;

//Angles defining the rotation of the camera.
double RotatedX = 70;
double RotatedY = 0;

//How far away the near plane is.
GLfloat nearDist = 0.1f;

//How much elevation has been applied to the movable seat.
GLfloat adjustableSeat = 0;

//An array of vec4's containing the vertices of the frustum in the third person view.
glm::vec4 frustumPoints[8];

/*
	Rotates the camera accordingly and then returns the modified modelview matrix.

	Initially, I also created a feature in the render method that allowed the point about
	which the camera rotates to be moved. This was done by a position vector being updated by 
	keyboard input and then used as a translate parameter in the render method. However, this caused
	the frustum to become distorted as the frustum's reference point was also changed. Therefore, I 
	chose not to include this in the program as it made the display look terrible when used.
 */
//glm::vec3 position;
glm::mat4 render(glm::mat4 Model){
	//Model = glm::translate(Model, position);
    Model = glm::rotate(Model, (GLfloat) RotatedX, glm::vec3(1,0,0));
    Model = glm::rotate(Model, (GLfloat) RotatedY, glm::vec3(0,1,0));
    return Model;
}

//Changes the value of the angle in which the camera should be rotated vertically.
void rotateX(double angle){
    if(!(RotatedX>=89 && angle>0) && !(RotatedX<=-89 && angle<0)){
        RotatedX += angle;
    }
}

//Changes the value of the angle in which the camera should be rotated horizontally.
void rotateY(double angle){
    RotatedY += angle;
}

//Initialize and derive the frustum points from the modelview and projection matrices. 
void getFrustumPoints(glm::mat4 Model, glm::mat4 Proj){
    //Initialize Frustum Points
	//Near plane
	frustumPoints[0].x=-1;frustumPoints[0].y=-1;frustumPoints[0].z=-1;frustumPoints[0].w=1;
	frustumPoints[1].x=1;frustumPoints[1].y=-1;frustumPoints[1].z=-1;frustumPoints[0].w=1;
	frustumPoints[2].x=1;frustumPoints[2].y=1;frustumPoints[2].z=-1;frustumPoints[2].w=1;
	frustumPoints[3].x=-1;frustumPoints[3].y=1;frustumPoints[3].z=-1;frustumPoints[3].w=1;
	//Far plane
	frustumPoints[4].x=-1;frustumPoints[4].y=-1;frustumPoints[4].z=1;frustumPoints[4].w=1;
	frustumPoints[5].x=1;frustumPoints[5].y=-1;frustumPoints[5].z=1;frustumPoints[5].w=1;
	frustumPoints[6].x=1;frustumPoints[6].y=1;frustumPoints[6].z=1;frustumPoints[0].w=1;
	frustumPoints[7].x=-1;frustumPoints[7].y=1;frustumPoints[7].z=1;frustumPoints[0].w=1;

    //Multiply and adjust scale to get frustum vertices
    for(int i=0; i<8; i++){
        frustumPoints[i] =  glm::inverse(Model) * glm::inverse(Proj) * frustumPoints[i];
		frustumPoints[i].x /= frustumPoints[i].w;
		frustumPoints[i].y /= frustumPoints[i].w;
		frustumPoints[i].z /= frustumPoints[i].w;
		frustumPoints[i].w = 1.0f;
    }  
}

//Displays the frustum lines, the camera, and the camera's coordinate lines in the third person view. 
void makeFrustumLines(glm::mat4 Model){
    glColor3f(0.45f,0.45f,0.75f);
	
	//cameraLoc is a vector in the direction of the camera with a unit length of 15 (exact position of the camera).
	glm::vec4 cameraLoc;
	cameraLoc = glm::normalize(frustumPoints[0]) + glm::normalize(frustumPoints[1]) + glm::normalize(frustumPoints[2]) + glm::normalize(frustumPoints[3]);
	cameraLoc = (3.75) * cameraLoc;
	
	//Shift the local origin to the camera's position
	Model = glm::translate(Model, glm::vec3(cameraLoc.x ,cameraLoc.y,cameraLoc.z));
	glLoadMatrixf(glm::value_ptr(Model));
	
	//Creates the triangular camera object
	glBegin(GL_POLYGON);
	glVertex3f(0,0,0);
	glVertex3f((7/nearDist)*(frustumPoints[2].x-frustumPoints[1].x)/2, (7/nearDist)*(frustumPoints[2].y-frustumPoints[1].y)/2, (7/nearDist)*(frustumPoints[2].z-frustumPoints[1].z)/2);
	glVertex3f(cameraLoc.x/6, cameraLoc.y/6, cameraLoc.z/6);
	glVertex3f(0,0,0);

	glColor3f(0.75f,0.45f,0.75f);
	glVertex3f(0,0,0);
	glVertex3f((7/nearDist)*(frustumPoints[1].x-frustumPoints[0].x)/2, (7/nearDist)*(frustumPoints[1].y-frustumPoints[0].y)/2, (7/nearDist)*(frustumPoints[1].z-frustumPoints[0].z)/2);
	glVertex3f(cameraLoc.x/6, cameraLoc.y/6, cameraLoc.z/6);
	glVertex3f(0,0,0);

	glColor3f(0.45f,0.45f,0.15f);
	glVertex3f(0,0,0);
	glVertex3f((7/nearDist)*(frustumPoints[1].x-frustumPoints[0].x)/2, (7/nearDist)*(frustumPoints[1].y-frustumPoints[0].y)/2, (7/nearDist)*(frustumPoints[1].z-frustumPoints[0].z)/2);
	glVertex3f((7/nearDist)*(frustumPoints[2].x-frustumPoints[1].x)/2, (7/nearDist)*(frustumPoints[2].y-frustumPoints[1].y)/2, (7/nearDist)*(frustumPoints[2].z-frustumPoints[1].z)/2);
	glVertex3f(0,0,0);
	glEnd();

	//Creates the coordinate axes about the camera
	glLineWidth(2);
	glColor3f(0.0,0.0,1.0);
	glBegin(GL_LINES);	
	glVertex3f(cameraLoc.x/2, cameraLoc.y/2, cameraLoc.z/2);
	glVertex3f(0,0,0);
	glEnd();
	glColor3f(0.0,1.0,0.0);
	glBegin(GL_LINES);	
	glVertex3f((10/nearDist)*(frustumPoints[2].x-frustumPoints[1].x)/2, (10/nearDist)*(frustumPoints[2].y-frustumPoints[1].y)/2, (10/nearDist)*(frustumPoints[2].z-frustumPoints[1].z)/2);
	glVertex3f(0,0,0);
	glEnd();
	glColor3f(1.0,0.0,0.0);
	glBegin(GL_LINES);	
	glVertex3f((10/nearDist)*(frustumPoints[1].x-frustumPoints[0].x)/2, (10/nearDist)*(frustumPoints[1].y-frustumPoints[0].y)/2, (10/nearDist)*(frustumPoints[1].z-frustumPoints[0].z)/2);
	glVertex3f(0,0,0);
	glEnd(); 
	Model = glm::translate(Model, glm::vec3(-cameraLoc.x,-cameraLoc.y,-cameraLoc.z));
    glLoadMatrixf(glm::value_ptr(Model));

	//Draws the entire frustum
	glLineWidth(2);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_POLYGON);
	glColor3f(1.0, 1.0, 1.0);
	
	//Draw near plane
    glVertex3f(frustumPoints[0].x, frustumPoints[0].y, frustumPoints[0].z);
    glVertex3f(frustumPoints[1].x, frustumPoints[1].y, frustumPoints[1].z);
    glVertex3f(frustumPoints[2].x, frustumPoints[2].y, frustumPoints[2].z);
    glVertex3f(frustumPoints[3].x, frustumPoints[3].y, frustumPoints[3].z);
    glVertex3f(frustumPoints[0].x, frustumPoints[0].y, frustumPoints[0].z);

	//Draw far plane
    glVertex3f(frustumPoints[4].x, frustumPoints[4].y, frustumPoints[4].z);
    glVertex3f(frustumPoints[5].x, frustumPoints[5].y, frustumPoints[5].z);
    glVertex3f(frustumPoints[6].x, frustumPoints[6].y, frustumPoints[6].z);
    glVertex3f(frustumPoints[7].x, frustumPoints[7].y, frustumPoints[7].z);
    glVertex3f(frustumPoints[4].x, frustumPoints[4].y, frustumPoints[4].z);

	//Connect near and far planes
	glVertex3f(frustumPoints[0].x, frustumPoints[0].y, frustumPoints[0].z);
	glVertex3f(frustumPoints[4].x, frustumPoints[4].y, frustumPoints[4].z);
    glVertex3f(frustumPoints[5].x, frustumPoints[5].y, frustumPoints[5].z);
	glVertex3f(frustumPoints[1].x, frustumPoints[1].y, frustumPoints[1].z);
	glVertex3f(frustumPoints[2].x, frustumPoints[2].y, frustumPoints[2].z);
    glVertex3f(frustumPoints[6].x, frustumPoints[6].y, frustumPoints[6].z);
    glVertex3f(frustumPoints[7].x, frustumPoints[7].y, frustumPoints[7].z);
	glVertex3f(frustumPoints[3].x, frustumPoints[3].y, frustumPoints[3].z);
    glVertex3f(frustumPoints[0].x, frustumPoints[0].y, frustumPoints[0].z);
    glVertex3f(frustumPoints[4].x, frustumPoints[4].y, frustumPoints[4].z);	

	//Connect near plane to camera
	glVertex3f(frustumPoints[0].x, frustumPoints[0].y, frustumPoints[0].z);
	glVertex3f(cameraLoc.x, cameraLoc.y, cameraLoc.z);
    glVertex3f(frustumPoints[1].x, frustumPoints[1].y, frustumPoints[1].z);
    glVertex3f(frustumPoints[2].x, frustumPoints[2].y, frustumPoints[2].z);
	glVertex3f(cameraLoc.x, cameraLoc.y, cameraLoc.z);
    glVertex3f(frustumPoints[3].x, frustumPoints[3].y, frustumPoints[3].z);
    glVertex3f(frustumPoints[0].x, frustumPoints[0].y, frustumPoints[0].z);
	glVertex3f(cameraLoc.x, cameraLoc.y, cameraLoc.z);
    glEnd();

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

//Creates the coordinate axes for x(blue), y(green), and z(red)
void makeCoordinateAxes(GLfloat lineWidth, GLdouble length){
    glLineWidth(lineWidth);
    glColor3f(1.0,0.0,0.0);
    glBegin(GL_LINES);
    glVertex3d(0,0,0);
    glVertex3d(length,0,0);
    glEnd();

    glColor3f(0.0,1.0,0.0);
    glBegin(GL_LINES);
    glVertex3d(0,0,0);
    glVertex3d(0,length,0);
    glEnd();

    glColor3f(0.0,0.0,1.0);
    glBegin(GL_LINES);
    glVertex3d(0,0,0);
    glVertex3d(0,0,length);
    glEnd();
}

//Creates the coordinate grid
void xyGrid(){
    glColor3f(0.7f,0.7f,0.7f);
    glLineWidth((GLfloat) 0.1);
    for(double i=-9.4; i<=9.4; i+=0.94){
        glBegin(GL_LINES);
        if(i!=0){
            glVertex3d(i,0,-9.4);
            glVertex3d(i,0,9.4);
        }
        else{
            glVertex3d(i,0,-9.4);
            glVertex3d(i,0,0);
        }
        glEnd();              
    }
    for(double i=-9.4; i<=9.4; i+=0.94){
        glBegin(GL_LINES);
        if(i!=0){
            glVertex3d(-9.4,0,i);
            glVertex3d(9.4,0,i);
        }
        else{
            glVertex3d(-9.4,0,i);
            glVertex3d(0,0,i);
        }
        glEnd();              
    }
}

//Used as a temporary copy of the modelview matrix and is repeatedly overwritten
//in its use of placing the components of the stool.
glm::mat4 another;

//Used to shift a component of the stool with respect to the local origin of the 
//modelview matrix. 
glm::mat4 Stool::translate(glm::mat4 Model, GLfloat x, GLfloat y, GLfloat z){
    Model = glm::rotate(Model, (GLfloat) -90, glm::vec3(1,0,0));
    Model = glm::translate(Model, glm::vec3(x,y,z));
    Model = glm::rotate(Model, (GLfloat) 90, glm::vec3(1,0,0));
    glLoadMatrixf(glm::value_ptr(Model));
    return Model;
}

//Produces one stool leg
void Stool::makeLeg(){
	//Dimensions of stool leg.
    GLfloat xC, yC, zC, offset;
    xC = (GLfloat)(3.25*0.25/3);yC = (GLfloat)(3.86);zC = (GLfloat)(0.88);offset=(GLfloat)(3.25*0.25/3);

    glBegin(GL_POLYGON);

    //Bottom
    glVertex3f(0, 0, 0);
    glVertex3f(xC, 0, 0);
    glVertex3f(xC, 0, offset);
    glVertex3f(0, 0, offset);
    glVertex3f(0, 0, 0);

    //Front
    glVertex3f(0, yC, zC+offset);
    glVertex3f(xC, yC, zC+offset);
    glVertex3f(xC, 0, offset);
    glVertex3f(0, 0, offset);
    glVertex3f(0, yC, zC+offset);

    //Back
    glVertex3f(0, yC, zC);
    glVertex3f(xC, yC, zC);
    glVertex3f(xC, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, yC, zC);

    //Left
    glVertex3f(0, yC, zC+offset);
    glVertex3f(0, yC, zC);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, offset);
    glVertex3f(0, yC, zC+offset);

    //Right
    glVertex3f(xC, yC, zC+offset);
    glVertex3f(xC, yC, zC);
    glVertex3f(xC, 0, 0);
    glVertex3f(xC, 0, offset);
    glVertex3f(xC, yC, zC+offset);

    //Top
    glVertex3f(0, yC, zC+offset);
    glVertex3f(xC, yC, zC+offset);
    glVertex3f(xC, yC, zC);
    glVertex3f(0, yC, zC);
    glVertex3f(0, yC, zC+offset);

    glEnd();
}

//Draws and connects all the components of the stool 
void Stool::stools(glm::mat4 Model, GLfloat x, GLfloat z){
    GLUquadric *q = gluNewQuadric();
    glMatrixMode(GL_MODELVIEW);

	//Compensates for the offset in x and z coordinates.
    x = x-1;z = z-1;

	//Produces the four legs of the stool
    another = glm::translate(Model, glm::vec3((1-(3.25*0.25/6)+x),0,(-0.525+z)));
    glLoadMatrixf(glm::value_ptr(another));
    glColor3f(1.0f, 0.45f, 0.65f);
    makeLeg();  

    another = glm::translate(Model, glm::vec3((1+(3.25*0.25/6)+x),0,(2.525+z)));
    another = glm::rotate(another, (GLfloat) 180, glm::vec3(0,1,0));
    glLoadMatrixf(glm::value_ptr(another));
    makeLeg();

    another = glm::translate(Model, glm::vec3((2.525+x),0,(1-(3.25*0.25/6)+z)));
    another = glm::rotate(another, (GLfloat) 270, glm::vec3(0,1,0));
    glLoadMatrixf(glm::value_ptr(another));
    makeLeg();

    another = glm::translate(Model, glm::vec3((-0.525+x),0,(1+(3.25*0.25/6)+z)));
    another = glm::rotate(another, (GLfloat) 90, glm::vec3(0,1,0));
    glLoadMatrixf(glm::value_ptr(another));
    makeLeg();

	//Light blue base support
    another = glm::translate(Model, glm::vec3((1+x),0.927,(1+z)));
    another = glm::rotate(another, (GLfloat) 90, glm::vec3(1,0,0));
    glLoadMatrixf(glm::value_ptr(another));
    glColor3f(0.0,0.5,0.75);
    glutSolidTorus(0.123, 0.927, 50, 50);

	//Red cylindrical seat support/height changer
    another = glm::translate(Model, glm::vec3((1+x),3.917+adjustableSeat,(1+z)));
    another = glm::rotate(another, (GLfloat) 90, glm::vec3(1,0,0));
    glLoadMatrixf(glm::value_ptr(another));  
    glColor3f(1.0,0.0,0.0);
    gluCylinder(q, 0.123, 0.123, 1.5, 50, 50);
    gluDisk(q, 0, 0.123, 50, 50);
    another = translate(another,0.0f,-1.5f,0.0f);  
    gluDisk(q, 0, 0.123, 50, 50);

	//Blue cylindrical support
    another = glm::translate(Model, glm::vec3((1+x),3.357,(1+z)));
    another = glm::rotate(another, (GLfloat) 90, glm::vec3(1,0,0));
    glLoadMatrixf(glm::value_ptr(another));      
    glColor3f(0.0,0.0,0.75);
    gluCylinder(q, 0.503, 0.503, 0.15, 50, 50);
    gluDisk(q, 0, 0.503, 50, 50);
    another = translate(another,0.0f,-0.15f,0.0f);
    gluDisk(q, 0, 0.503, 50, 50);

	//Yellow upper cylindrical support
    another = glm::translate(Model, glm::vec3((1+x),3.877,(1+z)));
    another = glm::rotate(another, (GLfloat) 90, glm::vec3(1,0,0));
    glLoadMatrixf(glm::value_ptr(another));
    glColor3f(1.0,1.0,0.0);  
    gluCylinder(q, 0.375, 0.375, 0.133, 50, 50);
    gluDisk(q, 0, 0.375, 50, 50);
    another = translate(another,0.0f,-0.133f,0.0f);
    gluDisk(q, 0, 0.375, 50, 50);

	//Green seat
    another = glm::translate(Model, glm::vec3((1+x),3.982+adjustableSeat,(1+z)));
    another = glm::rotate(another, (GLfloat) 90, glm::vec3(1,0,0));
    glLoadMatrixf(glm::value_ptr(another));
    glColor3f(0.0,0.75,0.0);
    gluCylinder(q, 0.92, 0.85, 0.083, 50, 50);
    glColor3f(0.0f,0.95f,0.0f);
    gluDisk(q, 0, 0.92, 50, 50);
    another = translate(another,0.0f,-0.083f,0.0f);
    glColor3f(0.0f,0.55f,0.0f);
    gluDisk(q, 0, 0.85, 50, 50);

    gluDeleteQuadric(q);  
}

//Settings for the fog and a boolean to determine whether to produce fog.
GLfloat density = 0.15f;
GLfloat fogColor[4] = {0.2f, 0.2f, 0.2f};
bool fogOn = false;

//If the fog is set to be on, then this function will produce fog.
//Otherwise, the fog is turned off.
void makeFog(){
	if(fogOn){
		glEnable(GL_FOG); 
		glFogi(GL_FOG_MODE, GL_EXP2); 
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_DENSITY, density);
		glHint(GL_FOG_HINT, GL_NICEST);
	}
	else{
		glDisable(GL_FOG);
	}
}

//Sets the properties for the lighting in the display.
GLfloat whiteSpecularLight[] = {0.15f, 0.15f, 0.15f};
GLfloat whiteAmbientLight[] = {0.65f, 0.65f, 0.65f};
GLfloat whiteDiffuseLight[] = {0.15f, 0.15f, 0.15f};

GLfloat whiteAmbientMaterial[] = {0.1f, 0.5f, 0.8f};
GLfloat whiteSpecularMaterial[] = {1.0f, 1.0f, 1.0f};
GLfloat mShininess[] = {0.75f, 0.75f, 0.75f};

GLfloat lightPosition[] = {-5, -5, -5, 0};
GLfloat lightDirection[] = {5, 5, 5};

double M_PI = 3.14;
void drw_polygon(int n = 30, int arg = 0, float mult = 1, float v = 1.0) {
    /*
        Function drw_polygon:
        Arguments:
            n - number of sides
            arg - starting angle (not so important at all)
            mult - multiplying sides to incrase their length
            v - cylinder height
    */
	n=40;
    // DumbProof Double Check :)
    if (arg < 0)
        arg = 0;
 
    // Cylinder Bottom
    glBegin(GL_POLYGON);
        glColor4f(1.0, 0.0, 0.0, 1.0);
        for(int i = arg; i <= (360 + arg); i += (360 / n)) {
            float a = i * M_PI / 180; // degrees to radians
            glVertex3f(mult * cos(a), mult * sin(a), 0.0);
        }
    glEnd();
 
    // Cylinder Top
    glBegin(GL_POLYGON);
        glColor4f(0.0, 0.0, 1.0, 1.0);
        for(int i = arg; i <= (360 + arg); i += (360 / n)) {
            float a = i * M_PI / 180; // degrees to radians
            glVertex3f(mult * cos(a), mult * sin(a), v);
        }
    glEnd();
 
    // Cylinder "Cover"
    glBegin(GL_QUAD_STRIP);
        glColor4f(1.0, 1.0, 0.0, 1.0);
        for(int i = arg; i < 480; i += (360 / n)) {
            float a = i * M_PI / 180; // degrees to radians
            glVertex3f(mult * cos(a), mult * sin(a), 0.0);
            glVertex3f(mult * cos(a), mult * sin(a), v);
        }
    glEnd();
 
 
 
}

void makeTri(int x, int y, int z, glm::vec3 points[]){
	glBegin(GL_POLYGON);
			glVertex3f(points[x].x, points[x].y, points[x].z);
			glVertex3f(points[y].x, points[y].y, points[y].z);
			glVertex3f(points[z].x, points[z].y, points[z].z);
	glEnd();
}

void drawMesh(){
	const int n=50; //(length of one side)
	glm::vec3 points[n*n];
	int index = 0;
	for(int i=0; i<n; i++){
		for(int j=0; j<n; j++){
			points[index] = glm::vec3((float)((float)j/(float)(n-1)), (float)((float)i/(float)(n-1)), 0); 
			index++;
		}
	}

	glColor4f(0.0, 1.0, 0.0, 1.0);
	for(int i=0; i<((n-1)*(n)); i++){
		if((i%n) != n-1){
		glBegin(GL_TRIANGLES);
			glVertex3f(points[i].x, points[i].y, points[i].z);
			glVertex3f(points[i+1].x, points[i+1].y, points[i+1].z);
			glVertex3f(points[i+n].x, points[i+n].y, points[i+n].z);
		glEnd();
		}
	}
	for(int i=n-1; i<(n*n); i++){
		if(i%n != (n-1)){
		glBegin(GL_TRIANGLES);
			glVertex3f(points[i].x, points[i].y, points[i].z);
			glVertex3f(points[i-(n-1)].x, points[i-(n-1)].y, points[i-(n-1)].z);
			glVertex3f(points[i+1].x, points[i+1].y, points[i+1].z);
		glEnd();
		}
	}
}

void drawCylinder(const float r, const float rA, const float h){
	const int n=360+2; 
	glm::vec3 pointsA[n];
	glm::vec3 pointsB[n];
	int index=0;
	float diff = r - rA;
	/*
	for(int i=0; i<h; i+=(h-1)){
		for(float j=0; j<=360; j+=30.0){
			points[index]=glm::vec3((float)((float)(r - (i/(h-1))*diff)*cos(j)),(float)((float)(r - (i/(h-1))*diff)*sin(j)), i);
			index++;
		}
	}*/
	if(diff ==0){
		for(float j=0; j<=361; j+=1.0){
			pointsA[index]=glm::vec3((float)((float)(r)*cos(j)),(float)((float)(r))*sin(j), 0.0f);
			index++;
		}
		index = 0;
		for(float j=0; j<=361; j+=1.0){
			pointsB[index]=glm::vec3((float)((float)(r)*cos(j)),(float)((float)(r))*sin(j), h);
			index++;
		}
	}
	else{
		for(float j=0; j<=361; j+=1.0){
			pointsA[index]=glm::vec3((float)((float)(r)*cos(j)),(float)((float)(r))*sin(j), 0.0f);
			index++;
		}
		index = 0;
		for(float j=0; j<=361; j+=1.0){
			pointsB[index]=glm::vec3((float)((float)(rA)*cos(j)),(float)((float)(rA))*sin(j), h);
			index++;
		}
	}

	for(int i=0; i<=360; i++){
		glBegin(GL_TRIANGLES);
			glVertex3f(pointsA[i].x, pointsA[i].y, pointsA[i].z);
			glVertex3f(pointsA[i+1].x, pointsA[i+1].y, pointsA[i+1].z);
			glVertex3f(pointsB[i].x, pointsB[i].y, pointsB[i].z);
		glEnd();
		glBegin(GL_TRIANGLES);
			glVertex3f(pointsB[i].x, pointsB[i].y, pointsB[i].z);
			glVertex3f(pointsA[i+1].x, pointsA[i+1].y, pointsA[i+1].z);
			glVertex3f(pointsB[i+1].x, pointsB[i+1].y, pointsB[i+1].z);
		glEnd();
	}
}

void drawSphere(const float r){
	const int n = 180;
	glm::vec3 points[16471];
	int index=0;
	
	for(float i=0; i<=180; i+=2){
		for(float j=0; j<=360; j+=2){
			float u = 2* M_PI * float(i) / 180;
			float v = 2* M_PI * (float(j) + u) / 360;
			points[index]=glm::vec3((float)((float)(r)*cos(v)*sin(u)),(float)((float)(r)*sin(v)*(float)(sin(u))),(float)((float)(r)*cos(u)));
			index++;
		}
	}
	index = 0;
	glColor3f(1.0,1.0,1.0);
	for(int i=0; i<(((n/2)-1)*(n)); i++){
		if(index == 544){
				glColor3f(1.0,0.0,0.0);
			}
		if((i%n) != n-1){
			makeTri(i, i+1, i+n, points);
		}
		else{
			makeTri(i, i-(n-1), i+n, points);
		}
		index++;
	}
	for(int i=n-1; i<(n*(n/2)); i++){
		if(i%n != (n-1)){
			makeTri(i, i-(n-1), i+1, points);
		}
		else{
			makeTri(i, i-(2*n-1), i-(n-1), points);
		}
	}
}

//For project 3
void drawWings(const float r){
	const int n = 360/2;
	glm::vec3 points[16471];
	int index=0;
	float rb = r+3.5*r;
	for(float i=0; i<=180; i+=2){
		for(float j=0; j<=360; j+=2){
			float u = 2* M_PI * float(i) / 180;
			float v = 2* M_PI * (float(j) + u) / 360;
			points[index]=glm::vec3((float)((float)(r)*cos(v)*sin(u)),(float)((float)(r)*sin(v)*(float)(sin(u))),(float)((float)(rb)*cos(v)));
			index++;
		}
	}
	
	for(int i=0; i<(((n/2)-1)*(n)); i++){
		if((i%n) != n-1){
			makeTri(i, i+1, i+n, points);
		}
	}
	for(int i=n-1; i<(n*(n/2)); i++){
		if(i%n != (n-1)){
			makeTri(i, i-(n-1), i+1, points);
		}
	}
}
void drawJets(const float r, const float rA, const float h){
	const int n=37; 
	glm::vec3 points[2*n];
	int index=0;
	float diff = r - rA;
	for(int i=0; i<=h; i+=h){
		for(float j=0; j<=360; j+=10){
			points[index]=glm::vec3((float)((float)(r - (i/h)*diff)*cos(j)),(float)((float)(r - (i/h)*diff)*sin(j)), i);
			index++;
		}
	}

	for(int i=0; i<n; i++){
		if((i%n) != (n-1)){
		glBegin(GL_TRIANGLES);
			glVertex3f(points[i].x, points[i].y, points[i].z);
			glVertex3f(points[i+1].x, points[i+1].y, points[i+1].z);
			glVertex3f(points[i+n].x, points[i+n].y, points[i+n].z);
		glEnd();
		}
		else{
			glBegin(GL_TRIANGLES);
			glVertex3f(points[i].x, points[i].y, points[i].z);
			glVertex3f(points[i-(n-1)].x, points[i-(n-1)].y, points[i-(n-1)].z);
			glVertex3f(points[i+n].x, points[i+n].y, points[i+n].z);
		glEnd();
		}
	}
	for(int i=n; i<(2*n); i++){
		if(i%n != (n-1)){
		glBegin(GL_TRIANGLES);
			glVertex3f(points[i].x, points[i].y, points[i].z);
			glVertex3f(points[i-(n-1)].x, points[i-(n-1)].y, points[i-(n-1)].z);
			glVertex3f(points[i+1].x, points[i+1].y, points[i+1].z);
		glEnd();
		}
		else{
			glBegin(GL_TRIANGLES);
			glVertex3f(points[i].x, points[i].y, points[i].z);
			glVertex3f(points[i-(2*n-1)].x, points[i-(2*n-1)].y, points[i-(2*n-1)].z);
			glVertex3f(points[i-(n-1)].x, points[i-(n-1)].y, points[i-(n-1)].z);
			glEnd();
		}
	}
}

void drawTorus(const float R, const float r){
	const int n = 360/10;
	glm::vec3 points[(n+1)*(1+n/2)];
	int index=0;

	for(float i=0; i<=180; i+=10){
		for(float j=0; j<=360; j+=10){
			float u = 2* M_PI * float(i) / 180;
			float v = 2* M_PI * (float(j) + u) / 360;
			points[index]=glm::vec3((float)((R + (float)(r)*cos(u))*cos(v)),(float)((R + (float)(r)*cos(u))*sin(v)),(float)((float)(r)*sin(u)));
			index++;
		}
	}
	/*
	for(float i=0; i<=180; i+=90){
		for(float j=0; j<=360; j+=90){
			points[index]=glm::vec3((float)((R + (float)(r)*cos(i))*cos(j)),(float)((R + (float)(r)*cos(i))*sin(j)),(float)((float)(r)*sin(i)));
			index++;
		}
	}*/
	
	/*
	for(std::size_t i=0, index=0; i<=num_rings; ++i) {
      for(std::size_t j=0; j<=num_segments; ++j, ++index) {
        // Compute texture coordinates (surface parameters)
        // Note that u is added to j. This creates a spiral pattern, which
        // allows us to draw the entire torus in one triangle strip without
        // using degenerate triangles.
        // (Yet we still need degenerate triangles to avoid texture
        // wrap-around)
        float u = float(i) / num_rings;
        float v = (float(j) + u) / num_segments;

        // Compute angles
        float u_angle = u * 2 * pi;
        float v_angle = v * 2 * pi;

        // Position
        float x = cos(u_angle) * (r1 + cos(v_angle) * r2);
        float y = sin(u_angle) * (r1 + cos(v_angle) * r2);
        float z = sin(v_angle) * r2;

        // Normal vector
        float nx = cos(u_angle) * cos(v_angle);
        float ny = sin(u_angle) * cos(v_angle);
        float nz = sin(v_angle);

        vertices[index].position = D3DXVECTOR3(x, y, z);
        vertices[index].normal = D3DXVECTOR3(nx, ny, nz);
        vertices[index].texcoords = D3DXVECTOR2(u, v);
      }
    }

	*/
	for(int i=0; i<(((n/2)-1)*(n)); i++){
		if((i%n) != n-1){
		glBegin(GL_TRIANGLES);
			glVertex3f(points[i].x, points[i].y, points[i].z);
			glVertex3f(points[i+1].x, points[i+1].y, points[i+1].z);
			glVertex3f(points[i+n].x, points[i+n].y, points[i+n].z);
		glEnd();
		}
	}
	//glColor4f(1.0, 1.0, 0.0, 1.0);
	for(int i=n-1; i<(n*(n/2)); i++){
		if(i%n != (n-1)){
		glBegin(GL_TRIANGLES);
			glVertex3f(points[i].x, points[i].y, points[i].z);
			glVertex3f(points[i-(n-1)].x, points[i-(n-1)].y, points[i-(n-1)].z);
			glVertex3f(points[i+1].x, points[i+1].y, points[i+1].z);
		glEnd();
		}
	}
}

void drawDisc(){
	const int n=3600; //(length of one side)
	glm::vec3 points[n+1];
	int r=5;
	points[0]=glm::vec3(0,0,0);
	int index = 1;
	for(float i=0.1; i<=360.1; i+=0.1){
		points[index]=glm::vec3((float)((float)(r)*cos(i)),(float)((float)(r)*sin(i)), 0);
		index++;
	}
	glColor3f(1.0, 1.0, 0.0);
	for(int i=0; i<n; i++){
		if(i != n-1){
			glBegin(GL_TRIANGLES);
			glVertex3f(points[i].x, points[i].y, points[i].z);
			glVertex3f(points[i+1].x, points[i+1].y, points[i+1].z);
			glVertex3f(points[0].x, points[0].y, points[0].z);
			glEnd();
		}
		else{
			glBegin(GL_TRIANGLES);
			glVertex3f(points[i].x, points[i].y, points[i].z);
			glVertex3f(points[1].x, points[1].y, points[1].z);
			glVertex3f(points[0].x, points[0].y, points[0].z);
			glEnd();
		}
	}
}
void drawCube(const float l){
	glm::vec3 points[8];
	int index = 0;
	points[0] = glm::vec3(0,l,0); 
	points[1] = glm::vec3(l,l,0);
	points[2] = glm::vec3(l,l,l);
	points[3] = glm::vec3(0,l,l);
	points[4] = glm::vec3(0,0,0);
	points[5] = glm::vec3(l,0,0);
	points[6] = glm::vec3(l,0,l);
	points[7] = glm::vec3(0,0,l);

	glColor4f(1.0, 1.0, 0.0, 1.0);
	makeTri(0,1,4,points);
	makeTri(4,1,5,points);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	makeTri(1,2,5,points);
	makeTri(5,2,6,points);
	glColor4f(0.0,0.0,0.0, 1.0);
	makeTri(2,3,6,points);
	makeTri(6,3,7,points);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	makeTri(3,0,7,points);
	makeTri(7,0,4,points);

	glColor4f(1.0,0.5,0.0, 1.0);
	makeTri(3,2,0,points);
	makeTri(0,2,1,points);
	glColor4f(0.5,0.0,0.75, 1.0);
	makeTri(7,6,4,points);
	makeTri(4,6,5,points);
	
}

void drawStuff(const float r){
	const int n = 180;
	glm::vec3 points[16471];
	int index=0;
	
	for(float i=0; i<=180; i+=2){
		for(float j=0; j<=360; j+=2){
			float u = 2* M_PI * float(i) / 180;
			float v = 2* M_PI * (float(j) + u) / 360;
			points[index]=glm::vec3((float)((float)(r+5)*(1/(pow(i+1,2)))*cos(v)*sin(u)),(float)((float)(r+5)*sin(v)*(float)(sin(u))),(float)((float)(r)*cos(u)));
			index++;
		}
	}
	glColor3f(0,0,1);
	for(int i=0; i<(((n/2)-1)*(n)); i++){
		if((i%n) != n-1){
			makeTri(i, i+1, i+n, points);
		}
		else{
			makeTri(i, i-(n-1), i+n, points);
		}
	}
	glColor3f(1,1,1);
	for(int i=n-1; i<(n*(n/2)); i++){
		if(i%n != (n-1)){
			makeTri(i, i-(n-1), i+1, points);
		}
		else{
			makeTri(i, i-(2*n-1), i-(n-1), points);
		}
	}
}

void drawStuff2(const float r){
	const int n = 180;
	glm::vec3 points[16471];
	int index=0;
	
	for(float i=0; i<=180; i+=2){
		for(float j=0; j<=360; j+=2){
			float u = 2* M_PI * float(i) / 180;
			float v = 2* M_PI * (float(j) + u) / 360;
			points[index]=glm::vec3((float)((float)(r+2)*cos(v)*sin(u)),(float)((float)(r)*pow(cos(v),2)*log(v)*(float)(sin(u))),(float)((float)(r+1)*cos(u)));
			index++;
		}
	}
	glColor3f(1,0,0);
	for(int i=0; i<(((n/2)-1)*(n)); i++){
		if((i%n) != n-1){
			makeTri(i, i+1, i+n, points);
		}
		else{
			makeTri(i, i-(n-1), i+n, points);
		}
	}
	glColor3f(0,0,0);
	for(int i=n-1; i<(n*(n/2)); i++){
		if(i%n != (n-1)){
			makeTri(i, i-(n-1), i+1, points);
		}
		else{
			makeTri(i, i-(2*n-1), i-(n-1), points);
		}
	}
}

//Produces the images for the first person view.
void display()
{
	//Sets the fog settings and displays fog when necessary.
	density = 0.15f;
	makeFog();

    //Produces the lighting effects
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);	

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightPosition);

    glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, whiteAmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuseLight);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, whiteAmbientMaterial);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);

	glShadeModel(GL_SMOOTH);

    //Set up settings for the projection matrix
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL); //For wireframe mode
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
	glm::mat4 projection_matrix = glm::perspective(45.0f, float(window.width) / float(window.height), nearDist, 60.0f);
    glLoadMatrixf(glm::value_ptr(projection_matrix));
    glViewport(0, 0, window.width, window.height);
    glMatrixMode(GL_MODELVIEW);

	//Set up for the modelview matrix
    glm::mat4 modelview_matrix;
    modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 0.0f, -19.0f));
	//Allows the camera to rotate around the environment.
    modelview_matrix = render(modelview_matrix);
    glLoadMatrixf(glm::value_ptr(modelview_matrix));

	//
	//drawTorus(5, 1);
	glColor4f(1.0, 1.0, 1.0, 1.0);
	//drawTorus(5, 1);
	/*
	modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 0.0f, -2.5f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
	glColor4f(1.0, 1.0, 1.0, 1.0);
	drawTorus(5, 1);
	glColor3f(0.0,0.0,0.0);
	//drawTorus(5, 0.55);
	modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 0.0f, 2.5f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));

	glColor3f(1.0,0.0,0.0);
	//drawCylinder(15, 1.5, 50);
	glColor3f(0.0,0.0,0.0);
	//drawCylinder(2.5, 2.5, 5);
	
	glColor3f(0.0,0.0,0.0);
	glutSolidSphere(0.75, 15, 15);
	glColor3f(1.0,0.0,0.0);
	drawSphere(10);*/
/*
	modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 0.0f, 2.5f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
	glColor3f(1.0,1.0,1.0);
	drawCylinder(3, 3, 5);
	glColor3f(0.0,0.0,0.0);
	drawCylinder(2.5, 2.5, 5);
	modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 0.0f, -2.5f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
	*/
	/*modelview_matrix = glm::translate(modelview_matrix, glm::vec3(-20.0f, -20.0f, -20.0f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
	drawCube(40);
	modelview_matrix = glm::translate(modelview_matrix, glm::vec3(20.0f, 20.0f, 20.0f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
	
	modelview_matrix = glm::rotate(modelview_matrix, (GLfloat) 90, glm::vec3(0,1,0));
	glLoadMatrixf(glm::value_ptr(modelview_matrix));
	glColor3f(0.25,0.25,0.35);
	modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 4.0f, 0.0f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
	drawWings(1);
	modelview_matrix = glm::rotate(modelview_matrix, (GLfloat) 90, glm::vec3(0,0,1));
	glLoadMatrixf(glm::value_ptr(modelview_matrix));
	glColor3f(1.0, 0, 0);
	drawStuff(1.5);
	drawStuff2(1.1);
	modelview_matrix = glm::rotate(modelview_matrix, (GLfloat) -90, glm::vec3(0,0,1));
	glLoadMatrixf(glm::value_ptr(modelview_matrix));
	modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, -4.0f, 0.0f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
	modelview_matrix = glm::rotate(modelview_matrix, (GLfloat) -90, glm::vec3(0,1,0));
	glLoadMatrixf(glm::value_ptr(modelview_matrix));
	modelview_matrix = glm::translate(modelview_matrix, glm::vec3(-2.75f, 4.0f, -1.5f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
	glColor3f(0.0, 0.0, 0.0);
	drawJets(1, 0, 3.5);
	drawCylinder(1,0,3.5);
	glColor3f(1.0, 0.0, 0.0);
	drawJets(0.9, 0, 2.9);
	glColor3f(1.0, 0.5, 0.0);
	drawJets(0.75, 0, 2.9);
	glColor3f(1.0, 1.0, 0.0);
	drawJets(0.8, 0, 2.9);
	glColor3f(1.0, 1.0, 1.0);
	drawJets(0.4, 0, 2.9);
	modelview_matrix = glm::translate(modelview_matrix, glm::vec3(2.75f, -4.0f, 1.5f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
	modelview_matrix = glm::translate(modelview_matrix, glm::vec3(2.75f, 4.0f, -1.5f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
	glColor3f(0.0, 0.0, 0.0);
	drawJets(1, 0, 3.5);
	drawCylinder(1,0,3.5);
	glColor3f(1.0, 0.0, 0.0);
	drawJets(0.9, 0, 2.9);
	glColor3f(1.0, 0.5, 0.0);
	drawJets(0.75, 0, 2.9);
	glColor3f(1.0, 1.0, 0.0);
	drawJets(0.8, 0, 2.9);
	glColor3f(1.0, 1.0, 1.0);
	drawJets(0.4, 0, 2.9);
	modelview_matrix = glm::translate(modelview_matrix, glm::vec3(-2.75f, -4.0f, 1.5f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));*/
	
	/*
	modelview_matrix = glm::rotate(modelview_matrix, (GLfloat) 180, glm::vec3(0,1,0));
	glLoadMatrixf(glm::value_ptr(modelview_matrix));
	//modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 0.0f, -50.0f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
	drawCylinder(3, 0, 5);
	//modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 0.0f, 50.0f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
	modelview_matrix = glm::rotate(modelview_matrix, (GLfloat) -180, glm::vec3(0,1,0));
	glLoadMatrixf(glm::value_ptr(modelview_matrix));*/
	//drawSphere(2);
	//drawDisc();
	//drawMesh();
	//drw_polygon(3, 0, 1, 1.0);
	//Create the environment
    makeCoordinateAxes(3, 9.4);
    xyGrid();

	//Creates stools at the specified coordinates.
    stool1.stools(modelview_matrix, 4,3);
    stool2.stools(modelview_matrix, 1,-2);
    stool3.stools(modelview_matrix, -2,2);

	//Gathers the frustum locations for use in the third person display.
	getFrustumPoints(modelview_matrix, projection_matrix);

    glutSwapBuffers();
    glutPostRedisplay();  
}

//Produces the images for the third person view.
void display2()
{
	//Sets the fog settings and displays fog when necessary.
	density = 0.05f;
	makeFog();

    //Produces the lighting effects
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);	

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightPosition);

    glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, whiteAmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuseLight);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, whiteAmbientMaterial);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);

	glShadeModel(GL_SMOOTH);

    //Set up settings for the projection matrix
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL); //For wireframe mode
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
	glm::mat4 projection_matrix = glm::perspective(45.0f, float(window2.width) / float(window2.height), nearDist, 508.0f);
    glLoadMatrixf(glm::value_ptr(projection_matrix));
    glViewport(0, 0, window2.width, window2.height);
    glMatrixMode(GL_MODELVIEW);
    glm::mat4 modelview_matrix;
    
	//Set up for the modelview matrix
    modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, -5.0f, -35.0f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));

	//Create the environment
    makeCoordinateAxes(3, 9.4);
    xyGrid();

	//Creates stools at the specified coordinates.
    stool1.stools(modelview_matrix, 4,3);
    stool2.stools(modelview_matrix, 1,-2);
    stool3.stools(modelview_matrix, -2,2);
	
	//Creates the frustum display in the thrid person window.
	glColor3f(0.5, 0.5, 0.5);
	makeFrustumLines(modelview_matrix);
  
    glutSwapBuffers();
    glutPostRedisplay();  
}

//Reshapes the first person window
void reshape(GLint w, GLint h)
{
    if (h > 0)
    {
        window.height = h;
        window.width = w;
    }
}

//Reshapes the third person window, independently of the first.
void reshape2(GLint w, GLint h)
{
    if (h > 0)
    {
        window2.height = h;
        window2.width = w;
    }
}

//Takes in keyboard commands and performs the appropriate task on both screens simultaneously.
void keyboard(unsigned char c, int x, int y)
{
    switch (c)
    {
	//q, x, and ESC closes the program
    case 'q':
	case 'x':
    case 27:
        glutLeaveMainLoop();
        return;
	//w toggles between wireframe and solid modes
    case 'w':
        window.wireframe = !window.wireframe;
        break;
    //e incrementally elevates the seat up. It can not exceed the set maximum bound.
	case 'e':
        if(adjustableSeat<0.75){
            adjustableSeat+=0.01f;
        }          
        return;
    //d incrementally descends the seat. It can not exceed the set minimum bound.
	case 'd':
        if(adjustableSeat>0){
            adjustableSeat-=0.01f;
        }
        if(adjustableSeat<=0){
            adjustableSeat = 0;
        }
        return;
	//f toggles the fog between on and off.
	case 'f':
		if(!fogOn){
			fogOn = true;
		}
		else{
			fogOn = false;
		}
    }  
	/* From the attempt of creating a moving reference point for the camera.
	case 'i':
		position.x++;
	case 'j':
		position.z--;
	case 'k':
		position.x--;
	case 'l':
		position.z++;
	case 'u':
		position.y++;
	case 'n':
		position.y--;
		*/
}

//Takes in keyboard arrow input and moves the camera accordingly in 1 degree increments.
void spec(int c, int x, int y)
{
    switch(c){
        case GLUT_KEY_UP:
            rotateX(1);
            return;
        case GLUT_KEY_DOWN:
            rotateX(-1);
            return;
        case GLUT_KEY_RIGHT:
            rotateY(-1);
            return;
        case GLUT_KEY_LEFT:
            rotateY(1);
            return;
    }
}

//Produces the two windows and displays while interpreting keyboard input and window
//size changing. 
int main(int argc, char * argv[])
{
    glutInit(&argc , argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(1400 ,750);//window.widthwindow.height
    window.handle = glutCreateWindow("Stool Camera View");
    glutPositionWindow(1, 1);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(spec);

/*	glutInitWindowSize(window2.width , window2.height);
    window2.handle = glutCreateWindow("Third Person View");
    glutPositionWindow(1, 351);
    glutDisplayFunc(display2);
    glutReshapeFunc(reshape2);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(spec);*/

    glutMainLoop();
}

/*
This was my first attempt at making the display2 method. I initially tried to create the third person view
from scratch. This attempt started out with some success, but became nearly impossible when I tried to 
display the frustum. Therefore I had to abandon this attempt and take a different approach by using the 
display from the first person and making a few modelview translations for positioning, which proved to
be much more efficient.

void display2()
{
    ////////Lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat whiteSpecularLight[] = {0.1, 0.1, 0.1};
    GLfloat whiteAmbientLight[] = {0.65, 0.65, 0.65};
    GLfloat whiteDiffuseLight[] = {0.15, 0.15, 0.15};

    GLfloat whiteSpecularMaterial[] = {0.5, 0.5, 0.5};
    GLfloat mShininess[] = {0.5, 0.5, 0.5};

    glLightfv(GL_LIGHT0, GL_SPECULAR, whiteSpecularLight);
    glLightfv(GL_LIGHT0, GL_AMBIENT, whiteAmbientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuseLight);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecularMaterial);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);

    ///////Projection
    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, window.wireframe ? GL_LINE : GL_FILL);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glm::mat4 projection_matrix = glm::perspective(45.0f, float(window.width) / float(window.height), 0.1f, 500.0f);
    glLoadMatrixf(glm::value_ptr(projection_matrix));
    glViewport(0, 0, window.width, window.height);
    glMatrixMode(GL_MODELVIEW);
    glm::mat4 modelview_matrix;

  
    //modelview_matrix = render(modelview_matrix);
    //glLoadMatrixf(glm::value_ptr(modelview_matrix));

    modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, -3.0f, -20.0f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
    makeCoordinateAxes(3, 9.4);
    xyGrid();
    stool(modelview_matrix, 2, 2);
    stool(modelview_matrix, 2, -2);
    stool(modelview_matrix, -2, 2);

    modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 3.0f, 20.0f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
    modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 0.0f, -7.0f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
    //modelview_matrix = glm::rotate(modelview_matrix, (GLfloat) 90, glm::vec3(1,0,0));
    modelview_matrix = glm::rotate(modelview_matrix, (GLfloat) 270, glm::vec3(0,1,0));
    //modelview_matrix = glm::rotate(modelview_matrix, (GLfloat) 90, glm::vec3(0,0,1));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
    gluLookAt(0,0,0,0,0,0,0,0,0);
    modelview_matrix = render(modelview_matrix);
    gluLookAt(0,0,0, 0,0,0, 0,1,0);
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
    makeCoordinateAxes(1.2, 2);
    glColor3f(0.5, 0.5, 0.5);
    //modelview_matrix = glm::rotate(modelview_matrix, (GLfloat) 0, glm::vec3(0,1,0));
    modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 0.0f, -1.0f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
    glutSolidCone(0.5, 1, 50, 50);
    modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 0.0f, 1.0f));
    //modelview_matrix = glm::rotate(modelview_matrix, (GLfloat) 0, glm::vec3(0,1,0));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
    glutSolidCube(1);
    //modelview_matrix = glm::rotate(modelview_matrix, (GLfloat) -90, glm::vec3(0,0,1));
    modelview_matrix = glm::rotate(modelview_matrix, (GLfloat) -90, glm::vec3(0,1,0));
    //modelview_matrix = glm::rotate(modelview_matrix, (GLfloat) -90, glm::vec3(1,0,0));
    modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 0.0f, 7.5f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
  
    modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 0.0f, -7.5f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));

  

    /*modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 3.0f, 13.0f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
    modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 0.0f, 7.5f));
    modelview_matrix = render(modelview_matrix);
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
    glColor3f(0.5, 0.5, 0.5);
    glutSolidCube(1);
    modelview_matrix = glm::translate(modelview_matrix, glm::vec3(0.0f, 0.0f, -7.5f));
    glLoadMatrixf(glm::value_ptr(modelview_matrix));
    modelview_matrix = render(modelview_matrix);
    glLoadMatrixf(glm::value_ptr(modelview_matrix));*/
    /*
    glutSwapBuffers();
    glutPostRedisplay();  
}
*/