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

    std::unique_ptr<Mesh> CreateCubeModel(Device& device, glm::vec3 offset)
	{
        Mesh::Data modelBuilder{};
        modelBuilder.vertices = {
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        };

        for (auto& v : modelBuilder.vertices) 
        {
            v.position += offset;
        }

        modelBuilder.indices = 
        {
        	0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
        	12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21
        };

        return std::make_unique<Mesh>(device, modelBuilder);
    }

	void Application::LoadGameObjects()
	{
		std::shared_ptr<Mesh> mesh = CreateCubeModel(m_Device, {0, 0, 0});

        auto cube = GameObject::CreateGameObject();
        cube.mesh = mesh;
        cube.transform.translation = { 0.f, 0.f, 2.5f };
        cube.transform.scale = { 0.5f, 0.5f, 0.5f };
        m_GameObjects.push_back(std::move(cube));
	}
}
