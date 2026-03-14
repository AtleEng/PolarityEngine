#pragma once

#include "SceneCamera.h"

#include "engine/core/UUID.h"
#include "engine/renderer/Texture.h"
#include "engine/audio/Audio.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Polarity
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};
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
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Position)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}
	};
	struct SpriteComponent
	{
		Ref<Texture2D> Texture;
		glm::vec4 Color = glm::vec4(1.0f);
		float Scale = 1;

		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent&) = default;
		SpriteComponent(const glm::vec4& color)
			: Color(color) {}
	};
	struct AudioSourceComponent
	{
		Ref<AudioSource> Audio;
		float Gain = 1.0f;
		float Pitch = 1.0f;
		bool Loop = false;

		AudioSourceComponent() = default;
		AudioSourceComponent(const AudioSourceComponent&) = default;
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	class ScriptableEntity; // forward declaration, see 'ScriptableEntity.h' (It includes Entity which we dont want Components.h to know about)

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

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent, SpriteComponent,
		CameraComponent, ScriptComponent,
		ScriptComponent, AudioSourceComponent>;
}