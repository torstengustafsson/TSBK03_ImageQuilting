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
			
			//draw a patch of texture to current fbo patch
			useFBO(fbo_patch, fbo_texture, 0L);

			float pix_w = 1/(float)width;  // width of one pixel
			float pix_h = 1/(float)height; // height of one pixel
			float overlap_w = synthesis_size/8.0 * pix_w;  // amount of width pixels that overlap each other
			float overlap_h = synthesis_size/8.0 * pix_h;  // amount of height pixels that overlap each other

			float x_pos = rng(width*(1.0-1.0/w_count_in)) * pix_w;
			float y_pos = rng(height*(1.0-1.0/h_count_in)) * pix_w;
			float x_width = 1/w_count_in;
			float y_height = 1/h_count_in;

			//create the square to draw our patch from (without overlap)
			Square s(x_pos + overlap_w, y_pos + overlap_h, x_width - 2 * overlap_w, y_height - 2 * overlap_h);
			
			//create the overlapping rectangles based on our square
			Square s_left(x_pos - overlap_w, y_pos, 2 * overlap_w, y_height);
			Square s_right(x_pos + x_width - overlap_w, y_pos, 2 * overlap_w, y_height);
			Square s_up(x_pos, y_pos + y_height - overlap_h, x_width, 2 * overlap_h);
			Square s_down(x_pos, y_pos - overlap_h, x_width, 2 * overlap_h);

			//corner cases must be handled also

			//fix position for pong-ponging
			float overlap_w_out = overlap_w * tex.width / width;
			float overlap_h_out = overlap_w * tex.height / height;

			s.set_position(i/w_count_out + overlap_w_out, j/h_count_out + overlap_h_out, 1/w_count_out - 2 * overlap_w_out, 1/h_count_out - 2 * overlap_h_out);
			s_left.set_position(i/w_count_out - overlap_w_out, j/h_count_out, 2 * overlap_w_out, 1/h_count_out);
			s_right.set_position((i + 1)/w_count_out - overlap_w_out, j/h_count_out, 2 * overlap_w_out, 1/h_count_out);
			s_up.set_position(i/w_count_out, (j + 1)/h_count_out - overlap_h_out, 1/w_count_out, 2 * overlap_h_out);
			s_down.set_position(i/w_count_out, j/h_count_out - overlap_h_out, 1/w_count_out, 2 * overlap_h_out);
			
			//perform drawing
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(plaintextureshader);
			glClearColor(0.0, 0.0, 0.0, 0);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			DrawModel(s.get(), plaintextureshader, "in_Position", NULL, "in_TexCoord");

			glUseProgram(transparencyshader);
			glUniform1i(glGetUniformLocation(transparencyshader, "side"), 0);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord1"), x_pos + overlap_w);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord2"), x_pos - overlap_w);
			DrawModel(s_left.get(), transparencyshader, "in_Position", NULL, "in_TexCoord");
			glUniform1i(glGetUniformLocation(transparencyshader, "side"), 1);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord1"), x_pos + x_width + overlap_w);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord2"), x_pos + x_width - overlap_w);
			DrawModel(s_right.get(), transparencyshader, "in_Position", NULL, "in_TexCoord");
			glUniform1i(glGetUniformLocation(transparencyshader, "side"), 2);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord1"), y_pos + y_height + overlap_h);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord2"), y_pos + y_height - overlap_h);
			DrawModel(s_up.get(), transparencyshader, "in_Position", NULL, "in_TexCoord");
			glUniform1i(glGetUniformLocation(transparencyshader, "side"), 3);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord1"), y_pos + overlap_h);
			glUniform1f(glGetUniformLocation(transparencyshader, "coord2"), y_pos - overlap_h);
			DrawModel(s_down.get(), transparencyshader, "in_Position", NULL, "in_TexCoord");

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

	if(count % 2 == 0)
		useFBO(fbo_final, fbo2, 0L);
	else 
		useFBO(fbo_final, fbo1, 0L);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(plaintextureshader);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	DrawModel(image_square.get(), plaintextureshader, "in_Position", NULL, "in_TexCoord");
}