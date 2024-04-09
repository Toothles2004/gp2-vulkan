#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <string>

namespace lve
{
	class Window final
	{
	public:
		Window(int width, int height, std::string name);
		~Window();

		bool ShouldClose() { return glfwWindowShouldClose(m_Window); }
		VkExtent2D GetExtent() { return { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) }; }
		bool WasWindowResized() { return m_FrameBufferResized; }
		void ResetWindowResizedFlag() { m_FrameBufferResized = false; }

		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

		Window(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(const Window&) = delete;
		Window& operator=(Window&&) = delete;

	private:
		static void FrameBufferResizedCallback(GLFWwindow* window, int width, int height);
		void InitWindow();

		int m_Width;
		int m_Height;
		bool m_FrameBufferResized{ false };

		std::string m_WindowName;
		GLFWwindow* m_Window;
	};
}
