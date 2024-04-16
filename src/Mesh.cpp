#include "Mesh.h"

namespace lve
{
	Mesh::Mesh(Device& device, const Data& builder)
		: m_Device{ device }
	{
		CreateVertexBuffers(builder.vertices);
		CreateIndexBuffer(builder.indices);
	}

	Mesh::~Mesh()
	{
		vkDestroyBuffer(m_Device.GetDevice(), m_VertrexBuffer, nullptr);
		vkFreeMemory(m_Device.GetDevice(), m_VertexBufferMemory, nullptr);

		if(m_HasIndexBuffer)
		{
			vkDestroyBuffer(m_Device.GetDevice(), m_IndexBuffer, nullptr);
			vkFreeMemory(m_Device.GetDevice(), m_IndexBufferMemory, nullptr);
		}
	}

	std::vector<VkVertexInputBindingDescription> Mesh::Vertex::GetBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescription(1);
		bindingDescription[0].binding = 0;
		bindingDescription[0].stride = sizeof(Vertex);
		bindingDescription[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	std::vector<VkVertexInputAttributeDescription> Mesh::Vertex::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}

	void Mesh::Bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { m_VertrexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if(m_HasIndexBuffer)
		{
			vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void Mesh::Draw(VkCommandBuffer commandBuffer)
	{
		if(m_HasIndexBuffer)
		{
			vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
		}
	}

	void Mesh::CreateVertexBuffers(const std::vector<Vertex>& vertices)
	{
		m_VertexCount = static_cast<uint32_t>(vertices.size());
		assert(m_VertexCount >= 3 && "Vertex count must be at least 3 for a triangle");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;

		VkBuffer stagingBuffer{};
		VkDeviceMemory stagingBufferMemory{};

		m_Device.CreateBuffer
		(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);

		// Copy vertex data to staging buffer
		void* data;
		vkMapMemory(m_Device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_Device.GetDevice(), stagingBufferMemory);

		m_Device.CreateBuffer
		(
			bufferSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_VertrexBuffer,
			m_VertexBufferMemory
		);

		m_Device.CopyBuffer(stagingBuffer, m_VertrexBuffer, bufferSize);

		vkDestroyBuffer(m_Device.GetDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device.GetDevice(), stagingBufferMemory, nullptr);
	}

	void Mesh::CreateIndexBuffer(const std::vector<uint32_t>& indices)
	{
		m_IndexCount = static_cast<uint32_t>(indices.size());
		m_HasIndexBuffer = m_IndexCount > 0;

		if(!m_HasIndexBuffer)
		{
			return;
		}

		VkDeviceSize bufferSize = sizeof(indices[0]) * m_IndexCount;

		VkBuffer stagingBuffer{};
		VkDeviceMemory stagingBufferMemory{};

		m_Device.CreateBuffer
		(
			bufferSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer,
			stagingBufferMemory
		);

		// Copy vertex data to staging buffer
		void* data;
		vkMapMemory(m_Device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
		vkUnmapMemory(m_Device.GetDevice(), stagingBufferMemory);

		m_Device.CreateBuffer
		(
			bufferSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_IndexBuffer,
			m_IndexBufferMemory
		);

		m_Device.CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

		vkDestroyBuffer(m_Device.GetDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device.GetDevice(), stagingBufferMemory, nullptr);
	}
}
