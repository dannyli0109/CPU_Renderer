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
	return false;
}

void CPURenderer::Resterise(std::vector<VertexShaderPayload>& payloads)
{
	float minX = INFINITY, minY = INFINITY, maxX = -INFINITY, maxY = -INFINITY;
	std::vector<glm::vec3> positions = { payloads[0].position, payloads[1].position, payloads[2].position};
	for (int i = 0; i < positions.size(); i++)
	{
		glm::vec3 pos = positions[i];
		if (minX > pos.x) minX = pos.x;
		if (minY > pos.y) minY = pos.y;
		if (maxX < pos.x) maxX = pos.x;
		if (maxY < pos.y) maxY = pos.y;
	}

	for (int y = minY; y <= maxY; y++)
	{
		for (int x = minX; x <= maxX; x++)
		{
			if (x < 0 || y < 0) continue;
			if (x >= w || y >= h) continue;
			glm::vec3 p = { x + 0.5f, y + 0.5f, 1.0f };
			//std::cout << positions[0].x << ", " << positions[0].y << std::endl;
			if (PointInTriangle(p, positions))
			{
				glm::vec3 baryCentric = ComputeBarycentric2D(p, positions);
				float alpha = baryCentric.x;
				float beta = baryCentric.y;
				float gamma = baryCentric.z;

				float z1 = payloads[0].position.z;
				float z2 = payloads[1].position.z;
				float z3 = payloads[2].position.z;

				float w1 = payloads[0].position.w;
				float w2 = payloads[1].position.w;
				float w3 = payloads[2].position.w;

				float wReciprocal = 1.0f / alpha / w1 + beta / w2 + gamma / w3;;
				float zInterpolated = 
					alpha * z1 / w1 +
					beta * z2 / w2 +
					gamma * z3 / w3;
				//zInterpolated *= wReciprocal;

				int index = GetCoordinate(std::floor(p.x), std::floor(p.y));
				if (zInterpolated < depthBuffer[index])
				{
					depthBuffer[index] = zInterpolated;

					glm::vec4 c1 = payloads[0].color;
					glm::vec4 c2 = payloads[1].color;
					glm::vec4 c3 = payloads[2].color;

					//glm::vec3 n1 = payloads[0].out.vertexNormal;
					//glm::vec3 n2 = payloads[1].out.vertexNormal;
					//glm::vec3 n3 = payloads[2].out.vertexNormal;

					glm::vec3 n1 = payloads[0].out.vertexNormal;
					glm::vec3 n2 = payloads[1].out.vertexNormal;
					glm::vec3 n3 = payloads[2].out.vertexNormal;

					glm::mat3 tbn1 = payloads[0].out.tbn;
					glm::mat3 tbn2 = payloads[1].out.tbn;
					glm::mat3 tbn3 = payloads[2].out.tbn;


					/*glm::vec3 t1 = payloads[0].out.tbn[0];
					glm::vec3 t2 = payloads[0].out.tbn[0];
					glm::vec3 t3 = payloads[0].out.tbn[0];*/


					glm::vec2 uv1 = payloads[0].texCoords;
					glm::vec2 uv2 = payloads[1].texCoords;
					glm::vec2 uv3 = payloads[2].texCoords;

					glm::vec3 p1 = positions[0];
					glm::vec3 p2 = positions[1];
					glm::vec3 p3 = positions[2];

					glm::vec3 mp1 = payloads[0].out.modelPos;
					glm::vec3 mp2 = payloads[1].out.modelPos;
					glm::vec3 mp3 = payloads[2].out.modelPos;

					glm::vec3 color = Interpolate(alpha, beta, gamma, c1, c2, c3, w1, w2, w3);
					glm::vec3 normal = Interpolate(alpha, beta, gamma, n1, n2, n3, w1, w2, w3);
	/*				normal.x = normal.x * 0.5f + 0.5f;
					normal.y = normal.y * 0.5f + 0.5f;
					normal.z = normal.z * 0.5f + 0.5f;*/
					normal = glm::normalize(normal);

					glm::vec2 uv = Interpolate(alpha, beta, gamma, glm::vec3(uv1, 0), glm::vec3(uv2, 0), glm::vec3(uv3, 0), w1, w2, w3);
					glm::vec3 position = Interpolate(alpha, beta, gamma, p1, p2, p3, w1, w2, w3);
					glm::vec3 modelPos = Interpolate(alpha, beta, gamma, mp1, mp2, mp3, w1, w2, w3);
					glm::mat3 tbn = Interpolate(alpha, beta, gamma, tbn1, tbn2, tbn3, w1, w2, w3);


					FragmentShaderPayload payload(position, color, normal, uv);
					payload.tbn = tbn;
					//payload.tbn = payloads[]
					payload.modelPos = modelPos;
					glm::vec3 fragColor = fragmentShader(this, payload);
					SetPixel(std::floor(p.x), std::floor(p.y), glm::vec4(fragColor, 1));
				}
			}
		}
	}
}

int CPURenderer::GetCoordinate(int x, int y)
{
	return y * w + x;
}

glm::vec3 CPURenderer::Interpolate(float alpha, float beta, float gamma, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, float w1, float w2, float w3)
{
	return (alpha * v1 / w1 + beta * v2 / w2 + gamma * v3 / w3);
}

glm::mat3 CPURenderer::Interpolate(float alpha, float beta, float gamma, glm::mat3 m1, glm::mat3 m2, glm::mat3 m3, float w1, float w2, float w3)
{
	glm::vec3 t = Interpolate(alpha, beta, gamma, m1[0], m2[0], m3[0], w1, w2, w3);
	glm::vec3 b = Interpolate(alpha, beta, gamma, m1[1], m2[1], m3[1], w1, w2, w3);
	glm::vec3 n = Interpolate(alpha, beta, gamma, m1[2], m2[2], m3[2], w1, w2, w3);
	return { t, b, n };
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

void CPURenderer::BindTextureUint(unsigned int textureUint, unsigned int textureId)
{
	textureUints[textureUint] = textureId;
}

Texture* CPURenderer::GetTexture(std::string s)
{
	unsigned int textureUnit = uniform1i[s];
	unsigned int textureId = textureUints[textureUnit];
	return textureMap[textureId];
}

void CPURenderer::SetClippingPlane(float near, float far)
{
	n = near;
	f = far;
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
	glm::mat4 model = uniform4fM["modelMatrix"];
	glm::mat4 view = uniform4fM["viewMatrix"];
	glm::mat4 projection = uniform4fM["projectionMatrix"];
	glm::mat4 mvp = projection * view * model;
	float f1 = (f - n) / 2.0;
	float f2 = (f + n) / 2.0;
	for (int i = 0; i < indices.size() / 3; i++)
	{
		unsigned short i0 = indices[i * 3];
		unsigned short i1 = indices[i * 3 + 1];
		unsigned short i2 = indices[i * 3 + 2];
		std::vector<unsigned short> ind = { i0, i1, i2 };
		std::vector<VertexShaderPayload> payloads;
		for (int j = 0; j < 3; j++)
		{
			Vertex vertex = vertices[ind[j]];
			VertexShaderPayload payload(vertex);
			glm::vec4 v4 = vertexShader(this, payload);
			payload.position = v4 / v4.w;
			payload.position.x = 0.5f * w * (payload.position.x + 1.0f);
			payload.position.y = 0.5f * h * (payload.position.y + 1.0f);
			payload.position.z = payload.position.z * f1 + f2;
			payloads.push_back(payload);
		}
		Resterise(payloads);
	}
	UpdateTexture();
}

unsigned int CPURenderer::UploadVertices(const std::vector<Vertex>& vertexData)
{
	unsigned int buffer = GenerateBuffer();
	BindVertexBuffer(buffer);
	verticesMap[activeVertexBuffer] = vertexData;
	UnbindVertexBuffer();
	return buffer;
}

unsigned int CPURenderer::UploadIndices(const std::vector<unsigned short>& indexData)
{
	unsigned int buffer = GenerateBuffer();
	BindIndexBuffer(buffer);
	indicesMap[activeIndexBuffer] = indexData;
	UnbindIndexBuffer();
	return buffer;
}

unsigned int CPURenderer::UploadTexture(Texture* texture)
{
	unsigned int buffer = GenerateBuffer();
	textureMap[buffer] = texture;
	return buffer;
}

void CPURenderer::SetUniform(std::string s, glm::mat4 m)
{
	uniform4fM[s] = m;
}

void CPURenderer::SetUniform(std::string s, int i)
{
	uniform1i[s] = i;
}

void CPURenderer::SetUniform(std::string s, glm::vec3 v)
{
	uniform3fv[s] = v;
}

glm::mat4 CPURenderer::GetUniform4fM(std::string s)
{
	return uniform4fM[s];
}

int CPURenderer::GetUnform1i(std::string s)
{
	return uniform1i[s];
}

glm::vec3 CPURenderer::GetUnform3fv(std::string s)
{
	return uniform3fv[s];
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

