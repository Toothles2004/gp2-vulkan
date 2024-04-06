#include "Application.h"

namespace lve
{
	void Application::Run()
	{
		while(!m_Window.ShouldClose())
		{
			glfwPollEvents();
		}
	}
}