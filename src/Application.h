#pragma once
#include "Window.h"
#include "Pipeline.h"
#include "Device.h"
#include "SwapChain.h"
#include "Mesh.h"
#include "GameObject.h"

// std includes
#include <memory>
#include <vector>

namespace lve
{
	class Application final
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
		void LoadGameObjects();
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void DrawFrame();
		void RecreateSwapChain();
		void RecordCommandBuffer(int imageIndex);
		void RenderGameObjects(VkCommandBuffer commandBuffer);

		Window m_Window{ m_WIDTH, m_HEIGHT, "Hello Vulkan!" };
		Device m_Device{ m_Window };
		std::unique_ptr<SwapChain> m_SwapChain;
		std::unique_ptr<Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		std::vector<GameObject> m_GameObjects;
	};
}
