#pragma once
#include "Window.h"
#include "Pipeline.h"
#include "Device.h"
#include "SwapChain.h"

// std includes
#include <memory>
#include <vector>

namespace lve
{
	class Application
	{
	public:
		static constexpr int m_WIDTH{ 800 };
		static constexpr int m_HEIGHT{ 600 };

		Application();
		~Application();

		Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

		void Run();

	private:
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();
		void DrawFrame();

		Window m_Window{ m_WIDTH, m_HEIGHT, "Hello Vulkan!" };
		Device m_Device{ m_Window };
		SwapChain m_SwapChain{ m_Device, m_Window.GetExtent() };
		std::unique_ptr<Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
		std::vector<VkCommandBuffer> m_CommandBuffers;
	};
}
