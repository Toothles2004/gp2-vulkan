#pragma once
#include "Window.h"
#include "Device.h"
#include "GameObject.h"
#include "Renderer.h"

// std includes
#include <memory>
#include <vector>

namespace lve
{
	class Application final
	{
	public:
		static constexpr int m_WIDTH{ 800 };
		static constexpr int m_HEIGHT{ 600 };

		Application();
		~Application();

		Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

		void Run();

	private:
		void LoadGameObjects();
		void RandomizeTerrain();

		Window m_Window{ m_WIDTH, m_HEIGHT, "Hello Vulkan!" };
		Device m_Device{ m_Window };
		Renderer m_Renderer{ m_Window, m_Device };
		std::vector<GameObject> m_GameObjects;
		std::vector<GameObject> m_GameObjects2D;
	};
}
