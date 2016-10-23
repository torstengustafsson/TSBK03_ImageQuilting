#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <memory>

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


#include "Square.h"
#include "RandomGenerator.h"

using namespace std;

class ImageQuilter
{
public:
	ImageQuilter(const unsigned& w, const unsigned& h, const unsigned& ss, TextureData t);

	void create_image();

	void render();

private:
	const unsigned width, height;
	const unsigned synthesis_size;

	RandomGenerator rng;

	GLuint phongshader = 0, plaintextureshader = 0, combineshader = 0;
	TextureData tex;
	FBOstruct *fbo_texture, *fbo_final, *fbo_patch;
	FBOstruct *fbo1, *fbo2, *fbo3, *fbo4;

	Square image_square;
};