#pragma once
#include "Window.h"
#include "Pipeline.h"

namespace lve
{
	class Application
	{
	public:
		static constexpr int m_WIDTH{ 800 };
		static constexpr int m_HEIGHT{ 600 };

		void Run();

	private:
		Window m_Window{ m_WIDTH, m_HEIGHT, "Hello Vulkan!" };
		Pipeline m_Pipeline{ "Shaders/SimpleShader.vert.spv", "Shaders/SimpleShader.frag.spv" };
	};
}
