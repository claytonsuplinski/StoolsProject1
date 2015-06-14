#include <iostream>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

using namespace std;

//Creates a stool object.
class Stool{
private:
	//Used to position components of the stool in the proper locations
	glm::mat4 translate(glm::mat4 Model, GLfloat x, GLfloat y, GLfloat z);
	//Creates one leg of the stool
	void makeLeg();
public:
	//Controls the general construction of the stool
	void stools(glm::mat4 Model, GLfloat x, GLfloat z);
};