#include "Shader.h"
#include "Renderer.h"

glm::vec3 PhongVertexNormalFragmentShader(Renderer* r, FragmentShaderPayload& payload)
{
	glm::vec3 ka = glm::vec3(0.005, 0.005, 0.005);
	glm::vec3 ks = glm::vec3(0.7937, 0.7937, 0.7937);
	Texture* diffuse = r->GetTexture("diffuseTexture");
	glm::vec3 eyePos = r->GetUnform3fv("eyePosition");
	glm::vec3 kd = diffuse->GetColor(payload.texCoords.x, payload.texCoords.y);
	//glm::vec3 kd = payload.color;
	Light l1 = Light{ {20, 20, 20}, {500, 500, 500} };
	Light l2 = Light{ {-20, 20, 0}, {500, 500, 500} };
	glm::vec3 ambientLightIntensity = { 10, 10, 10 };

	std::vector<Light> lights = { l1, l2 };

	glm::vec3 result = { 0, 0, 0 };
	float p = 150;


	for (int i = 0; i < lights.size(); i++)
	{
		Light l = lights[i];
		//std::cout << payload.viewPos << std::endl;
		float r2 = glm::distance2(l.position, payload.modelPos);
		glm::vec3 toLight = glm::normalize(l.position - payload.modelPos);
		float cosTheta = glm::dot(glm::normalize(payload.normal), toLight);
		glm::vec3 ld = kd * (l.intensity / r2) * std::max(0.0f, cosTheta);

		 //ls = ks(I/r^2)* max(0, n dot h)^p;
		glm::vec3 viewDir = glm::normalize(eyePos - payload.modelPos);
		glm::vec3 h = glm::normalize(viewDir + toLight);
		float spec = glm::dot(glm::normalize(payload.normal), h);
		glm::vec3 ls =  ks * (l.intensity / r2) * std::pow(std::max(0.0f, spec), p);

		result += (ld + ls);
	}
	glm::vec3 la = ka * ambientLightIntensity;
	result += la;
	return result;
}

glm::vec3 PhongNormalMapFragmentShader(Renderer* r, FragmentShaderPayload& payload)
{
	glm::vec3 ka = glm::vec3(0.005, 0.005, 0.005);
	Texture* specular = r->GetTexture("specularTexture");
	glm::vec3 ks = specular->GetColor(payload.texCoords.x, payload.texCoords.y);
	Texture* diffuse = r->GetTexture("diffuseTexture");
	glm::vec3 eyePos = r->GetUnform3fv("eyePosition");
	glm::vec3 kd = diffuse->GetColor(payload.texCoords.x, payload.texCoords.y);
	//glm::vec3 kd = payload.color;
	Light l1 = Light{ {20, 20, 20}, {500, 500, 500} };
	Light l2 = Light{ {-20, 20, 0}, {500, 500, 500} };
	glm::vec3 ambientLightIntensity = { 10, 10, 10 };

	std::vector<Light> lights = { l1, l2 };

	glm::vec3 result = { 0, 0, 0 };
	float p = 150;

	Texture* normalTexture = r->GetTexture("normalTexture");
	glm::vec3 normalTap = normalTexture->GetColor(payload.texCoords.x, payload.texCoords.y);
	glm::vec3 mapNormal = { normalTap.x * 2.0f - 1.0f, normalTap.y * 2.0f - 1.0f, normalTap.z * 2.0f - 1.0f };
	glm::vec3 normal = glm::normalize(glm::vec4(payload.tbn * mapNormal, 0));

	for (int i = 0; i < lights.size(); i++)
	{
		Light l = lights[i];
		//std::cout << payload.viewPos << std::endl;
		float r2 = glm::distance2(l.position, payload.modelPos);
		glm::vec3 toLight = glm::normalize(l.position - payload.modelPos);
		float cosTheta = glm::dot(glm::normalize(normal), toLight);
		glm::vec3 ld = kd * (l.intensity / r2) * std::max(0.0f, cosTheta);

		//ls = ks(I/r^2)* max(0, n dot h)^p;
		glm::vec3 viewDir = glm::normalize(eyePos - payload.modelPos);
		glm::vec3 h = glm::normalize(viewDir + toLight);
		float spec = glm::dot(glm::normalize(normal), h);
		glm::vec3 ls = ks * (l.intensity / r2) * std::pow(std::max(0.0f, spec), p);

		result += (ld + ls);
	}
	glm::vec3 la = ka * ambientLightIntensity;
	result += la;
	return result;
}

glm::vec3 NormalMapFragmentShader(Renderer* r, FragmentShaderPayload& payload)
{
	Texture* normal = r->GetTexture("normalTexture");
	glm::vec3 normalTap = normal->GetColor(payload.texCoords.x, payload.texCoords.y);
	glm::vec3 mapNormal = { normalTap.x * 2.0f - 1.0f, normalTap.y * 2.0f - 1.0f, normalTap.z * 2.0f - 1.0f };

	//glm::mat3 tbn = glm::transpose(payload.out.)

	return glm::normalize(glm::vec4(payload.tbn * mapNormal, 0));
}

glm::vec4 VertexNormalVertexShader(Renderer* r, VertexShaderPayload& payload)
{
	//glm::mat3 tbn = glm::transpose(payload.out.)
	glm::mat4 model = r->GetUniform4fM("modelMatrix");
	glm::mat4 view = r->GetUniform4fM("viewMatrix");
	glm::mat4 projection = r->GetUniform4fM("projectionMatrix");
	glm::vec3 modelPos = model * payload.position;
	payload.out.modelPos = modelPos;

	glm::mat4 invTransform = glm::transpose(glm::inverse(view * model));
	payload.out.vertexNormal = invTransform * glm::vec4(payload.normal, 0);

	return projection * view * model * payload.position;
}

glm::vec4 NormalMapVertexShader(Renderer* r, VertexShaderPayload& payload)
{
	glm::mat4 model = r->GetUniform4fM("modelMatrix");
	glm::mat4 view = r->GetUniform4fM("viewMatrix");
	glm::mat4 projection = r->GetUniform4fM("projectionMatrix");
	glm::vec3 modelPos = model * payload.position;
	payload.out.modelPos = modelPos;
	glm::vec3 t = glm::normalize(model * glm::vec4(payload.tangent, 0.0f));
	glm::vec3 b = glm::normalize(model * glm::vec4(payload.bitangent, 0.0f));
	glm::vec3 n = glm::normalize(model * glm::vec4(payload.normal, 0.0f));
	payload.out.tbn = {t, b, n};


	return projection * view * model * payload.position;
}

VertexShaderPayload::VertexShaderPayload(Vertex& v)
{
	//position(v.position), normal(v.normal), color(v.color), texCoords(v.uv), tangent(v.vertTangent), bitangent(v.vertBitangent)
	position = v.position;
	normal = v.normal;
	color = v.color;
	texCoords = v.uv;
	tangent = v.vertTangent;
	bitangent = v.vertBitangent;
}
