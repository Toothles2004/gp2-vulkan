#pragma once
#include "Mesh.h"

// libraries
#include <glm/gtc/matrix_transform.hpp>

//std includes
#include <memory>

namespace lve
{
	struct TransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};

		glm::mat4 Mat4();
		glm::mat3 NormalMatrix();
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
		TransformComponent transform{};

	private:
		explicit GameObject(IdT objId) : m_Id(objId) {}

		IdT m_Id;
	};
}