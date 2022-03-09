#pragma once
#include "Graphics.h"
#include <string>
#include "Utilities.h"

class ShaderProgram
{
public:
	ShaderProgram() = delete;

	ShaderProgram(std::string vertexFilename, std::string fragmentFilename);

	~ShaderProgram();

	void UseProgram();

	GLuint GetUniformLocation(std::string varName);

	void SetInt(std::string s, int n);

	static void ClearPrograms();

private:
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint program;
	bool initialised = false;
};