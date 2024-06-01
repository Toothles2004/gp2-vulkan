#pragma once
#include "GameObject.h"
#include "Window.h"

namespace lve
{
	class KeyboardInput
	{
	public:
		KeyboardInput() = default;
		void KeyEvent(int key, int scancode, int action, int mods);
		void MouseMove(GLFWwindow* window, double xpos, double ypos);
		void MouseEvent(GLFWwindow* window, int button, int action, int mods);
		void Update(GameObject& gameObject, float deltaTime);
		bool ShouldRandomize() { return m_bRandomizeTerrain; }
		void ShouldRandomizeFalse() { m_bRandomizeTerrain = false; }

	private:
		static inline glm::vec2 m_DragStart{0,0};
		static inline glm::vec3 m_Rotation{0,0,0};
		static inline glm::vec3 m_MoveDir{0.f,0.f,0.f};
		static inline const float m_MoveSpeed{ 1.f };
		static inline const float m_LookSpeed{ 1.f };
		static inline float m_Yaw{ 0.f };
		static inline float m_Pitch{ 0.f };
		static inline bool m_bRandomizeTerrain{};
	};
}

