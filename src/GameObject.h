#pragma once
#include "Mesh.h"

//std includes
#include <memory>

namespace lve
{
	struct Transform2DComponent
	{
		glm::vec2 translation{};
		glm::vec2 scale{ 1.f, 1.f };
		float rotation{};

		glm::mat2 Mat2()
		{
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMatrix{ {c,s}, {-s, c} };

			glm::mat2 scaleMat{ {scale.x, 0.f}, {0.f, scale.y} };
			return rotMatrix * scaleMat;
		}
	};

	class GameObject
	{
	public:
		using IdT = unsigned int;

		static GameObject CreateGameObject()
		{
			static IdT currentId = 0;
			return GameObject(currentId++);
		}

		IdT GetId() const { return m_Id; }

		GameObject(const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator=(const GameObject&) = delete;
		GameObject& operator=(GameObject&&) = default;

		std::shared_ptr<Mesh> mesh;
		glm::vec3 color{};
		Transform2DComponent transform2D{};

	private:
		explicit GameObject(IdT objId) : m_Id(objId) {}

		IdT m_Id;
	};
}