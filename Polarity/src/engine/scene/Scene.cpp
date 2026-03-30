#include "polpch.h"
#include "Scene.h"

#include "engine/core/Application.h"
#include "engine/renderer/Renderer2D.h"
#include "engine/utils/Random.h"

#include "Components.h"
#include "ScriptableEntity.h"
#include "Entity.h"

#include "engine/utils/PlatformUtils.h"
#include "engine/Project/Project.h"

#include "engine/scripting/ScriptingEngine.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Polarity
{

	Scene::Scene()
		:m_Registry()
	{
		m_Registry.RegisterComponent<IDComponent>();
		m_Registry.RegisterComponent<NameComponent>();
		m_Registry.RegisterComponent<TransformComponent>();

		m_Registry.RegisterComponent<SpriteComponent>();
		m_Registry.RegisterComponent<AudioSourceComponent>();
		m_Registry.RegisterComponent<CameraComponent>();

		m_Registry.RegisterComponent<ScriptComponent>();
	}

	Scene::~Scene()
	{

	}

	template<typename... Component>
	static void CopyComponent(ECS::Registry& dst, ECS::Registry& src, const std::unordered_map<UUID, ECS::Entity>& entityMap)
	{
		([&]() {
			for (auto srcEntity : src.GetView<Component>())
			{
				ECS::Entity dstEntity = entityMap.at(src.GetComponent<IDComponent>(srcEntity).ID);
				auto& srcComponent = src.GetComponent<Component>(srcEntity);
				dst.AddOrReplaceComponent<Component>(dstEntity, srcComponent);
			}
		}(), ...);
	}

	template<typename... Component>
	static void CopyComponent(ComponentGroup<Component...>, ECS::Registry& dst, ECS::Registry& src, const std::unordered_map<UUID, ECS::Entity>& entityMap)
	{
		CopyComponent<Component...>(dst, src, entityMap);
	}

	template<typename... Component>
	static void CopyComponentIfExists(Entity dst, Entity src)
	{
		([&]()
		{
			if (src.HasComponent<Component>())
				dst.AddOrReplaceComponent<Component>(src.GetComponent<Component>());
		}(), ...);
	}

	template<typename... Component>
	static void CopyComponentIfExists(ComponentGroup<Component...>, Entity dst, Entity src)
	{
		CopyComponentIfExists<Component...>(dst, src);
	}


	Ref<Scene> Scene::Copy(Ref<Scene> other)
	{
		POL_PROFILE_FUNCTION();

		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportWidth = other->m_ViewportWidth;
		newScene->m_ViewportHeight = other->m_ViewportHeight;

		auto& srcSceneRegistry = other->m_Registry;
		auto& dstSceneRegistry = newScene->m_Registry;
		std::unordered_map<UUID, ECS::Entity> entityMap;

		// Create entities in new scene
		auto idView = srcSceneRegistry.GetView<IDComponent, NameComponent>();
		for (auto e : idView)
		{
			UUID uuid = srcSceneRegistry.GetComponent<IDComponent>(e).ID;
			const auto& name = srcSceneRegistry.GetComponent<NameComponent>(e).Name;
			Entity newEntity = newScene->CreateEntityWithUUID(uuid, name);
			entityMap[uuid] = (ECS::Entity)newEntity;
		}

		// Copy components (except IDComponent and NameComponent)
		CopyComponent(AllComponents{}, dstSceneRegistry, srcSceneRegistry, entityMap);

		return newScene;
	}

	void Scene::OnRuntimeStart()
	{
		POL_PROFILE_FUNCTION();
	}

	void Scene::OnRuntimeStop()
	{
		POL_PROFILE_FUNCTION();
	}

	void Scene::OnUpdateRuntime(Timestep tS)
	{
		POL_PROFILE_FUNCTION();
		// ---------------------------------------------------------- Update Scripts --
		{
			for (auto entity : m_Registry.GetView<ScriptComponent>())
			{
				auto& script = m_Registry.GetComponent<ScriptComponent>(entity);
				if (!script.Instance)
				{
					script.Instance = ScriptEngine::Create(script.ScriptName);
					if (script.Instance)
					{
						script.Instance->m_Entity = Entity{ entity, this };
						script.Instance->m_Input = &Application::Get().GetInput();

						script.Instance->OnCreate();
					}
				}
				if (script.Instance)
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

				Renderer2D::DrawSprite(transform.GetTransform(), sprite, entity);
			}

			Renderer2D::EndScene();
		}
		else
		{
			POL_CORE_ERROR("No camera in scene!")
		}
		// ----------------------------------------------------------------------------

		for (auto e : m_DestroyQueue)
		{
			m_DestroyQueue.push_back(e);
		}
	}

	void Scene::OnUpdateEditor(Timestep ts, EditorCamera& camera)
	{
		POL_PROFILE_FUNCTION();
		// ---------------------------------------------------------- Render Sprites --
		Renderer2D::BeginScene(camera);

		for (auto entity : m_Registry.GetView<TransformComponent, SpriteComponent>())
		{
			auto& transform = m_Registry.GetComponent<TransformComponent>(entity);
			auto& sprite = m_Registry.GetComponent<SpriteComponent>(entity);

			Renderer2D::DrawSprite(transform.GetTransform(), sprite, entity);
		}

		Renderer2D::EndScene();
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
		return CreateEntityWithUUID(UUID(), name);
	}

	Entity Scene::CreateEntityWithUUID(UUID uuid, const std::string name)
	{
		POL_PROFILE_FUNCTION();
		Entity entity = { m_Registry.CreateEntity(), this };
		if (entity.GetHandle() == ECS::INVALID_ENTITY)
		{
			return entity;
		}

		auto& idComp = entity.AddComponent<IDComponent>();
		idComp.ID = uuid;

		auto& nameComp = NameComponent();
		nameComp.Name = name;
		entity.AddComponent<NameComponent>(nameComp);

		auto& transform = TransformComponent();
		entity.AddComponent<TransformComponent>(transform);

		POL_CORE_INFO("Scene: Created %s [%i]", name.c_str(), idComp.ID);
		return entity;
	}

	void Scene::DestroyEntity(std::string name)
	{
		ECS::Entity target = ECS::INVALID_ENTITY;

		for (auto entity : m_Registry.GetView<NameComponent>())
		{
			if (m_Registry.GetComponent<NameComponent>(entity).Name == name)
			{
				target = entity;
				break;
			}
		}

		if (target != ECS::INVALID_ENTITY)
		{
			DestroyEntity(target);
			POL_CORE_INFO("Scene: Destroyed entity: %s", name.c_str());
		}
		else
		{
			POL_CORE_INFO("Destroy failed for entity: %s", name.c_str());
		}
	}

	void Scene::DestroyEntity(ECS::Entity handle)
	{
		POL_CORE_INFO("Scene: Destroyed entity id: %d", handle);
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

	Entity Scene::GetPrimaryCameraEntity()
	{
		POL_PROFILE_FUNCTION();
		auto view = m_Registry.GetView<CameraComponent>();
		for (auto entity : view)
		{
			const auto& camera = m_Registry.GetComponent<CameraComponent>(entity);
			if (camera.Primary)
			{
				return Entity(entity, this);
			}
		}
		return {};
	}
}