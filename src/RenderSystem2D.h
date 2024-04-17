#pragma once
#include "Camera.h"
#include "Device.h"
#include "FrameInfo.h"
#include "GameObject.h"
#include "Pipeline.h"

#include <memory>
#include <vector>
namespace lve
{
	class RenderSystem2D
	{
    public:
        RenderSystem2D(Device& device, VkRenderPass renderPass);
        ~RenderSystem2D();

        void RenderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects);

        RenderSystem2D(const RenderSystem2D&) = delete;
        RenderSystem2D(RenderSystem2D&&) noexcept = delete;
        RenderSystem2D& operator=(const RenderSystem2D&) = delete;
        RenderSystem2D& operator=(RenderSystem2D&&) noexcept = delete;

    private:
        void CreatePipelineLayout();
        void CreatePipeline(VkRenderPass renderPass);

		Device& m_Device;
		std::unique_ptr<Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
	};
}

