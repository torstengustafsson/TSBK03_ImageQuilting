#include "ImageQuilter.h"

void ImageQuilter::create_image_random()
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

			//create the square to draw our patch from
			Square s(
				rng(width*(1-1/w_count_in))/(float)width,	 // x
				rng(height*(1-1/h_count_in))/(float)height,  // y
				1/w_count_in,								 // width
				1/h_count_in);								 // height			
			s.set_position(i/w_count_out, j/h_count_out, 1/w_count_out, 1/h_count_out); // fix position for ping-ponging

			//perform drawing
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(plaintextureshader);
			glClearColor(0.0, 0.0, 0.0, 0);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			DrawModel(s.get(), plaintextureshader, "in_Position", NULL, "in_TexCoord");

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