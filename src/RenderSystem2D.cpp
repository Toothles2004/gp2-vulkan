#include "RenderSystem2D.h"

// std
#include <stdexcept>
#include <array>
#include <cassert>

//library
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


namespace lve
{
	struct SimplePushConstantData
	{
		glm::mat4 transform{ 1.f };
	};

	RenderSystem2D::RenderSystem2D(Device& device, VkRenderPass renderPass)
		: m_Device(device)
	{
		CreatePipelineLayout();
		CreatePipeline(renderPass);
	}

	RenderSystem2D::~RenderSystem2D()
	{
		vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
	}

	void RenderSystem2D::CreatePipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(m_Device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void RenderSystem2D::CreatePipeline(VkRenderPass renderPass)
	{
		assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = m_PipelineLayout;
		m_Pipeline = std::make_unique<Pipeline>(m_Device, "Shaders/SimpleShader.vert.spv", "Shaders/SimpleShader.frag.spv", pipelineConfig);
	}

	void RenderSystem2D::RenderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects)
	{
		m_Pipeline->Bind(frameInfo.commandBuffer);

		for (auto& object : gameObjects)
		{
			SimplePushConstantData push{};
			push.transform = object.transform.Mat4();

			vkCmdPushConstants
			(
				frameInfo.commandBuffer,
				m_PipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push
			);
			object.mesh->Bind(frameInfo.commandBuffer);
			object.mesh->Draw(frameInfo.commandBuffer);
		}
	}
}
