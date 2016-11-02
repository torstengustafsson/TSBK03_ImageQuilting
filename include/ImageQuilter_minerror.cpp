#include "ImageQuilter.h"

void ImageQuilter::create_image_minerror()
{
	fbo_texture->texid = tex.texID;

	int count = 0;

	vector<patch_data> patches;

	//pick out 2 times the minimum neccessary amount of random patches to fill image
	for(int k = 0; k < 2*w_count_out*h_count_out; k++) {
		float x_pos = rng(width*(1.0-1.0/w_count_in)) * pix_w;
		float y_pos = rng(height*(1.0-1.0/h_count_in)) * pix_w;
		Square s(x_pos - overlap_w, y_pos - overlap_h, x_width + 2 * overlap_w, y_height + 2 * overlap_h);
		
		//Draw each potential patch in the lower left corner
		s.set_position(0, 0, 1/w_count_out + 2 * overlap_w_out, 1/h_count_out + 2 * overlap_h_out);

		FBOstruct* fbo_temp = initFBO(width, height, 0);
		useFBO(fbo_temp, fbo_texture, 0L);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(transparencyshader);
		glClearColor(0.0, 0.0, 0.0, 0);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glUniform1f(glGetUniformLocation(transparencyshader, "coord_l1"), x_pos - overlap_w);
		glUniform1f(glGetUniformLocation(transparencyshader, "coord_l2"), x_pos + overlap_w);
		glUniform1f(glGetUniformLocation(transparencyshader, "coord_r1"), x_pos + x_width - overlap_w);
		glUniform1f(glGetUniformLocation(transparencyshader, "coord_r2"), x_pos + x_width + overlap_w);
		glUniform1f(glGetUniformLocation(transparencyshader, "coord_u1"), y_pos + y_height - overlap_h);
		glUniform1f(glGetUniformLocation(transparencyshader, "coord_u2"), y_pos + y_height + overlap_h);
		glUniform1f(glGetUniformLocation(transparencyshader, "coord_d1"), y_pos - overlap_h);
		glUniform1f(glGetUniformLocation(transparencyshader, "coord_d2"), y_pos + overlap_h);
		DrawModel(s.get(), transparencyshader, (char*)"in_Position", NULL, (char*)"in_TexCoord");

		patches.push_back( patch_data(fbo_temp, new Square(s), *this) );
	}

	//index reperesent the coordinates for patch positions
	const unsigned w_dim = static_cast<int>(w_count_out);
	const unsigned h_dim = static_cast<int>(h_count_out);
	
	//completed contains translated final patch while working patch only contains final patch
	patch_data* completed_patches[w_dim][h_dim];
	patch_data* working_patches[w_dim][h_dim];

	for(unsigned i = 0; i < w_count_out; i++) {
		for(unsigned j = 0; j < h_count_out; j++) {
			
			//calculate best-fitting patch
			float best_fit = FLT_MAX;
			int patch_index = 0;
			for(unsigned p = 0; p < patches.size(); p++) {
				float left_fit = 0;
				float down_fit = 0;
				if(i > 0) // left side check
					left_fit = calc_minerror(patches[p], *working_patches[i-1][j], false);
				if(j > 0) // down side check
					down_fit = calc_minerror(patches[p], *completed_patches[i][j-1], true);

				if(left_fit+down_fit < best_fit) {
					best_fit = left_fit+down_fit;
					patch_index = p;
				}
			}
			completed_patches[i][j] = new patch_data(patches[patch_index]);
			working_patches[i][j] = new patch_data(patches[patch_index]);
			patches.push_back( patch_data(patches[patch_index]) );
			patches.erase(patches.begin() + patch_index);
			patches.erase(patches.begin() + patch_index);
			draw_fbo_translated(fbo_patch, completed_patches[i][j]->fbo, i/w_count_in - overlap_w, j/h_count_in - overlap_h);
			draw_fbo(completed_patches[i][j]->fbo, fbo_patch, 0L, plaintextureshader);


			if(count++ == 0) {
				//fill fbo1 with first patch during first iteration
				draw_fbo(fbo1, completed_patches[0][0]->fbo, 0L, plaintextureshader);
			}
			else {
				//ping-pong between fbo1 and fbo2
				bool even = (count % 2 == 0);
				draw_fbo((even ? fbo2 :  fbo1), (even ? fbo1 :  fbo2), completed_patches[i][j]->fbo, combineshader);
			}
		}
	}

	useFBO(fbo_final, (count % 2 == 0) ? fbo2 : fbo1, 0L);
	//useFBO(fbo_final, fbo_test, 0L);

	//draw the final image
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(plaintextureshader);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	DrawModel(image_square.get(), plaintextureshader, (char*)"in_Position", NULL, (char*)"in_TexCoord");
}

float ImageQuilter::calc_minerror(patch_data in1, patch_data in2, bool side)
{
	//FBOstruct *fbo_minerror = initFBO(width, height, 0);
	useFBO(fbo_test, in1.fbo, in2.fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(minerrorshader);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	//in1.s->move_texture(0, 0, overlap_w_out, 1/h_count_out + 2 * overlap_h_out); //takes the left side of the patch
	
	glUniform1i(glGetUniformLocation(minerrorshader, "texUnit1"), 0);
	glUniform1i(glGetUniformLocation(minerrorshader, "texUnit2"), 1);
	
	glUniform1f(glGetUniformLocation(minerrorshader, "width"), patch_w_out);
	glUniform1f(glGetUniformLocation(minerrorshader, "height"), patch_h_out);
	glUniform1f(glGetUniformLocation(minerrorshader, "offset_w"), overlap_w);
	glUniform1f(glGetUniformLocation(minerrorshader, "offset_h"), overlap_h);
	glUniform1i(glGetUniformLocation(minerrorshader, "side"), 1);
	
	DrawModel(image_square.get(), minerrorshader, (char*)"in_Position", NULL, (char*)"in_TexCoord");

	//TODO: should read one side at a time
	int arr_x = fbo_test->width;
	int arr_y = fbo_test->height;

	GLfloat array[3*arr_x*arr_y];
	glReadPixels(0, 0, arr_x, arr_y, GL_RGB,GL_FLOAT, array);

	//calculate the fitness value of the current overlap
	float fitness = 0;
	for(int i = 0; i < 3*arr_x*arr_y; i++) {
		fitness += array[i];
	}

	return fitness;
}