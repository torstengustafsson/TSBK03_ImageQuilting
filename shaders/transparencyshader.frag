#version 150

// The idea of coord is to have them be the lower and upper bounds for the current texture coordinate 
// side, and from it return a normalized value in their range. Values must be set in the correct
// range outside of the shader. l stand for left, r for right, u for up and d for down.
// 1 is always the lower value which makes 2 always the higher value.
uniform float coord_l1;
uniform float coord_l2;
uniform float coord_r1;
uniform float coord_r2;
uniform float coord_u1;
uniform float coord_u2;
uniform float coord_d1;
uniform float coord_d2;

in vec2 outTexCoord;
uniform sampler2D texUnit;
out vec4 out_Color;

//checks if current pixel is within these bounds
bool bounds(float x1, float x2, float y1, float y2)
{
	return outTexCoord.x >= x1 && outTexCoord.x <= x2 && outTexCoord.y >= y1 && outTexCoord.y <= y2;
}

void main(void)
{
	float opacity = 1.0;
	if(bounds(coord_l1, coord_l2, coord_d2, coord_u1)) { // left side
		opacity = (coord_l1 - outTexCoord.x) / (coord_l1 - coord_l2);
	}
	else if(bounds(coord_r1, coord_r2, coord_d2, coord_u1)) { // right side
		opacity = (coord_r2 - outTexCoord.x) / (coord_r2 - coord_r1);
	}
	else if(bounds(coord_l2, coord_r1, coord_u1, coord_u2)) { // up side
		opacity = (coord_u2 - outTexCoord.y) / (coord_u2 - coord_u1);
	}
	else if(bounds(coord_l2, coord_r1, coord_d1, coord_d2)) { // down side
		opacity = (coord_d1 - outTexCoord.y) / (coord_d1 - coord_d2);
	}
	else if(bounds(coord_l1, coord_l2, coord_u1, coord_u2)) { // upper left corner
		opacity = ((coord_u2 - outTexCoord.y) / (coord_u2 - coord_u1)) * ((coord_l1 - outTexCoord.x) / (coord_l1 - coord_l2));
	}
	else if(bounds(coord_r1, coord_r2, coord_u1, coord_u2)) { // upper right corner
		opacity = ((coord_u2 - outTexCoord.y) / (coord_u2 - coord_u1)) * ((coord_r2 - outTexCoord.x) / (coord_r2 - coord_r1));
	}
	else if(bounds(coord_l1, coord_l2, coord_d1, coord_d2)) { // lower left corner
		opacity = ((coord_d1 - outTexCoord.y) / (coord_d1 - coord_d2)) * ((coord_l1 - outTexCoord.x) / (coord_l1 - coord_l2));
	}
	else if(bounds(coord_r1, coord_r2, coord_d1, coord_d2)) { // lower right corner
		opacity = ((coord_d1 - outTexCoord.y) / (coord_d1 - coord_d2)) * ((coord_r2 - outTexCoord.x) / (coord_r2 - coord_r1));
	}

	out_Color = opacity * texture(texUnit, outTexCoord);
}