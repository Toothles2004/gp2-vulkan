#include "Application.h"
#include "SimpleRenderSystem.h"
#include "Camera.h"
#include "KeyboardInput.h"

// std
#include <stdexcept>
#include <chrono>
#include <array>
#include <cassert>

//library
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lve
{
	Application::Application()
	{
		LoadGameObjects();
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		SimpleRenderSystem simpleRenderSystem{m_Device, m_Renderer.GetSwapChainRenderPass()};
        Camera camera{};

        camera.SetViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

        auto viewerObject = GameObject::CreateGameObject();
        KeyboardInput cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while(!m_Window.ShouldClose())
		{
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.MoveInPlaneXYZ(m_Window.GetGLFWwindow(), frameTime, viewerObject);
            camera.SetViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = m_Renderer.GetAspectRatio();

            camera.SetPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

			if(auto commandBuffer = m_Renderer.BeginFrame())
			{
				m_Renderer.BeginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.RenderGameObjects(commandBuffer, m_GameObjects, camera);
				m_Renderer.EndSwapChainRenderPass(commandBuffer);
				m_Renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(m_Device.GetDevice());
	}

	void Application::LoadGameObjects()
	{
		std::shared_ptr<Mesh> mesh = Mesh::CreateModelFromFile(m_Device, "C:\\Users\\leent\\Documents\\school\\DAE_GD\\semester_4\\Graphics_programming_2\\projects\\Assignment\\gp2-vulkan\\src\\Models\\flatVase.obj");

        auto flatVase = GameObject::CreateGameObject();
        flatVase.mesh = mesh;
        flatVase.transform.translation = { -0.5f, 0.5f, 2.5f };
		flatVase.transform.scale = glm::vec3{ 3.f };
        m_GameObjects.push_back(std::move(flatVase));

		mesh = Mesh::CreateModelFromFile(m_Device, "C:\\Users\\leent\\Documents\\school\\DAE_GD\\semester_4\\Graphics_programming_2\\projects\\Assignment\\gp2-vulkan\\src\\Models\\smoothVase.obj");

		auto smoothVase = GameObject::CreateGameObject();
		smoothVase.mesh = mesh;
		smoothVase.transform.translation = { 0.5f, 0.5f, 2.5f };
		smoothVase.transform.scale = glm::vec3{ 3.f };
		m_GameObjects.push_back(std::move(smoothVase));
	}
}
