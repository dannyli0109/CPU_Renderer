#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(const std::string filename)
{
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, 4);
	channels = 4;
	pixels = (unsigned char*)malloc(width * height * channels);
	memcpy(pixels, data, width * height * channels);
	stbi_image_free(data);
}

glm::vec4 Texture::GetColor(float x, float y)
{

	int xI = (int)round(x * width) % width;
	int yI = (int)round(y * height) % height;

	unsigned char* c = pixels + (width * yI + xI) * channels;
	return { (short)c[0] / 255.0f , (short)c[1] / 255.0f, (short)c[2] / 255.0f, (short)c[3] / 255.0f };
}

Texture::~Texture()
{
	if (pixels)
	{
		free(pixels);
	}
	pixels = nullptr;
}
