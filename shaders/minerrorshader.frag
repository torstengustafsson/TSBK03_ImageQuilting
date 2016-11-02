#version 150

uniform sampler2D texUnit1;
uniform sampler2D texUnit2;

in vec2 outTexCoord;
out vec4 fragColor;

uniform float width; // the width of a patch
uniform float height; // the height of a patch
uniform float offset_w; // the size of the overlap width
uniform float offset_h; // the size of the overlap height

uniform int side; // 0 = left side. 1 = down side


void main(void)
{
	if(side == 0 && outTexCoord.x < offset_w) {
		vec2 texCoord1 = outTexCoord;
		vec2 texCoord2 = outTexCoord + vec2(width - offset_w, 0.0);
		
		vec4 a  = texture(texUnit1, texCoord1); // left patch
		vec4 b  = texture(texUnit2, texCoord2); // right patch
		fragColor = vec4(pow(a.x - b.x, 2), pow(a.y - b.y, 2), pow(a.z - b.z, 2), 1.0);
	}
	else if(side == 1 && outTexCoord.y < offset_h) {
		vec2 texCoord1 = outTexCoord + vec2(0.0, height - offset_h);
		vec2 texCoord2 = outTexCoord;
		
		vec4 a  = texture(texUnit1, texCoord1); // left patch
		vec4 b  = texture(texUnit2, texCoord2); // right patch
		fragColor = vec4(pow(a.x - b.x, 2), pow(a.y - b.y, 2), pow(a.z - b.z, 2), 1.0);
	}
	else {
		fragColor = vec4(0.0);	
	}
}
