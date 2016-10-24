#version 150

//side: 0 = left, 1 = right, 2 = up, 3 = down
uniform int side;
uniform float coord1;
uniform float coord2;

in vec2 outTexCoord;
uniform sampler2D texUnit;
out vec4 out_Color;

void main(void)
{
	float opacity = 0.0;
	if(side == 0) {
		opacity = 1 - (coord1 - outTexCoord.x) / (coord1 - coord2);
	}
	else if(side == 1) {
		opacity = (coord1 - outTexCoord.x) / (coord1 - coord2);
	}
	else if(side == 2) {
		opacity = (coord1 - outTexCoord.y) / (coord1 - coord2);
	}
	else if(side == 3) {
		opacity = 1 - (coord1 - outTexCoord.y) / (coord1 - coord2);
	}

    out_Color = opacity * texture(texUnit, outTexCoord);
}
