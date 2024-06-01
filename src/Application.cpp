#include "Application.h"
#include "SimpleRenderSystem.h"
#include "Camera.h"
#include "KeyboardInput.h"
#include "Buffer.h"

// std
#include <stdexcept>
#include <chrono>
#include <array>
#include <cassert>
#include <numeric>

//library
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "RenderSystem2D.h"

namespace lve
{
	struct GlobalUbo
	{
		glm::mat4 projectionView{1.f};
		glm::vec3 lightDirection = glm::normalize(glm::vec3{1.f, -3.f, -1.f});
	};

	Application::Application()
	{
		LoadGameObjects();
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);

		for(int index{}; index < uboBuffers.size(); ++index)
		{
			uboBuffers[index] = std::make_unique<Buffer>
			(
				m_Device,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, automatically flush memory
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);

			uboBuffers[index]->Map();
		}

		SimpleRenderSystem simpleRenderSystem{m_Device, m_Renderer.GetSwapChainRenderPass()};
		RenderSystem2D renderSystem2D{m_Device, m_Renderer.GetSwapChainRenderPass()};
        Camera camera{};

        camera.SetViewTarget(glm::vec3(1.0f, 3.0f, 0.0f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewerObject = GameObject::CreateGameObject();
        KeyboardInput cameraController{};
		/*void* pUser = glfwGetWindowUserPointer(m_Window.GetGLFWwindow());
		KeyboardInput* cameraController = static_cast<KeyboardInput*>(pUser);*/

        auto currentTime = std::chrono::high_resolution_clock::now();

		while(!m_Window.ShouldClose())
		{
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            //cameraController.MoveInPlaneXYZ(m_Window.GetGLFWwindow(), frameTime, viewerObject);
			cameraController.Update(viewerObject, frameTime);
            camera.SetViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = m_Renderer.GetAspectRatio();

            camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

			if(auto commandBuffer = m_Renderer.BeginFrame())
			{
				int frameIndex = m_Renderer.GetFrameIndex();
				FrameInfo frameInfo
				{
					frameIndex,
					frameTime,
					commandBuffer,
					camera
				};

				// Update
				GlobalUbo ubo{};
				ubo.projectionView = camera.GetProjectionMatrix() * camera.GetViewMatrix();
				uboBuffers[frameIndex]->WriteToBuffer(&ubo);
				uboBuffers[frameIndex]->Flush();

				// Render
				m_Renderer.BeginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.RenderGameObjects(frameInfo, m_GameObjects);
				renderSystem2D.RenderGameObjects(frameInfo, m_GameObjects2D);
				m_Renderer.EndSwapChainRenderPass(commandBuffer);
				m_Renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(m_Device.GetDevice());
	}

	void Application::LoadGameObjects()
	{
		int rows{ 128 };
		int columns{ 128 };
		float height{ 10 };
		float width{ 10 };
		
		std::pair<Device&, Mesh::Data> temp = Mesh::GeneratePerlinNoiseMap(m_Device, rows, columns, height, width, 0.08f);
		std::shared_ptr<Mesh> perlinMap = std::make_unique<Mesh>(temp.first, temp.second);
		auto perlinNoise{ GameObject::CreateGameObject() };
		perlinNoise.mesh = perlinMap;
		perlinNoise.transform.translation = { -15.f, 5.0f, 10.0f };
		perlinNoise.transform.scale = glm::vec3{ 1.f };
		m_GameObjects.push_back(std::move(perlinNoise));

		std::shared_ptr<Mesh> mountainTerrain = Mesh::CreateTerrain(m_Device, rows, columns, temp.second);
		auto terrain { GameObject::CreateGameObject() };
		terrain.mesh = mountainTerrain;
		terrain.transform.translation = { -5.f, 5.0f, 10.0f };
		terrain.transform.scale = glm::vec3{ 1.f };
		m_GameObjects.push_back(std::move(terrain));

		std::shared_ptr<Mesh> mesh = Mesh::CreateModelFromFile(m_Device, "\\Models\\flatVase.obj");
		auto flatVase{ GameObject::CreateGameObject() };
        flatVase.mesh = mesh;
        flatVase.transform.translation = { -0.5f, 0.5f, 2.5f };
		flatVase.transform.scale = glm::vec3{ 3.f };
        m_GameObjects.push_back(std::move(flatVase));

		mesh = Mesh::CreateModelFromFile(m_Device, "\\Models\\smoothVase.obj");
		auto smoothVase{ GameObject::CreateGameObject() };
		smoothVase.mesh = mesh;
		smoothVase.transform.translation = { 0.5f, 0.5f, 2.5f };
		smoothVase.transform.scale = glm::vec3{ 3.f };
		m_GameObjects.push_back(std::move(smoothVase));

		mesh = Mesh::CreateModelFromFile(m_Device, "\\Models\\smoothVase.obj");
		GameObject gameObject{ GameObject::CreateGameObject() };
		gameObject.mesh = mesh;
		gameObject.transform.translation = { 0.8f, 0.8f, .5f };
		gameObject.transform.scale = { 1.5f, 1.5f, 1.5f };
		m_GameObjects2D.push_back(std::move(gameObject));

		mesh = Mesh::CreateModelFromFile(m_Device, "\\Models\\cube.obj");
		GameObject cube{ GameObject::CreateGameObject() };
		cube.mesh = mesh;
		cube.transform.translation = { -0.7f, 0.5f, 0.2f };
		cube.transform.scale = { 0.2f, 0.2f, 0.2f };
		m_GameObjects2D.push_back(std::move(cube));
	}
}
