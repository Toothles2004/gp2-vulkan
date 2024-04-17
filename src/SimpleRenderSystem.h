#pragma once
#include "Pipeline.h"
#include "Device.h"
#include "GameObject.h"
#include "Camera.h"
#include "FrameInfo.h"

// std includes
#include <memory>
#include <vector>

namespace lve
{
	class SimpleRenderSystem final
	{
	public:

		SimpleRenderSystem(Device& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		void RenderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects);

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem(SimpleRenderSystem&&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(SimpleRenderSystem&&) = delete;

	private:
		void CreatePipelineLayout();
		void CreatePipeline(VkRenderPass renderPass);

		Device& m_Device;
		std::unique_ptr<Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}
