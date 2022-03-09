#pragma once
#include "Matrices.h"

struct Vertex
{
	glm::vec3 position{ 0, 0, 0 };
	glm::vec4 color{ 0, 0, 0, 1 };
};

class Renderer
{
public:
	virtual void Draw() = 0;
	virtual void Clear() = 0;
	virtual unsigned int UploadVertices(const std::vector<Vertex>&& vertexData) = 0;
	virtual unsigned int UploadIndices(const std::vector<unsigned short>&& indexData) = 0;
};