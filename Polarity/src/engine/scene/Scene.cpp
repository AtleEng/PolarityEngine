#include "polpch.h"
#include "Scene.h"

#include "engine/renderer/Renderer2D.h"
#include "engine/utils/Random.h"

#include "Components.h"
#include "Entity.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Polarity
{

	Scene::Scene()
		:m_Registry()
	{
		m_Registry.RegisterComponent<NameComponent>();
		m_Registry.RegisterComponent<TransformComponent>();

		m_Registry.RegisterComponent<SpriteComponent>();
		m_Registry.RegisterComponent<CameraComponent>();

		m_Registry.RegisterComponent<ScriptComponent>();
	}

	Scene::~Scene()
	{

	}

	void Scene::OnUpdate(Timestep tS)
	{
		// ---------------------------------------------------------- Update Scripts --
		{
			for (auto entity : m_Registry.GetView<ScriptComponent>())
			{
				auto& script = m_Registry.GetComponent<ScriptComponent>(entity);
				if (!script.Instance)
				{
					script.Instance = script.InstantiateScript();
					script.Instance->m_Scene = this;
					script.Instance->m_EntityHandle = entity;
					script.Instance->OnCreate();
				}
				script.Instance->OnUpdate(tS);
			}
		}
		// ----------------------------------------------- Find Main Camera in Scene --
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			for (auto entity : m_Registry.GetView<CameraComponent, TransformComponent>())
			{
				auto& camera = m_Registry.GetComponent<CameraComponent>(entity);
				auto& transform = m_Registry.GetComponent<TransformComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}
		// ---------------------------------------------------------- Render Sprites --
		if (mainCamera)
		{
			Renderer2D::BeginScene(mainCamera->GetProjection(), cameraTransform);

			for (auto entity : m_Registry.GetView<TransformComponent, SpriteComponent>())
			{
				auto& transform = m_Registry.GetComponent<TransformComponent>(entity);
				auto& sprite = m_Registry.GetComponent<SpriteComponent>(entity);

				Renderer2D::DrawQuad(transform.GetTransform(), sprite.Color);
			}

			Renderer2D::EndScene();
		}
		else
		{
			LOG_WARN("Scene has no main camera!");
		}
		// ----------------------------------------------------------------------------

		for (auto e : m_DestroyQueue)
		{
			m_DestroyQueue.push_back(e);
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		for (auto e : m_Registry.GetView<CameraComponent>())
		{
			auto& camera = m_Registry.GetComponent<CameraComponent>(e);
			if (!camera.FixedAspectRatio)
			{
				camera.Camera.SetViewPortSize(width, height);
			}
		}
	}

	Entity Scene::CreateEntity(std::string name)
	{
		Entity entity = { m_Registry.CreateEntity(), this };

		auto& nameComp = NameComponent();
		nameComp.Name = name;
		entity.AddComponent<NameComponent>(nameComp);

		auto& transform = TransformComponent();
		entity.AddComponent<TransformComponent>(transform);

		LOG_DEBUG("Created %s", name.c_str());
		return entity;
	}

	void Scene::DestroyEntity(std::string name)
	{
		ECS::Entity target = ECS::INVALID_ENTITY;

		for (auto e : m_Registry.GetView<NameComponent>())
		{
			if (m_Registry.GetComponent<NameComponent>(e).Name == name)
			{
				target = e;
				break;
			}
		}

		if (target != ECS::INVALID_ENTITY)
		{
			LOG_DEBUG("Destroyed %s", name.c_str());
			DestroyEntity(target);
		}
		else
		{
			LOG_DEBUG("Destroyed failed for %s", name.c_str());
		}
	}

	void Scene::DestroyEntity(ECS::Entity handle)
	{
		LOG_DEBUG("Destroyed %d", handle);
		m_Registry.DestroyEntity(handle);
	}

	void Scene::ClearEntities()
	{
		m_Registry.Clear();
	}

	bool Scene::IsAlive(ECS::Entity handle)
	{
		return m_Registry.IsAlive(handle);
	}
}