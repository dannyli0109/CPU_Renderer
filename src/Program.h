#pragma once
#include "Graphics.h"
#include "GUI.h"
#include "CPURenderer.h"
#include "Utilities.h"
#include "Texture.h"
class Program
{
public:
	int Init();
	void Update();
	void End();
	void Draw();
private:
	void InitGUI();
	void UpdateGUI();
	void EndGUI();
	CPURenderer* renderer;
	
	GLFWwindow* window;
	glm::vec2 viewportSize;
};