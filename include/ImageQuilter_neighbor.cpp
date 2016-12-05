#include "ImageQuilter.h"

void ImageQuilter::create_image_neighbor()
{
	fbo_texture->texid = tex.texID;

	int count = 0;

	for(unsigned i = 0; i < w_count_out; i++) {
		for(unsigned j = 0; j < h_count_out; j++) {
			
			//we will draw a patch of texture to current fbo patch
			useFBO(fbo_patch, fbo_texture, 0L);

			float x_pos = overlap_w_in + rng(tex.width*(1.0-(1.0/w_count_in + 2 * overlap_w_in))) * pix_w_in;
			float y_pos = overlap_h_in + rng(tex.height*(1.0-(1.0/h_count_in + 2 * overlap_h_in))) * pix_h_in;

			//create the square to draw our patch from
			Square s(
				x_pos - overlap_w_in, 
				y_pos - overlap_h_in, 
				patch_w_in + 2 * overlap_w_in, 
				patch_h_in + 2 * overlap_h_in
			);

			//position square to fit final image
			s.set_position(
				i/w_count_out - overlap_w_out, 
				j/h_count_out - overlap_h_out, 
				1/w_count_out + 2 * overlap_w_out, 
				1/h_count_out + 2 * overlap_h_out
			);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(transparencyshader);
			glClearColor(0.0, 0.0, 0.0, 0);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

			//perform drawing of side squares
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_l1"), x_pos - overlap_w_in);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_l2"), x_pos + overlap_w_in);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_r1"), x_pos + patch_w_in - overlap_w_in);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_r2"), x_pos + patch_w_in + overlap_w_in);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_u1"), y_pos + patch_h_in - overlap_h_in);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_u2"), y_pos + patch_h_in + overlap_h_in);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_d1"), y_pos - overlap_h_in);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_d2"), y_pos + overlap_h_in);

			DrawModel(s.get(), transparencyshader, (char*)"in_Position", NULL, (char*)"in_TexCoord");

			if(count++ == 0) {
				draw_fbo(fbo1, fbo_patch, 0L, plaintextureshader);
			}
			else {
				//ping-pong between fbo1 and fbo2
				if(count % 2 == 0)
					draw_fbo(fbo2, fbo1, fbo_patch, combineshader);
				else 
					draw_fbo(fbo1, fbo2, fbo_patch, combineshader);
			}
		}
	}

	useFBO(fbo_final, (count % 2 == 0) ? fbo2 : fbo1, 0L);
	
	//draw the final image
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(plaintextureshader);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	DrawModel(image_square.get(), plaintextureshader, (char*)"in_Position", NULL, (char*)"in_TexCoord");
}