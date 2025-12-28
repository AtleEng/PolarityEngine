#pragma once

#include "SceneCamera.h"
#include "ScriptableEntity.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Polarity
{
	struct NameComponent
	{
		std::string Name = "EntityName";

		NameComponent() = default;
		NameComponent(const NameComponent&) = default;
		NameComponent(const std::string name)
			: Name(name) {}
	};
	struct TransformComponent
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale =	 { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& position)
			: Position(position) {}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, { 1, 0, 0})
				* glm::rotate(glm::mat4(1.0f), Rotation.y, { 0, 1, 0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.z, { 0, 0, 1 });

			return glm::translate(glm::mat4(1.0f), Position)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};
	struct SpriteComponent
	{
		glm::vec4 Color = glm::vec4(1.0f);
		float Scale = 1;

		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent&) = default;
		SpriteComponent(const glm::vec4& color)
			: Color(color) {}
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct ScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*( *InstantiateScript)();
		void (*DestroyScript)(ScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>( new T()); };
			DestroyScript = [](ScriptComponent* sc) { delete sc->Instance; sc->Instance = nullptr; };
		}
	};
}