#include <iostream>
#include <math.h>
#include <stdlib.h>

#ifdef __APPLE__
// Mac
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// uses framework Cocoa
#else
	#ifdef WIN32
// MS
		#include <windows.h>
		#include <stdio.h>
		#include <GL/glew.h>
		#include <GL/glut.h>
	#else
// Linux
		#include <stdio.h>
		#include <GL/gl.h>
		#include "MicroGlut.h"
//		#include <GL/glut.h>
	#endif
#endif

#include "LoadTGA.h"
#include "VectorUtils3.h"
#include "GL_utilities.h"
#include "loadobj.h"
#include "zpr.h"


#include<vector>

using namespace std;

class Square
{
public:
	Square(const float x, const float y, const float w, const float h);

	void set_position(const float x, const float y, const float w = 0, const float h = 0);
	Model* get();

private:
	GLfloat square[12];
	GLfloat squareTexCoord[8];
	GLuint squareIndices[6] = {0, 1, 2, 0, 2, 3};

	float width, height;

	Model* squareModel;
};