#pragma once

#include <string>
#include "Matrices.h"
#include <vector>

class Texture
{
public:
	Texture(const std::string filename);
	glm::vec4 GetColor(float x, float y);
	~Texture();
private:
	int width, height, channels;
	unsigned char* pixels;
};