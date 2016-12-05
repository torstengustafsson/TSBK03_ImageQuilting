#include <iostream>
#include <chrono>	//TODO only used for testing, might want to remove when complete
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
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

	struct patch_data {
		FBOstruct *fbo;
		Square *s;
		ImageQuilter& parent;
		
		patch_data(FBOstruct* _fbo, ImageQuilter& _p) : parent{_p} 
		{
			fbo = initFBO(_fbo->width, _fbo->height, 0); // TODO: this is horribly slow and uses too much GPU memory
			parent.draw_fbo(fbo, _fbo, 0L, parent.plaintextureshader);
			//s = new Square(*(_s));
		}

		patch_data& operator=(const patch_data& pd) {
			fbo = pd.fbo;
			s = pd.s;
			return *this;
		}
	};

	GLuint plaintextureshader = 0, combineshader = 0, transparencyshader = 0, overlapshader = 0,
		   minerrorshader = 0, translateshader = 0;
	TextureData tex;
	FBOstruct *fbo_texture, *fbo_final, *fbo_patch;
	FBOstruct *fbo1, *fbo2; // ping-pong FBOs
	FBOstruct *fbo_work; // used for various purposes

	Square image_square;

	void draw_fbo(FBOstruct *out, FBOstruct *in1, FBOstruct *in2, GLuint& shader);
	void draw_fbo_translated(FBOstruct *out, FBOstruct *in, GLfloat x, GLfloat y);

	//Used by minerror method
	const int amount_patches = 40; // higher value should give better images, but slows down program linearly
	vector<patch_data> patches;
	void generate_patches();
	float calc_minerror(patch_data& in1, patch_data& in2, bool side);

	void print_values();

	float w_count_in;
	float h_count_in;
	float pix_w_in; // width of one pixel
	float pix_h_in; // height of one pixel
	float overlap_w_in;  // amount of width pixels that overlap each other
	float overlap_h_in;  // amount of height pixels that overlap each other
	float patch_w_in;  // width of a patch (without the overlap)
	float patch_h_in; // height of a patch (without the overlap)

	//convert to final image coordinates
	float w_count_out;
	float h_count_out;
	float pix_w_out;
	float pix_h_out;
	float overlap_w_out;
	float overlap_h_out;
	float patch_w_out;
	float patch_h_out;
};
