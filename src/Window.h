#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <string>

namespace lve
{
	class Window
	{
	public:
		Window(int width, int height, std::string name);
		~Window();

		bool ShouldClose() { return glfwWindowShouldClose(m_Window); }

		Window(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) = delete;

	private:

		void InitWindow();

		const int m_Width;
		const int m_Height;

		std::string m_WindowName;
		GLFWwindow* m_Window;
	};
}
