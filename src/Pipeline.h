#pragma once
#include "Device.h"

// std includes
#include <string>
#include <vector>

namespace lve
{
	struct PipelineConfigInfo
	{
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo(PipelineConfigInfo&&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(PipelineConfigInfo&&) = delete;

		VkPipelineViewportStateCreateInfo viewportInfo{};
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
		VkPipelineMultisampleStateCreateInfo multisampleInfo{};
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
		std::vector<VkDynamicState> dynamicStateEnables{};
		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class Pipeline final
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

		static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);
		void Bind(VkCommandBuffer commandBuffer);

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
