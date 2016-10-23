#include "ImageQuilter.h"

ImageQuilter::ImageQuilter(const unsigned& w, const unsigned& h, const unsigned& ss)
	: width{w}, height{h}, synthesis_size{ss}, image_square(0, 0, 1, 1)
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
	combineshader = loadShaders("shaders/plaintextureshader.vert", "shaders/combineshader.frag");  // renders with light (used for initial renderin of teapot)


	//initialize FBOs
	fbo_texture = initFBO(width, height, 0);
	fbo_final = initFBO(width, height, 0);
	fbo_patch = initFBO(width, height, 0);
	
	//temp
	fbo1 = initFBO(width, height, 0);
	fbo2 = initFBO(width, height, 0);
	fbo3 = initFBO(width, height, 0);
	fbo4 = initFBO(width, height, 0);

	//load texture
	LoadTGATextureSimple("images/uppochner.tga", &tex);

	create_image();
}

void ImageQuilter::create_image()
{
	fbo_texture->texid = tex;

	int temp = 0;
 	//vector<unique_ptr<FBOstruct>> patches;
 	float w_count = width/synthesis_size;
	float h_count = height/synthesis_size;
	for(unsigned i = 0; i < w_count; i++) {
		for(unsigned j = 0; j < h_count; j++) {
			
			//draw a patch of texture to current fbo patch
			useFBO(fbo_patch, fbo_texture, 0L);

			//create the square to draw our patch from
			Square s(
				rng(width*(1-1/w_count))/(float)width,	 // x
				rng(height*(1-1/w_count))/(float)height, // y
				1/w_count,								 // width
				1/h_count);								 // height			
			s.set_position(i/w_count, j/h_count); // fix position for ping-ponging

			//perform drawing
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(plaintextureshader);
			glClearColor(0.0, 0.0, 0.0, 0);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			DrawModel(s.get(), plaintextureshader, "in_Position", NULL, "in_TexCoord");

			if(temp++ == 0) {
				useFBO(fbo1, fbo_patch, 0L);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glUseProgram(plaintextureshader);
				glDisable(GL_CULL_FACE);
				glDisable(GL_DEPTH_TEST);
				DrawModel(image_square.get(), plaintextureshader, "in_Position", NULL, "in_TexCoord");
			}
			else {
				//ping-pong between fbo1 and fbo2
				if(temp % 2 == 0)
					useFBO(fbo2, fbo1, fbo_patch);
				else 
					useFBO(fbo1, fbo2, fbo_patch);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glUseProgram(combineshader);
				glUniform1i(glGetUniformLocation(combineshader, "texUnit"), 0);
				glUniform1i(glGetUniformLocation(combineshader, "texUnit2"), 1);
				DrawModel(image_square.get(), combineshader, "in_Position", NULL, "in_TexCoord");
			}

			//patches.push_back(move(fbo_patch));
		}
	}

	if(temp % 2 == 0)
		useFBO(fbo_final, fbo2, 0L);
	else 
		useFBO(fbo_final, fbo1, 0L);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(plaintextureshader);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	DrawModel(image_square.get(), plaintextureshader, "in_Position", NULL, "in_TexCoord");

	//cout << "patches size = " << patches.size() << endl;
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
	DrawModel(image_square.get(), plaintextureshader, "in_Position", NULL, "in_TexCoord");

	glutSwapBuffers();
}

void ImageQuilter::working_temp()
{
	useFBO(fbo1, fbo_texture, 0L);
	
	Square s(
		rng(width*0.8)/(float)width,		// x
		rng(height*0.8)/(float)height,	// y
		0.2,							// width
		0.2);							// height			
	s.set_position(0.0, 0.8);

	glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(plaintextureshader);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	DrawModel(s.get(), plaintextureshader, "in_Position", NULL, "in_TexCoord");


	useFBO(fbo2, fbo_texture, 0L);
	
	Square s2(
		rng(width*0.8)/(float)width,		// x
		rng(height*0.8)/(float)height,	// y
		0.2,							// width
		0.2);							// height			
	s2.set_position(0.25, 0.8);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(plaintextureshader);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	DrawModel(s2.get(), plaintextureshader, "in_Position", NULL, "in_TexCoord");

	useFBO(fbo3, fbo_texture, 0L);
	
	Square s3(
		rng(width*0.8)/(float)width,		// x
		rng(height*0.8)/(float)height,	// y
		0.2,							// width
		0.2);							// height			
	s3.set_position(0.5, 0.8);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(plaintextureshader);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	DrawModel(s3.get(), plaintextureshader, "in_Position", NULL, "in_TexCoord");


	useFBO(fbo4, fbo1, fbo2);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(combineshader);
	glUniform1i(glGetUniformLocation(combineshader, "texUnit"), 0);
	glUniform1i(glGetUniformLocation(combineshader, "texUnit2"), 1);
	DrawModel(image_square.get(), combineshader, "in_Position", NULL, "in_TexCoord");


	useFBO(fbo_final, fbo4, fbo3);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(combineshader);
	glUniform1i(glGetUniformLocation(combineshader, "texUnit"), 0);
	glUniform1i(glGetUniformLocation(combineshader, "texUnit2"), 1);
	DrawModel(image_square.get(), combineshader, "in_Position", NULL, "in_TexCoord");
}