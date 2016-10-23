#include "ImageQuilter.h"

ImageQuilter::ImageQuilter(const unsigned& w, const unsigned& h, const unsigned& ss) 
	: width{w}, height{h}, synthesis_size{ss}, image_square(0, 0, 1, 1)
{
	dumpInfo();  // shader info

	// GL inits
	glClearColor(0.1, 0.1, 0.3, 0);
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
	
	//temp
	fbo1 = initFBO(width, height, 0);
	fbo2 = initFBO(width, height, 0);
	fbo3 = initFBO(width, height, 0);
	fbo4 = initFBO(width, height, 0);

	//load texture (how does this work though?)
	LoadTGATextureSimple("images/uppochner.tga", &tex);

	create_image();
}

void ImageQuilter::create_image()
{
	fbo_texture->texid = tex;

 	vector<unique_ptr<FBOstruct>> patches;
 	unsigned w_count = width/synthesis_size;
	unsigned h_count = height/synthesis_size;
	for(unsigned i = 0; i < w_count*h_count; i++) {
		unique_ptr<FBOstruct> fbo_patch = unique_ptr<FBOstruct>(initFBO(width, height, 0));
		
		//draw a patch of texture to current fbo patch
		useFBO(fbo_patch.get(), fbo_texture, 0L);
		Square s(0.0, 0.0, 0.5, 0.5);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(plaintextureshader);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		DrawModel(s.get(), plaintextureshader, "in_Position", NULL, "in_TexCoord");

		patches.push_back(move(fbo_patch));
	}

	//useFBO(fbo_final, patches[0].get(), patches[1].get());
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glUseProgram(combineshader);

	//glUniform1i(glGetUniformLocation(combineshader, "texUnit"), 0);
	//glUniform1i(glGetUniformLocation(combineshader, "texUnit2"), 1);

	//DrawModel(image_square.get(), combineshader, "in_Position", NULL, "in_TexCoord");

	cout << "patches size = " << patches.size() << endl;
}

void ImageQuilter::render()
{
	/*
	unsigned w_count = width/synthesis_size;
	unsigned h_count = height/synthesis_size;
	for(unsigned i = w_count-1; i > 0; i--) {
		for(unsigned j = h_count-1; j > 0; j--) {
			if(i % 2 == 0)
				useFBO(fbo2, fbo1, 0L);
			else
				useFBO(fbo1, fbo2, 0L);
			
			Square s(i/w_count, j/h_count, i/w_count, j/h_count);

			glClearColor(0.0, 0.0, 0.0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			DrawModel(s.get(), plaintextureshader, "in_Position", NULL, "in_TexCoord");

			if(i % 2 == 0)
				useFBO(fbo3, fbo2, 0L);
			else
				useFBO(fbo3, fbo1, 0L);

			glClearColor(0.0, 0.0, 0.0, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			DrawModel(image_square.get(), plaintextureshader, "in_Position", NULL, "in_TexCoord");
		}
	}
	*/

	//temporary hardcoded stuff that renders patches to fbo_final
 	working_temp();

	
	//Draw the final image
//	glFlush(); // Can cause flickering on some systems. Can also be necessary to make drawing complete.
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
	Square s(0, 0, 0.5, 0.5);
	s.set_position(0.5, 0.3);
	
	glClearColor(0.0, 0.0, 0.0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(plaintextureshader);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	DrawModel(s.get(), plaintextureshader, "in_Position", NULL, "in_TexCoord");


	useFBO(fbo2, fbo_texture, 0L);
	Square s2(0.2, 0.4, 0.5, 0.5);
	s2.set_position(0.0, 0.5);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(plaintextureshader);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	DrawModel(s2.get(), plaintextureshader, "in_Position", NULL, "in_TexCoord");

	useFBO(fbo3, fbo_texture, 0L);
	Square s3(0.5, 0.5, 0.3, 0.3);
	s3.set_position(0.2, 0.0);
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