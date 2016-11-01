#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <memory>
#include <fstream>

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
#include "float.h"


#include "Square.h"
#include "RandomGenerator.h"

using namespace std;

class ImageQuilter
{
public:
	ImageQuilter(const unsigned& w, const unsigned& h, const unsigned& ss, TextureData t);

	void create_image_random();
	void create_image_neighbor();
	void create_image_minerror();

	void render();

	void saveImage();

private:
	const unsigned width, height;
	const unsigned synthesis_size;

	RandomGenerator rng;

	GLuint plaintextureshader = 0, combineshader = 0, transparencyshader = 0, minerrorshader = 0;
	TextureData tex;
	FBOstruct *fbo_texture, *fbo_final, *fbo_patch;
	FBOstruct *fbo1, *fbo2, *fbo3, *fbo4;

	Square image_square;

	void draw_fbo(FBOstruct *out, FBOstruct *in1, FBOstruct *in2, GLuint& shader);
	void draw_fbo_minerror(FBOstruct *out, FBOstruct *in1, FBOstruct *in2);
	void draw_square_fbo(Square *s, FBOstruct *out, FBOstruct *in1);

	struct patch_data {
		FBOstruct *fbo;
		Square* s;
		patch_data(FBOstruct* _fbo, Square* _s) : fbo{_fbo}, s{_s} {}
		patch_data() {}
		patch_data& operator=(const patch_data& pd) {
			fbo = pd.fbo;
			s = pd.s;
			return *this;
		}
	};
};
