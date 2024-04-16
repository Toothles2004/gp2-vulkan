#pragma once
#include "Device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std include
#include <vector>

namespace lve
{
	class Mesh final
	{
	public:
		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};

		struct Data
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};
		};
		
		Mesh(Device& device, const Data& builder);
		~Mesh();

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);

		Mesh(const Mesh&) = delete;
		Mesh(Mesh&&) = delete;
		Mesh& operator=(const Mesh&) = delete;
		Mesh& operator=(Mesh&&) = delete;

	private:
		void CreateVertexBuffers(const std::vector<Vertex>& vertices);
		void CreateIndexBuffer(const std::vector<uint32_t>& indices);

		Device& m_Device;
		VkBuffer m_VertrexBuffer;
		VkDeviceMemory m_VertexBufferMemory;
		uint32_t m_VertexCount;

		bool m_HasIndexBuffer = false;
		VkBuffer m_IndexBuffer;
		VkDeviceMemory m_IndexBufferMemory;
		uint32_t m_IndexCount;
	};
}
