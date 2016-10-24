#version 150

in vec2 outTexCoord;
uniform sampler2D texUnit;
out vec4 out_Color;

void main(void)
{
    out_Color = 0.5 * texture(texUnit, outTexCoord);
}
