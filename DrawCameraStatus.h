#pragma once
#include <GLFW/glfw3.h>
#include <memory>

class LightMoveCallback;

class DrawCameraStatus
{
	public:
		DrawCameraStatus();
		void render(GLFWwindow* window, std::shared_ptr<LightMoveCallback> callback);
};