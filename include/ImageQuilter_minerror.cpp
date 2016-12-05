#include "ImageQuilter.h"


void ImageQuilter::create_image_minerror()
{
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	fbo_texture->texid = tex.texID;

	int count = 0;

	//index reperesent the coordinates for patch positions
	const unsigned w_dim = static_cast<int>(w_count_out);
	const unsigned h_dim = static_cast<int>(h_count_out);
	
	//Store final patches positioned in the corner so we can work with them
	patch_data* working_patches[w_dim*h_dim];

	for(unsigned i = 0; i < w_count_out; i++) {
		for(unsigned j = 0; j < h_count_out; j++) {

			//calculate best-fitting patch
			float best_fit = FLT_MAX;
			int patch_index = 0;
			generate_patches(); // generate new patches to the patches vector
			for(unsigned p = 0; p < patches.size(); p++) {
				float left_fit = 0;
				float down_fit = 0;
				if(i > 0) // left side check
					left_fit = calc_minerror(patches[p], *working_patches[(i-1)*4+j], false);
				if(j > 0) // down side check
					down_fit = calc_minerror(patches[p], *working_patches[i*4+(j-1)], true);

				if(pow(left_fit,2)+pow(down_fit,2) < best_fit) {
					best_fit = pow(left_fit,2)+pow(down_fit,2);
					patch_index = p;
				}
			}

			draw_fbo(fbo_work, patches[patch_index].fbo, 0L, plaintextureshader);
			working_patches[i*4+j] = new patch_data(patches[patch_index]);
			draw_fbo_translated(fbo_patch, fbo_work, i/w_count_out - overlap_w_out, j/h_count_out - overlap_h_out);

			//Render the completed patch with transparency
			useFBO(fbo_work, fbo_patch, 0L);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(transparencyshader);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

			float temp = synthesis_size < 32 ? 0 : pix_w_out; // prevents a weird bug where one pixel per patch gets outside of bounds with certain values
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_l1"), i/w_count_out - overlap_w_out);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_l2"), i/w_count_out + overlap_w_out + temp);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_r1"), (i+1)/w_count_out - overlap_w_out);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_r2"), (i+1)/w_count_out + overlap_w_out + temp);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_d1"), j/h_count_out - overlap_h_out);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_d2"), j/h_count_out + overlap_h_out + temp);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_u1"), (j+1)/h_count_out - overlap_h_out);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_u2"), (j+1)/h_count_out + overlap_h_out + temp);

			DrawModel(image_square.get(), transparencyshader, (char*)"in_Position", NULL, (char*)"in_TexCoord");
			//draw_fbo(fbo_work, fbo_patch, 0L, plaintextureshader);

			if(count++ == 0) {
				//fill fbo1 with first patch during first iteration
				draw_fbo(fbo1, fbo_work, 0L, plaintextureshader);
			}
			else {
				//ping-pong between fbo1 and fbo2
				bool even = (count % 2 == 0);
				draw_fbo((even ? fbo2 :  fbo1), (even ? fbo1 :  fbo2), fbo_work, combineshader);
			}

			//Draw the finished parts of the scene
			useFBO(fbo_final, (count % 2 == 0) ? fbo2 : fbo1, 0L);
			//useFBO(fbo_final, patches[patch_index].fbo, 0L);
			glUseProgram(plaintextureshader);
			glDisable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			DrawModel(image_square.get(), plaintextureshader, (char*)"in_Position", NULL, (char*)"in_TexCoord");
			render();
		}
	}

	useFBO(fbo_final, (count % 2 == 0) ? fbo2 : fbo1, 0L);
	//useFBO(fbo_final, fbo_texture, 0L);
	//draw_fbo(fbo1, completed_patches[1*4+0]->fbo, completed_patches[0*4+1]->fbo, combineshader);
	//draw_fbo(fbo2, fbo1, completed_patches[1*4+1]->fbo, combineshader);
	//useFBO(fbo_final, fbo2, 0L);

	//draw the final image
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(plaintextureshader);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	DrawModel(image_square.get(), plaintextureshader, (char*)"in_Position", NULL, (char*)"in_TexCoord");

	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	cout << "time elapsed: " << elapsed_seconds.count() << endl;
}

void ImageQuilter::generate_patches()
{
	glUseProgram(plaintextureshader);
	for(patch_data& p : patches) {
		float x_pos = overlap_w_in + rng(tex.width*(1.0-2*overlap_w_in-1.0/w_count_in)) * pix_w_in;
		float y_pos = overlap_h_in + rng(tex.height*(1.0-2*overlap_h_in-1.0/h_count_in)) * pix_w_in;
		Square s(x_pos - overlap_w_in, y_pos - overlap_h_in, patch_w_in + 2 * overlap_w_in, patch_h_in + 2 * overlap_h_in);
	
		//Draw each potential patch in the lower left corner
		s.set_position(0, 0, 1/w_count_out + 2 * overlap_w_out, 1/h_count_out + 2 * overlap_h_out);

		useFBO(p.fbo, fbo_texture, 0L);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		DrawModel(s.get(), plaintextureshader, (char*)"in_Position", NULL, (char*)"in_TexCoord");
	}
}

float ImageQuilter::calc_minerror(patch_data& in1, patch_data& in2, bool side)
{
	useFBO(fbo_work, in1.fbo, in2.fbo);
	
	glUseProgram(minerrorshader);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glUniform1i(glGetUniformLocation(minerrorshader, "texUnit1"), 0);
	glUniform1i(glGetUniformLocation(minerrorshader, "texUnit2"), 1);
	glUniform1f(glGetUniformLocation(minerrorshader, "width"), patch_w_out + 2 * overlap_w_out);
	glUniform1f(glGetUniformLocation(minerrorshader, "height"), patch_h_out + 2 * overlap_h_out);
	glUniform1f(glGetUniformLocation(minerrorshader, "offset_w"), overlap_w_in);
	glUniform1f(glGetUniformLocation(minerrorshader, "offset_h"), overlap_h_in);
	glUniform1i(glGetUniformLocation(minerrorshader, "side"), 1);
	DrawModel(image_square.get(), minerrorshader, (char*)"in_Position", NULL, (char*)"in_TexCoord");

	//TODO: should read one side at a time
	int arr_x = (patch_w_out + 2 * overlap_w_out) * width;
	int arr_y = (patch_h_out + 2 * overlap_h_out) * height;
	GLfloat array[1*arr_x*arr_y];
	glReadPixels(0, 0, arr_x, arr_y, GL_RED,GL_FLOAT, array);

	//calculate the fitness value of the current overlap
	float fitness = 0;
	for(int i = 0; i < 1*arr_x*arr_y; i++) {
		fitness += array[i];
	}

	return fitness;
}