#pragma once
#include "Matrices.h"
#include <vector>
#include "Texture.h"


struct Vertex
{
	glm::vec4 position{ 0, 0, 0, 1 };
	glm::vec4 color{ 0, 0, 0, 1 };
	glm::vec2 uv{ 0, 0 };
	glm::vec3 normal{ 0, 0, 0 };
	glm::vec3 vertTangent{ 0, 0, 0 };
	glm::vec3 vertBitangent{ 0, 0, 0 };
};

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<unsigned short> indices;
};

struct Model
{
	std::vector<Mesh> meshes;
};

struct Light
{
	glm::vec3 position;
	glm::vec3 intensity;
};


class Renderer
{
public:
	virtual void Draw() = 0;
	virtual void Clear() = 0;
	virtual unsigned int UploadVertices(const std::vector<Vertex>& vertexData) = 0;
	virtual unsigned int UploadIndices(const std::vector<unsigned short>& indexData) = 0;
	virtual unsigned int UploadTexture(Texture* texture) = 0;
	virtual Texture* GetTexture(std::string s) = 0;
	virtual glm::mat4 GetUniform4fM(std::string s) = 0;
	virtual int GetUnform1i(std::string s) = 0;
	virtual glm::vec3 GetUnform3fv(std::string s) = 0;
};