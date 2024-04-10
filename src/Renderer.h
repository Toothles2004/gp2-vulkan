#pragma once
#include "Window.h"
#include "Device.h"
#include "SwapChain.h"

// std includes
#include <memory>
#include <vector>
#include <cassert>

namespace lve
{
	class Renderer final
	{
	public:

		Renderer(Window& window, Device& device);
		~Renderer();

		VkRenderPass GetSwapChainRenderPass() const { return m_SwapChain->GetRenderPass(); }
		bool IsFrameInProgress() const { return m_IsFrameStarted; }
		VkCommandBuffer GetCurrentCommandBuffer() const
		{
			assert(m_IsFrameStarted && "Cannot get command buffer when frame not in progress.");
			return m_CommandBuffers[m_CurrentFrameIndex];
		}

		VkCommandBuffer BeginFrame();
		void EndFrame();
		void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

		int GetFrameIndex() const
		{
			assert(m_IsFrameStarted && "Cannot get frame index when frame not in progress.");
			return m_CurrentFrameIndex;
		}

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) = delete;

	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapChain();

		Window& m_Window;
		Device& m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		uint32_t m_CurrentImageIndex;
		int m_CurrentFrameIndex{ 0 };
		bool m_IsFrameStarted{false};
	};
}
