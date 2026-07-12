#pragma once

#include "SceneCamera.h"

#include "engine/core/UUID.h"
#include "engine/renderer/Texture.h"
#include "engine/audio/Audio.h"
#include "engine/scripting/ScriptingTypes.h"

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
		AssetHandle Texture = 0;
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

	struct ScriptComponent
	{
		// Script instance created in scene runtime
		Ref<ScriptableEntity> Instance = nullptr;
		// A Script template loaded from DLL
		Ref<ScriptTemplate> Template = nullptr;

		std::string Name;
		std::vector<ScriptFieldInstance> StoredFields;
	};

	template<typename... Component>
	struct ComponentGroup
	{
	};

	using AllComponents =
		ComponentGroup<TransformComponent, SpriteComponent,
		CameraComponent, ScriptComponent, AudioSourceComponent>;
}