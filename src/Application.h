#pragma once
#include "Window.h"
#include "Pipeline.h"
#include "Device.h"

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
		Device m_Device{ m_Window };
		Pipeline m_Pipeline
		{
			m_Device,
			"Shaders/SimpleShader.vert.spv",
			"Shaders/SimpleShader.frag.spv",
			Pipeline::DefaultPipelineConfigInfo(m_WIDTH, m_HEIGHT)
		};
	};
}
