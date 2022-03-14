#include "Utilities.h"

std::string LoadFileAsString(std::string filename)
{
	std::stringstream fileSoFar;
	std::ifstream file(filename);

	if (file.is_open())
	{
		while (!file.eof())
		{
			std::string thisLine;
			std::getline(file, thisLine);
			fileSoFar << thisLine << std::endl;
		}
		return fileSoFar.str();
	}
	else
	{
		std::cout << "Error loading file: " << filename << std::endl;
		return "";
	}
}

void ProcessNode(aiNode* node, const aiScene* scene, std::vector<Mesh>& meshes)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		std::vector<Vertex> vertices;
		std::vector<unsigned short> indices;

		vertices.reserve(mesh->mNumVertices);
		indices.reserve(mesh->mNumFaces);


		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			vertex.position = {
				mesh->mVertices[i].x,
				mesh->mVertices[i].y,
				mesh->mVertices[i].z,
				1
			};

			if (mesh->HasVertexColors(0))
			{
				vertex.color = {
					mesh->mColors[0][i].r,
					mesh->mColors[0][i].g,
					mesh->mColors[0][i].b,
					mesh->mColors[0][i].a
				};
			}
			else
			{
				vertex.color = {
					0, 0, 0, 1
				};
			}

			if (mesh->HasNormals())
			{
				//std::cout << "test" << std::endl;
				vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
			}
			else
			{
				vertex.normal = { 0, 0, 0 };
			}

			if (mesh->mTextureCoords[0])
			{
				vertex.uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			}
			else
			{
				vertex.uv = { 0, 0 };
			}

			if (mesh->HasTangentsAndBitangents())
			{
				vertex.vertTangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
				vertex.vertBitangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
			}
			else
			{
				vertex.vertTangent = { 0, 0, 0 };
				vertex.vertBitangent = { 0, 0, 0 };
			}

			vertices.push_back(vertex);
		}

		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			{
				indices.push_back(mesh->mFaces[i].mIndices[j]);
			}
		}
		Mesh newMesh = { vertices, indices };
		meshes.push_back(newMesh);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, meshes);
	}
}


Model* LoadModel(std::string filename)
{

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return nullptr;
	}
	std::vector<Mesh> meshes;
	ProcessNode(scene->mRootNode, scene, meshes);
	return new Model({ meshes });
}
