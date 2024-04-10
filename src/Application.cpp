#include "Application.h"

// std
#include <stdexcept>
#include <array>
#include <cassert>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve
{
	struct SimplePushConstantData
	{
		glm::mat2 transform{1.f};
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	Application::Application()
	{
		LoadGameObjects();
		CreatePipelineLayout();
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	Application::~Application()
	{
		vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
	}

	void Application::Run()
	{
		while(!m_Window.ShouldClose())
		{
			glfwPollEvents();
			DrawFrame();
		}

		vkDeviceWaitIdle(m_Device.GetDevice());
	}

	void Application::LoadGameObjects()
	{
		std::vector<Mesh::Vertex> vertices
		{
			{{0.0f, -0.5f}, {1.f, 0.f, 0.f}},
			{{0.5f, 0.5f}, {0.f, 1.f, 0.f}},
			{{-0.5f, 0.5f}, {0.f, 0.f, 1.f}}
		};

		auto mesh = std::make_shared<Mesh>(m_Device, vertices);

		auto triangle = GameObject::CreateGameObject();
		triangle.mesh = mesh;
		triangle.color = {0.1f, 0.8f, 0.1f};
		triangle.transform2D.translation.x = 0.2f;
		triangle.transform2D.scale = {2.f, 0.5f};
		triangle.transform2D.rotation = 0.25f * glm::two_pi<float>();

		m_GameObjects.push_back(std::move(triangle));
	}

	void Application::CreatePipelineLayout()
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

		if(vkCreatePipelineLayout(m_Device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void Application::CreatePipeline()
	{
		assert(m_SwapChain != nullptr && "Cannot create pipeline before swap chain");
		assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = m_SwapChain->GetRenderPass();
		pipelineConfig.pipelineLayout = m_PipelineLayout;
		m_Pipeline = std::make_unique<Pipeline>(m_Device, "Shaders/SimpleShader.vert.spv", "Shaders/SimpleShader.frag.spv", pipelineConfig);
	}

	void Application::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(m_SwapChain->GetImageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_Device.GetCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if(vkAllocateCommandBuffers(m_Device.GetDevice(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void Application::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_Device.GetDevice(), m_Device.GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
		m_CommandBuffers.clear();	
	}

	void Application::DrawFrame()
	{
		uint32_t imageIndex;
		auto result = m_SwapChain->AcquireNextImage(&imageIndex);

		if(result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}

		if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire next image!");
		}

		RecordCommandBuffer(imageIndex);
		result = m_SwapChain->SubmitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);

		if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.WasWindowResized())
		{
			m_Window.ResetWindowResizedFlag();
			RecreateSwapChain();
			return;
		}

		if(result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present image!");
		}
	}

	void Application::RecreateSwapChain()
	{
		auto extent = m_Window.GetExtent();

		while(extent.width == 0 || extent.height == 0)
		{
			extent = m_Window.GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_Device.GetDevice());

		if (m_SwapChain == nullptr)
		{
			m_SwapChain.reset(nullptr);
			m_SwapChain = std::make_unique<SwapChain>(m_Device, extent);
		}
		else
		{
			m_SwapChain.reset(nullptr);
			m_SwapChain = std::make_unique<SwapChain>(m_Device, extent, std::move(m_SwapChain));

			if(m_SwapChain->GetImageCount() != m_CommandBuffers.size())
			{
				FreeCommandBuffers();
				CreateCommandBuffers();
			}
		}	

		// if render pass compatible do nothing else
		CreatePipeline();
	}

	void Application::RecordCommandBuffer(int imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_SwapChain->GetRenderPass();
		renderPassInfo.framebuffer = m_SwapChain->GetFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_SwapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_SwapChain->GetSwapChainExtent().width);
		viewport.height = static_cast<float>(m_SwapChain->GetSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{{0, 0}, m_SwapChain->GetSwapChainExtent()};
		vkCmdSetViewport(m_CommandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(m_CommandBuffers[imageIndex], 0, 1, &scissor);

		RenderGameObjects(m_CommandBuffers[imageIndex]);

		vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);
		if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	void Application::RenderGameObjects(VkCommandBuffer commandBuffer)
	{
		m_Pipeline->Bind(commandBuffer);

		for(auto& object : m_GameObjects)
		{
			object.transform2D.rotation = glm::mod(object.transform2D.rotation + 0.01f, glm::two_pi<float>());

			SimplePushConstantData push{};
			push.offset = object.transform2D.translation;
			push.color = object.color;
			push.transform = object.transform2D.Mat2();

			vkCmdPushConstants
			(
				commandBuffer,
				m_PipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push
			);
			object.mesh->Bind(commandBuffer);
			object.mesh->Draw(commandBuffer);
		}
	}
}
