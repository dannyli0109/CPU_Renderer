#pragma once
#include <vector>
#include "Graphics.h"
#include "ShaderProgram.h"
#include "Renderer.h"
#include <unordered_map>

class CPURenderer : public Renderer
{
public:
	static CPURenderer* GetInstance();
	static CPURenderer* CreateInstance(int width, int height);
	CPURenderer(const CPURenderer& other) = delete;
	CPURenderer& operator=(const CPURenderer& other) = delete;
	static void DeleteInstance();
	void Clear() override;
	void Draw() override;
	unsigned int UploadVertices(const std::vector<Vertex>&& vertexData) override;
	unsigned int UploadIndices(const std::vector<unsigned short>&& indexData) override;
	void SetUniform(std::string s, glm::mat4 m);

	void BindVertexBuffer(unsigned int id);
	void UnbindVertexBuffer();
	void BindIndexBuffer(unsigned int id);
	void UnbindIndexBuffer();
	void DrawLine(glm::vec3 p1, glm::vec3 p2, glm::vec4 color);
	void UpdateTexture();
private:
	glm::vec4 clearColor = { 0, 0, 0, 1 };
	static CPURenderer* instance;
	int w;
	int h;
	GLuint textureID;
	// container for the texture
	GLuint quadBuffer;
	ShaderProgram* shader;
	std::vector<glm::vec4> frameBuffer;
	std::vector<float> depthBuffer;
	unsigned int bufferCount = 0;
	unsigned int activeVertexBuffer = 0;
	unsigned int activeIndexBuffer = 0;

	std::unordered_map<unsigned int, std::vector<Vertex>> verticesMap;
	std::unordered_map<unsigned int, std::vector<unsigned short>> indicesMap;
	std::unordered_map<std::string, glm::mat4> uniformM4;
	
private:
	void InitTexture();
	CPURenderer(int width, int height);
	~CPURenderer();
	void InitQuad();
	void SetPixel(int x, int y, glm::vec4 color);
	unsigned int GenerateBuffer();
	glm::vec3 ComputeBarycentric2D(glm::vec3 position, std::vector<glm::vec3>& v);
	bool PointInTriangle(glm::vec3 p, std::vector<glm::vec3>& v);
	void Resterise(std::vector<Vertex>& v);
	int GetCoordinate(int x, int y);


};