#include "ImageQuilter.h"

void ImageQuilter::create_image_neighbor()
{
	fbo_texture->texid = tex.texID;

	int count = 0;
 	float w_count_out = width/synthesis_size;
	float h_count_out = height/synthesis_size;
	
	float w_count_in = tex.width/synthesis_size;
	float h_count_in = tex.height/synthesis_size;
	
	for(unsigned i = 0; i < w_count_out; i++) {
		for(unsigned j = 0; j < h_count_out; j++) {
			
			//we will draw a patch of texture to current fbo patch
			useFBO(fbo_patch, fbo_texture, 0L);

			float pix_w = 1/(float)width;  // width of one pixel
			float pix_h = 1/(float)height; // height of one pixel
			float overlap_w = synthesis_size/8.0 * pix_w;  // amount of width pixels that overlap each other
			float overlap_h = synthesis_size/8.0 * pix_h;  // amount of height pixels that overlap each other

			float x_pos = rng(width*(1.0-1.0/w_count_in)) * pix_w;
			float y_pos = rng(height*(1.0-1.0/h_count_in)) * pix_w;
			float x_width = 1/w_count_in;
			float y_height = 1/h_count_in;

			//create the square to draw our patch from
			Square s(x_pos - overlap_w, y_pos - overlap_h, x_width + 2 * overlap_w, y_height + 2 * overlap_h);

			//convert overlap to final image coordinates
			float overlap_w_out = overlap_w * tex.width / width;
			float overlap_h_out = overlap_w * tex.height / height;

			//position square to fit final image
			s.set_position(i/w_count_out - overlap_w_out, j/h_count_out - overlap_h_out, 1/w_count_out + 2 * overlap_w_out, 1/h_count_out + 2 * overlap_h_out);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(transparencyshader);
			glClearColor(0.0, 0.0, 0.0, 0);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);

			//perform drawing of side squares
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_l1"), x_pos - overlap_w);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_l2"), x_pos + overlap_w);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_r1"), x_pos + x_width - overlap_w);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_r2"), x_pos + x_width + overlap_w);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_u1"), y_pos + y_height - overlap_h);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_u2"), y_pos + y_height + overlap_h);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_d1"), y_pos - overlap_h);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord_d2"), y_pos + overlap_h);
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