#pragma once
#include <vector>
#include "Graphics.h"
#include "ShaderProgram.h"
#include "Renderer.h"
#include <unordered_map>
#include <functional>
#include "Shader.h"

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
	unsigned int UploadVertices(const std::vector<Vertex>& vertexData) override;
	unsigned int UploadIndices(const std::vector<unsigned short>& indexData) override;
	unsigned int UploadTexture(Texture* texture) override;
	void SetUniform(std::string s, glm::mat4 m);
	void SetUniform(std::string s, int i);
	void SetUniform(std::string s, glm::vec3 v);
	glm::mat4 GetUniform4fM(std::string s);
	int GetUnform1i(std::string s);
	glm::vec3 GetUnform3fv(std::string s);

	void BindVertexBuffer(unsigned int id);
	void UnbindVertexBuffer();
	void BindIndexBuffer(unsigned int id);
	void UnbindIndexBuffer();
	void BindTextureUint(unsigned int textureUint, unsigned int textureId);
	Texture* GetTexture(std::string s);

	void SetClippingPlane(float near, float far);


	//void BindTexture(unsigned int id)
	void DrawLine(glm::vec3 p1, glm::vec3 p2, glm::vec4 color);
	void UpdateTexture();
	std::function<glm::vec4(Renderer* r, VertexShaderPayload& payload)> vertexShader;
	std::function<glm::vec3(Renderer* r, FragmentShaderPayload& payload)> fragmentShader;
private:
	glm::vec4 clearColor = { 0, 0, 0, 1 };
	static CPURenderer* instance;
	int w;
	int h;
	int f;
	int n;
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
	std::unordered_map<unsigned int, Texture*> textureMap;
	std::unordered_map<std::string, glm::mat4> uniform4fM;
	std::unordered_map<std::string, int> uniform1i;
	std::unordered_map<std::string, glm::vec3> uniform3fv;

	
private:
	void InitTexture();
	CPURenderer(int width, int height);
	~CPURenderer();
	void InitQuad();
	void SetPixel(int x, int y, glm::vec4 color);
	unsigned int GenerateBuffer();
	glm::vec3 ComputeBarycentric2D(glm::vec3 position, std::vector<glm::vec3>& v);
	bool PointInTriangle(glm::vec3 p, std::vector<glm::vec3>& v);
	void Resterise(std::vector<VertexShaderPayload>& payloads);
	int GetCoordinate(int x, int y);
	glm::vec3 Interpolate(float alpha, float beta, float gamma, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, float w1, float w2, float w3);
	glm::mat3 Interpolate(float alpha, float beta, float gamma, glm::mat3 m1, glm::mat3 m2, glm::mat3 m3, float w1, float w2, float w3);
	std::vector<unsigned int> textureUints = std::vector<unsigned int>(16, 0);
};