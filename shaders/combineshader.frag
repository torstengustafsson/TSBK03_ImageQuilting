#version 150

uniform sampler2D texUnit;
uniform sampler2D texUnit2;

in vec2 outTexCoord;
out vec4 fragColor;

void main(void)
{

vec2 texCoord = outTexCoord;

vec4 a  = texture(texUnit, texCoord);
vec4 b  = texture(texUnit2, texCoord);

fragColor = (a + b);

}
