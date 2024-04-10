#include "Application.h"
#include "SimpleRenderSystem.h"

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

		while(!m_Window.ShouldClose())
		{
			glfwPollEvents();

			if(auto commandBuffer = m_Renderer.BeginFrame())
			{
				m_Renderer.BeginSwapChainRenderPass(commandBuffer);
				simpleRenderSystem.RenderGameObjects(commandBuffer, m_GameObjects);
				m_Renderer.EndSwapChainRenderPass(commandBuffer);
				m_Renderer.EndFrame();
			}
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
}
