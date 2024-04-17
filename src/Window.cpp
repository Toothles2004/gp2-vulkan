#include "Window.h"
#include "KeyboardInput.h"

#include <stdexcept>

namespace lve
{
	Window::Window(int width, int height, std::string name)
		: m_Width(width),
		m_Height(height),
		m_WindowName(name)
	{
		InitWindow();
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create window surface!");
		}
	}

	void Window::FrameBufferResizedCallback(GLFWwindow* window, int width, int height)
	{
		auto currentWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		currentWindow->m_FrameBufferResized = true;
		currentWindow->m_Width = width;
		currentWindow->m_Height = height;
	}

	void Window::InitWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_Window = glfwCreateWindow(m_Width, m_Height, m_WindowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_Window, this);
		glfwSetFramebufferSizeCallback(m_Window, FrameBufferResizedCallback);

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			void* pUser = glfwGetWindowUserPointer(window);
			KeyboardInput* vBase = static_cast<KeyboardInput*>(pUser);
			vBase->KeyEvent(key, scancode, action, mods);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
		{
			void* pUser = glfwGetWindowUserPointer(window);
			KeyboardInput* vBase = static_cast<KeyboardInput*>(pUser);
			vBase->MouseMove(window, xpos, ypos);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			void* pUser = glfwGetWindowUserPointer(window);
			KeyboardInput* vBase = static_cast<KeyboardInput*>(pUser);
			vBase->MouseEvent(window, button, action, mods);
		});
	}
}
