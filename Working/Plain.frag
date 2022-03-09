#version 450

uniform sampler2D defaultTexture;

in vec2 UVs;

out vec4 FragColour;

void main()
{
	FragColour = texture(defaultTexture, UVs);
	//FragColour = vec4(1, 0, 0, 1);
}