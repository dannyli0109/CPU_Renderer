#include "CPURenderer.h"

CPURenderer::CPURenderer(int width, int height)
{
	this->w = width;
	this->h = height;
	frameBuffer.resize(w * h);
	depthBuffer.resize(w * h);
	InitTexture();
	InitQuad();
	shader = new ShaderProgram("Plain.vert", "Plain.frag");
}

CPURenderer* CPURenderer::GetInstance()
{
	return instance;
}

CPURenderer* CPURenderer::CreateInstance(int width, int height)
{
	if (!instance)
	{
		instance = new CPURenderer(width, height);
	}
	return instance;
}

void CPURenderer::DeleteInstance()
{
	if (instance)
	{
		delete instance;
		instance = nullptr;
	}
}

void CPURenderer::SetPixel(int x, int y, glm::vec4 color)
{
	if (x < 0 || x >= w) return;
	if (y < 0 || y >= h) return;
	frameBuffer[y * w + x] = color;
}

unsigned int CPURenderer::GenerateBuffer()
{
	return ++bufferCount;
}

glm::vec3 CPURenderer::ComputeBarycentric2D(glm::vec3 position, std::vector<glm::vec3>& v)
{
	float x = position.x;
	float y = position.y;
	float c1 = (x * (v[1].y - v[2].y) + (v[2].x - v[1].x) * y + v[1].x * v[2].y - v[2].x * v[1].y) / (v[0].x * (v[1].y - v[2].y) + (v[2].x - v[1].x) * v[0].y + v[1].x * v[2].y - v[2].x * v[1].y);
	float c2 = (x * (v[2].y - v[0].y) + (v[0].x - v[2].x) * y + v[2].x * v[0].y - v[0].x * v[2].y) / (v[1].x * (v[2].y - v[0].y) + (v[0].x - v[2].x) * v[1].y + v[2].x * v[0].y - v[0].x * v[2].y);
	float c3 = (x * (v[0].y - v[1].y) + (v[1].x - v[0].x) * y + v[0].x * v[1].y - v[1].x * v[0].y) / (v[2].x * (v[0].y - v[1].y) + (v[1].x - v[0].x) * v[2].y + v[0].x * v[1].y - v[1].x * v[0].y);
	return { c1,c2,c3 };
}

bool CPURenderer::PointInTriangle(glm::vec3 p, std::vector<glm::vec3>& v)
{
	bool allPositive = true;
	bool allNegative = true;
	for (int i = 0; i < v.size(); i++)
	{
		glm::vec3 v1 = v[(i + 1) % v.size()] - v[i];
		glm::vec3 v2 = p - v[i];

		glm::vec3 cross = { v1.y * v2.z - v2.y * v1.z, v1.z * v2.x - v2.z * v1.x, v1.x * v2.y - v2.x * v1.y };

		if (cross.z < 0) allPositive = false;
		if (cross.z > 0) allNegative = false;
	}
	if (allPositive || allNegative) return true;
}

void CPURenderer::Resterise(std::vector<Vertex>& v)
{
	float minX = INFINITY, minY = INFINITY, maxX = -INFINITY, maxY = -INFINITY;
	for (int i = 0; i < v.size(); i++)
	{
		glm::vec3 pos = v[i].position;
		if (minX > pos.x) minX = pos.x;
		if (minY > pos.y) minY = pos.y;
		if (maxX < pos.x) maxX = pos.x;
		if (maxY < pos.y) maxY = pos.y;
	}

	std::vector<glm::vec3> positions = { v[0].position, v[1].position, v[2].position };
	for (int y = minY; y <= maxY; y++)
	{
		for (int x = minX; x <= maxX; x++)
		{
			glm::vec3 p = { x + 0.5, y + 0.5, 1 };
			if (PointInTriangle(p, positions))
			{
				glm::vec3 baryCentric = ComputeBarycentric2D(p, positions);
				float alpha = baryCentric.x;
				float beta = baryCentric.y;
				float gamma = baryCentric.z;

				float w_reciprocal = 1.0 / (alpha / v[0].position.w + beta / v[1].position.w + gamma / v[2].position.w);
				float z_interpolated = alpha * v[0].position.z / v[0].position.w + beta * v[1].position.z / v[1].position.w + gamma * v[2].position.z / v[2].position.w;

				int index = GetCoordinate(std::floor(p.x), std::floor(p.y));
				if (z_interpolated < depthBuffer[index])
				{
					depthBuffer[index] = z_interpolated;
					SetPixel(std::floor(p.x), std::floor(p.y), v[0].color);
				}
			}
		}
	}
}

int CPURenderer::GetCoordinate(int x, int y)
{
	return y * w + x;
}

void CPURenderer::BindVertexBuffer(unsigned int id)
{
	activeVertexBuffer = id;
}

void CPURenderer::UnbindVertexBuffer()
{
	activeVertexBuffer = 0;
}

void CPURenderer::BindIndexBuffer(unsigned int id)
{
	activeIndexBuffer = id;
}

void CPURenderer::UnbindIndexBuffer()
{
	activeIndexBuffer = 0;
}

void CPURenderer::DrawLine(glm::vec3 p1, glm::vec3 p2, glm::vec4 color)
{
	bool steep = false;
	float x0 = p1.x;
	float x1 = p2.x;
	float y0 = p1.y;
	float y1 = p2.y;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (steep) {
			SetPixel(y, x, color);
		}
		else {
			SetPixel(x, y, color);
		}
		error2 += derror2;
		if (error2 > dx) {
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

void CPURenderer::Clear()
{
	std::fill(frameBuffer.begin(), frameBuffer.end(), clearColor);
	std::fill(depthBuffer.begin(), depthBuffer.end(), INFINITY);
}

void CPURenderer::Draw()
{
	shader->UseProgram();
	std::vector<Vertex>& vertices = verticesMap[activeVertexBuffer];
	std::vector<unsigned short>& indices = indicesMap[activeIndexBuffer];
	glm::mat4 model = uniformM4["modelMatrix"];
	glm::mat4 view = uniformM4["viewMatrix"];
	glm::mat4 projection = uniformM4["projectionMatrix"];
	glm::mat4 mvp = projection * view * model;
	float f1 = (100 - 0.1) / 2.0;
	float f2 = (100 + 0.1) / 2.0;
	for (int i = 0; i < indices.size() / 3; i++)
	{
		unsigned short i0 = indices[i * 3];
		unsigned short i1 = indices[i * 3 + 1];
		unsigned short i2 = indices[i * 3 + 2];
		std::vector<Vertex> v = { vertices[i0], vertices[i1], vertices[i2] };
		for (int j = 0; j < 3; j++)
		{
			glm::vec4 v4 = mvp * v[j].position;
			v[j].position = v4 / v4.w;
			v[j].position.x = 0.5f * w * (v[j].position.x + 1.0f);
			v[j].position.y = 0.5f * h * (v[j].position.y + 1.0f);
			v[j].position.z = v[j].position.z * f1 + f2;
		}
		Resterise(v);
	}
	UpdateTexture();
}

unsigned int CPURenderer::UploadVertices(const std::vector<Vertex>&& vertexData)
{
	unsigned int buffer = GenerateBuffer();
	BindVertexBuffer(buffer);
	verticesMap[activeVertexBuffer] = vertexData;
	UnbindVertexBuffer();
	return buffer;
}

unsigned int CPURenderer::UploadIndices(const std::vector<unsigned short>&& indexData)
{
	unsigned int buffer = GenerateBuffer();
	BindIndexBuffer(buffer);
	indicesMap[activeIndexBuffer] = indexData;
	UnbindIndexBuffer();
	return buffer;
}

void CPURenderer::SetUniform(std::string s, glm::mat4 m)
{
	uniformM4[s] = m;
}

CPURenderer::~CPURenderer()
{
	delete shader;
	glDeleteBuffers(1, &quadBuffer);
	glDeleteTextures(1, &textureID);
}

void CPURenderer::InitTexture()
{
	glGenTextures(1, &textureID);
}

void CPURenderer::UpdateTexture()
{
	shader->UseProgram();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, frameBuffer.data());
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);

	GLuint texLocation = shader->GetUniformLocation("defaultTexture");
	glUniform1i(texLocation, 0);

	glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	ShaderProgram::ClearPrograms();
}



void CPURenderer::InitQuad()
{
	glGenBuffers(1, &quadBuffer);

	float vertexPositionData[] = {
		1.0f, 1.0f,
		-1.0f, -1.0f,
		1.0f, -1.0f,

		-1.0f, -1.0f,
		1.0f, 1.0f,
		-1.0f, 1.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, vertexPositionData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glEnableVertexAttribArray(0);
}

CPURenderer* CPURenderer::instance = nullptr;

