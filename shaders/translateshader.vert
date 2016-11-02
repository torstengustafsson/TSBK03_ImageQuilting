#version 150

uniform mat4 mat;

in vec3 in_Position;
in vec2 in_TexCoord;

out vec2 outTexCoord;

void main(void)
{
	outTexCoord = in_TexCoord;
	gl_Position = mat * vec4(in_Position, 1.0);
}
