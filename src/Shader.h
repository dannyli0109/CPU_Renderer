#pragma once


#include "Matrices.h"
#include "Texture.h"
#include <algorithm>

class Renderer;
struct Vertex;

struct FragmentShaderPayload
{
	FragmentShaderPayload(const glm::vec3& pos, const glm::vec3& col, const glm::vec3& nor, const glm::vec2& tc) :position(pos), color(col), normal(nor), texCoords(tc) {}

	glm::vec3 position;
	glm::vec3 color;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec3 modelPos;
	glm::mat3 tbn;
};

struct VertexShaderPayload
{
	VertexShaderPayload(Vertex& v);

	struct Out
	{
		glm::vec3 modelPos;
		glm::vec3 vertexNormal;
		glm::mat3 tbn;
	};
	glm::vec4 position;
	glm::vec3 normal;
	glm::vec4 color;
	glm::vec2 texCoords;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	Out out;
};

glm::vec3 PhongVertexNormalFragmentShader(Renderer* r, FragmentShaderPayload& payload);
glm::vec3 PhongNormalMapFragmentShader(Renderer* r, FragmentShaderPayload& payload);

glm::vec3 NormalMapFragmentShader(Renderer* r, FragmentShaderPayload& payload);
glm::vec3 NormalMapFragmentShader(Renderer* r, FragmentShaderPayload& payload);

glm::vec4 VertexNormalVertexShader(Renderer* r, VertexShaderPayload& payload);
glm::vec4 NormalMapVertexShader(Renderer* r, VertexShaderPayload& payload);