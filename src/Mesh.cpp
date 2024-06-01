#include "Mesh.h"
#include "Utils.h"

//libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <TinyObjLoader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include "3rdParty/FastNoiseLite.h"

//std
#include <cassert>
#include <cstring>
#include <filesystem>
#include <unordered_map>

namespace std
{
	template<>
	struct hash<lve::Mesh::Vertex>
	{
		size_t operator()(lve::Mesh::Vertex const& vertex) const
		{
			size_t seed = 0;
			lve::HashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}

namespace lve
{
	void Mesh::Data::LoadModel(const std::string& filePath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warning, err;

		std::string file = __FILE__;
		const std::string path = file.substr(0, file.find_last_of("/\\")) + filePath;

		if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &err, path.c_str()))
		{
			throw std::runtime_error(warning + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for(const auto& shape : shapes)
		{
			for(const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				if(index.vertex_index >= 0)
				{
					vertex.position =
					{
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					};

					vertex.color =
					{
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2]
					};
				}

				if (index.normal_index >= 0)
				{
					vertex.normal =
					{
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}

				if (index.texcoord_index >= 0)
				{
					vertex.uv =
					{
						attrib.texcoords[2 * index.texcoord_index + 0],
						attrib.texcoords[2 * index.texcoord_index + 1],
					};
				}

				if(uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

	Mesh::Mesh(Device& device, const Data& builder)
		: m_Device{ device }
	{
		CreateVertexBuffers(builder.vertices);
		CreateIndexBuffer(builder.indices);
	}

	Mesh::~Mesh()
	{
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
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

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
		VkBuffer buffers[] = { m_VertexBuffer->GetBuffer()};
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if(m_HasIndexBuffer)
		{
			vkCmdBindIndexBuffer(commandBuffer, m_pIndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
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

	std::unique_ptr<Mesh> Mesh::CreateModelFromFile(Device& device, const std::string& filePath)
	{
		Data data{};
		data.LoadModel(filePath);

		return std::make_unique<Mesh>(device, data);
	}

	std::pair<Device&, Mesh::Data> Mesh::GeneratePerlinNoiseMap(Device& device, int rows, int columns, float height, float width, float frequency)
	{
		Data data{};
		srand(static_cast<uint32_t>(time(NULL)));
		FastNoiseLite noise( rand());
		noise.SetFrequency(frequency);
		noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
		noise.SetRotationType3D(FastNoiseLite::RotationType3D_None);
		noise.SetFractalType(FastNoiseLite::FractalType_None);
		noise.SetFractalOctaves(3);
		noise.SetFractalGain(0.4f);
		noise.SetFractalWeightedStrength(0.f);

		std::vector<float> noiseData(10);
		int index = 0;

		// Generate vertices
		for (int y{}; y < rows; ++y)
		{
			for (int x{}; x < columns; ++x)
			{
				Vertex vertex{};

				vertex.position = { static_cast<float>(x) * width / rows, 0.0f, static_cast<float>(y) * height / columns };
				vertex.color = { 1.0f, 1.0f, 1.0f };
				vertex.normal = { 0.0f, -1.0f, 0.0f };
				vertex.uv = { static_cast<float>(x) * width / rows / (columns - 1), static_cast<float>(y) * height / columns / (rows - 1) };

				noiseData[index++] =
					(
						1 * noise.GetNoise(static_cast<float>(x), static_cast<float>(y))
						+ 0.5f * noise.GetNoise(static_cast<float>(x)*2, static_cast<float>(y)*2)
						+ 0.25f * noise.GetNoise(static_cast<float>(x)*4, static_cast<float>(y)*4)
						);

				float randomColor = noiseData[index-1] / (1.f + 0.5f + 0.25f);
				vertex.color.r = randomColor;
				vertex.color.g = randomColor;
				vertex.color.b = randomColor;

				data.vertices.push_back(vertex);
				index = 0;
			}
		}

		// Generate indices
		for (int y{}; y < rows - 1; ++y)
		{
			for (int x{}; x < columns - 1; ++x)
			{
				const int topLeft = y * columns + x;
				const int topRight = topLeft + 1;
				const int bottomLeft = (y + 1) * columns + x;
				const int bottomRight = bottomLeft + 1;

				// Calculate normal for this face
				glm::vec3 edge1 = data.vertices[topRight].position - data.vertices[topLeft].position;
				glm::vec3 edge2 = data.vertices[bottomLeft].position - data.vertices[topLeft].position;
				glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

				// Assign normal to all vertices of this face
				data.vertices[topLeft].normal = normal;
				data.vertices[topRight].normal = normal;
				data.vertices[bottomLeft].normal = normal;
				data.vertices[bottomRight].normal = normal;

				data.indices.push_back(topLeft);
				data.indices.push_back(bottomLeft);
				data.indices.push_back(topRight);

				data.indices.push_back(topRight);
				data.indices.push_back(bottomLeft);
				data.indices.push_back(bottomRight);
			}
		}
		return std::pair<Device&, Mesh::Data>{device, data};
	}

	std::unique_ptr<Mesh> Mesh::CreateTerrain(Device& device, int rows, int columns, Data previousData)
	{
		Data data{};
		for (int y{}; y < rows; ++y)
		{
			for (int x{}; x < columns; ++x)
			{
				Vertex vertex{};
				float height = (previousData.vertices[y * columns + x].color.r + previousData.vertices[y * columns + x].color.g + previousData.vertices[y * columns + x].color.b) / 3;

				vertex.position = previousData.vertices[y * columns + x].position;
				vertex.position.y = height;
				vertex.color = { 0.4f, 0.3f, 0.2f };
				vertex.normal = previousData.vertices[y * columns + x].normal;
				vertex.uv = previousData.vertices[y * columns + x].uv;

				data.vertices.push_back(vertex);
			}
		}

		// Generate indices
		for (int y{}; y < rows - 1; ++y)
		{
			for (int x{}; x < columns - 1; ++x)
			{
				const int topLeft = y * columns + x;
				const int topRight = topLeft + 1;
				const int bottomLeft = (y + 1) * columns + x;
				const int bottomRight = bottomLeft + 1;

				// Calculate normal for this face
				glm::vec3 edge1 = data.vertices[topRight].position - data.vertices[topLeft].position;
				glm::vec3 edge2 = data.vertices[bottomLeft].position - data.vertices[topLeft].position;
				glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

				// Assign normal to all vertices of this face
				data.vertices[topLeft].normal = normal;
				data.vertices[topRight].normal = normal;
				data.vertices[bottomLeft].normal = normal;
				data.vertices[bottomRight].normal = normal;

				data.indices.push_back(topLeft);
				data.indices.push_back(bottomLeft);
				data.indices.push_back(topRight);

				data.indices.push_back(topRight);
				data.indices.push_back(bottomLeft);
				data.indices.push_back(bottomRight);
			}
		}

		return std::make_unique<Mesh>(device, data);
	}

	void Mesh::CreateVertexBuffers(const std::vector<Vertex>& vertices)
	{
		m_VertexCount = static_cast<uint32_t>(vertices.size());
		assert(m_VertexCount >= 3 && "Vertex count must be at least 3 for a triangle");
		VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;
		uint32_t vertexSize = sizeof(vertices[0]);

		Buffer stagingBuffer
		{
			m_Device,
			vertexSize,
			m_VertexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer((void*)vertices.data());

		m_VertexBuffer = std::make_unique<Buffer>
		(
			m_Device,
			vertexSize,
			m_VertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_Device.CopyBuffer(stagingBuffer.GetBuffer(), m_VertexBuffer->GetBuffer(), bufferSize);
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
		uint32_t indexSize = sizeof(indices[0]);

		Buffer stagingBuffer
		{
			m_Device,
			indexSize,
			m_IndexCount,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};

		stagingBuffer.Map();
		stagingBuffer.WriteToBuffer((void*)indices.data());

		m_pIndexBuffer = std::make_unique<Buffer>
			(
			m_Device,
			indexSize,
			m_IndexCount,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);

		m_Device.CopyBuffer(stagingBuffer.GetBuffer(), m_pIndexBuffer->GetBuffer(), bufferSize);
	}
}
