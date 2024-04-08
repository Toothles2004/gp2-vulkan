#pragma once
#include "Device.h"

// std includes
#include <string>
#include <vector>

namespace lve
{
	struct PipelineConfigInfo
	{
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineViewportStateCreateInfo viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class Pipeline
	{
	public:
		Pipeline
		(
			Device & device, 
			const std::string& vertFilePath, 
			const std::string fragFilePath, 
			const PipelineConfigInfo& configInfo
		);

		~Pipeline();

		static PipelineConfigInfo DefaultPipelineConfigInfo(uint32_t width, uint32_t height);

		Pipeline(const Pipeline&) = delete;
		Pipeline(Pipeline&&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;
		Pipeline& operator=(Pipeline&&) = delete;

	private:
		static std::vector<char> ReadFile(const std::string& filePath);

		void CreateGraphicsPipeline
		(
			const std::string& vertFilePath,
			const std::string& fragFilePath, 
			const PipelineConfigInfo& configInfo
		);

		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		Device& m_Device;
		VkPipeline m_GraphicsPipeline;
		VkShaderModule m_VertShaderModule;
		VkShaderModule m_FragShaderModule;
	};
}
