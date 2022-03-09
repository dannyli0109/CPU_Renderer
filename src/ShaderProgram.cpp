#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(std::string vertexFilename, std::string fragmentFilename)
{
	this->initialised = true;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	program = glCreateProgram();

	std::string vertexSource = LoadFileAsString(vertexFilename);
	std::string fragmentSource = LoadFileAsString(fragmentFilename);

	const char* vertexSourceC = vertexSource.c_str();
	int vertexSourceLength = (int)vertexSource.size();

	glShaderSource(vertexShader, 1, &vertexSourceC, &vertexSourceLength);
	glCompileShader(vertexShader);

	GLchar errorLog[256];
	GLint compileSuccess;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileSuccess);
	if (compileSuccess == GL_FALSE)
	{
		std::cout << "Error compiling vertex shader: " << vertexFilename << std::endl;
		glGetShaderInfoLog(vertexShader, 256, nullptr, errorLog);
		std::cout << errorLog;
		initialised = false;
	}


	const char* fragmentSourceC = fragmentSource.c_str();
	int fragmentSourceeLength = (int)fragmentSource.size();

	glShaderSource(fragmentShader, 1, &fragmentSourceC, &fragmentSourceeLength);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileSuccess);
	if (compileSuccess == GL_FALSE)
	{
		std::cout << "Error compiling fragment shader: " << fragmentFilename << std::endl;
		glGetShaderInfoLog(fragmentShader, 256, nullptr, errorLog);
		std::cout << errorLog;
		initialised = false;
	}


	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &compileSuccess);

	if (compileSuccess == GL_FALSE)
	{
		std::cout << "Error linking shaders: " << vertexFilename << " and " << fragmentFilename << std::endl;
		glGetProgramInfoLog(program, 256, nullptr, errorLog);
		std::cout << errorLog;
		initialised = false;
	}

	if (initialised)
	{
		std::cout << "Successfully loaded/linked shaders " << vertexFilename << " and " << fragmentFilename << "!" << std::endl;
	}
}

ShaderProgram::~ShaderProgram()
{
	if (initialised)
	{
		glDeleteProgram(program);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
}

void ShaderProgram::UseProgram()
{
	glUseProgram(program);
}

GLuint ShaderProgram::GetUniformLocation(std::string varName)
{
	return glGetUniformLocation(program, varName.c_str());
}

void ShaderProgram::SetInt(std::string s, int n)
{
	GLuint texLocation = GetUniformLocation(s.c_str());
	glUniform1i(texLocation, n);
}

void ShaderProgram::ClearPrograms()
{
	glUseProgram(0);
}
