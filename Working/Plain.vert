#version 450

layout (location = 0) in vec2 Position;

out vec2 UVs;

void main()
{
	UVs = vec2(vec2(Position.x, -Position.y) * 0.5 + 0.5);
	gl_Position = vec4(Position, 0.0, 1.0);
}