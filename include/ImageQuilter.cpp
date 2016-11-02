#include "ImageQuilter.h"

ImageQuilter::ImageQuilter(const unsigned& w, const unsigned& h, const unsigned& ss, TextureData t)
	: width{w}, height{h}, synthesis_size{ss}, tex{t}, image_square(0, 0, 1, 1)
	//w_count_out{(float)(width/synthesis_size)}, h_count_out{(float)(height/synthesis_size)},
	//w_count_in{(float)(tex.width/synthesis_size)}, h_count_in{(float)(tex.width/synthesis_size)},
	//pix_w{1/(float)width}, pix_h{1/(float)height},
	//overlap_w{(float)(synthesis_size/6.0 * pix_w)}, overlap_h{(float)(synthesis_size/6.0 * pix_h)},
	//x_width{1/w_count_in}, y_height{1/h_count_in},
	//overlap_w_out{overlap_w * tex.width / width}, overlap_h_out{overlap_h * tex.height / height},
	//patch_w_out{1/w_count_out + 2 * overlap_w_out}, patch_h_out{1/h_count_out + 2 * overlap_h_out}

{
	dumpInfo();  // shader info

	// GL inits
	glClearColor(0.0, 0.0, 0.0, 0);
	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	//Initialize a bunch of helper variables
	w_count_out = width/synthesis_size; // amount of patches that fit on out texture width
	h_count_out = height/synthesis_size; // amount of patches that fit on out texture height
	w_count_in = tex.width/synthesis_size; // amount of patches that fit on in texture width
	h_count_in = tex.height/synthesis_size; // amount of patches that fit on in texture width
	pix_w = 1/(float)width;  // width of one pixel
	pix_h = 1/(float)height; // height of one pixel
	overlap_w = synthesis_size/6.0 * pix_w;  // amount of width pixels that overlap each other
	overlap_h = synthesis_size/6.0 * pix_h;  // amount of height pixels that overlap each other
	x_width = 1/w_count_in;  // width of a patch (without the overlap)
	y_height = 1/h_count_in; // height of a patch (without the overlap)
	overlap_w_out = overlap_w * tex.width / width; // size of overlap width in out texture coordinates
	overlap_h_out = overlap_h * tex.height / height; // size of overlap height in out texture coordinates
	patch_w_out = 1/w_count_out + 2 * overlap_w_out; // size of patch width in out texture coordinates
	patch_h_out = 1/h_count_out + 2 * overlap_h_out; // size of patch height in out texture coordinates

	// Load and compile shaders
	plaintextureshader = loadShaders("shaders/plaintextureshader.vert", "shaders/plaintextureshader.frag");
	combineshader = loadShaders("shaders/plaintextureshader.vert", "shaders/combineshader.frag");
	transparencyshader = loadShaders("shaders/plaintextureshader.vert", "shaders/transparencyshader.frag");
	minerrorshader = loadShaders("shaders/plaintextureshader.vert", "shaders/minerrorshader.frag");
	translateshader = loadShaders("shaders/translateshader.vert", "shaders/plaintextureshader.frag");


	//initialize FBOs
	fbo_texture = initFBO(tex.width, tex.height, 0);
	fbo_final = initFBO(width, height, 0);
	fbo_patch = initFBO(width, height, 0);
	fbo1 = initFBO(width, height, 0);
	fbo2 = initFBO(width, height, 0);
	
	fbo_test = initFBO(width, height, 0);
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

	if(shader == combineshader || shader == minerrorshader) {
		glUniform1i(glGetUniformLocation(combineshader, "texUnit"), 0);
		glUniform1i(glGetUniformLocation(combineshader, "texUnit2"), 1);
	}

	DrawModel(image_square.get(), shader, (char*)"in_Position", NULL, (char*)"in_TexCoord");
}

void ImageQuilter::draw_fbo_translated(FBOstruct *out, FBOstruct *in, GLfloat x, GLfloat y)
{
	useFBO(out, in, 0L);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(translateshader);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	mat4 transMatrix = T(x, y, 0.0);
	glUniformMatrix4fv(glGetUniformLocation(translateshader, "mat"), 1, GL_TRUE, transMatrix.m);
	DrawModel(image_square.get(), translateshader, (char*)"in_Position", NULL, (char*)"in_TexCoord");
}