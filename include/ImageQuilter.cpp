#include "ImageQuilter.h"

ImageQuilter::ImageQuilter(const unsigned& w, const unsigned& h, const unsigned& ss, TextureData t)
	: width{w}, height{h}, synthesis_size{ss}, tex{t}, image_square(0, 0, 1, 1)
{
	dumpInfo();  // shader info

	// GL inits
	glClearColor(0.0, 0.0, 0.0, 0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	// Load and compile shaders
	plaintextureshader = loadShaders("shaders/plaintextureshader.vert", "shaders/plaintextureshader.frag");
	combineshader = loadShaders("shaders/plaintextureshader.vert", "shaders/combineshader.frag");
	transparencyshader = loadShaders("shaders/plaintextureshader.vert", "shaders/transparencyshader.frag");
	minerrorshader = loadShaders("shaders/plaintextureshader.vert", "shaders/minerrorshader.frag");

	//initialize FBOs
	fbo_texture = initFBO(tex.width, tex.height, 0);
	fbo_final = initFBO(width, height, 0);
	fbo_patch = initFBO(width, height, 0);

	//temp
	fbo1 = initFBO(width, height, 0);
	fbo2 = initFBO(width, height, 0);
	fbo3 = initFBO(width, height, 0);
	fbo4 = initFBO(width, height, 0);

	//create_image_random();
	create_image_neighbor();
	//create_image_minerror();
}

void ImageQuilter::render()
{
	//Draw the final image generated from the create_image() method
	glFlush(); // Can cause flickering on some systems. Can also be necessary to make drawing complete.
	useFBO(0L, fbo_final, 0L);
	glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Activate second shader program
	glUseProgram(plaintextureshader);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	DrawModel(image_square.get(), plaintextureshader, (char*)"in_Position", NULL, (char*)"in_TexCoord");

	glutSwapBuffers();
}


// code taken from http://www.flashbang.se/archives/155
void ImageQuilter::saveImage()
{
	int x = fbo_final->width;
	int y = fbo_final->height;
	const char* filename = "output.tga";

	// get the image data
	long imageSize = x * y * 3;
	unsigned char *data = new unsigned char[imageSize];
	glReadPixels(0,0,x,y, GL_BGR,GL_UNSIGNED_BYTE,data);// split x and y sizes into bytes
	int xa= x % 256;
	int xb= (x-xa)/256;int ya= y % 256;
	int yb= (y-ya)/256;//assemble the header
	unsigned char header[18]={0,0,2,0,0,0,0,0,0,0,0,0,(unsigned char)xa,(unsigned char)xb,(unsigned char)ya,(unsigned char)yb,24,0};
	// write header and data to file
	fstream File(filename, ios::out | ios::binary);
	File.write (reinterpret_cast<char *>(header), sizeof (char)*18);
	File.write (reinterpret_cast<char *>(data), sizeof (char)*imageSize);
	File.close();

	delete[] data;
	data=NULL;
}

void ImageQuilter::draw_fbo(FBOstruct *out, FBOstruct *in1, FBOstruct *in2, GLuint& shader)
{
	useFBO(out, in1, in2);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shader);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	if(shader == combineshader) {
		glUniform1i(glGetUniformLocation(combineshader, "texUnit"), 0);
		glUniform1i(glGetUniformLocation(combineshader, "texUnit2"), 1);
	}

	DrawModel(image_square.get(), shader, (char*)"in_Position", NULL, (char*)"in_TexCoord");
}


void ImageQuilter::draw_fbo_minerror(FBOstruct *out, FBOstruct *in1, FBOstruct *in2)
{
	useFBO(out, in1, in2);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(minerrorshader);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	DrawModel(image_square.get(), minerrorshader, (char*)"in_Position", NULL, (char*)"in_TexCoord");
}

void ImageQuilter::draw_square_fbo(Square *s, FBOstruct *out, FBOstruct *in1)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(plaintextureshader);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	DrawModel(s->get(), plaintextureshader, (char*)"in_Position", NULL, (char*)"in_TexCoord");
}