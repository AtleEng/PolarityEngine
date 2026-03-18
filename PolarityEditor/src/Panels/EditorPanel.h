#pragma once
#include "engine/renderer/Texture.h"
#include "engine/renderer/Framebuffer.h"
#include "engine/scene/Scene.h"
#include "engine/scene/Entity.h"

#include "../UIIcons.h";

namespace Polarity
{
	class EditorPanel;

	struct EditorContext
	{
	public:
		void SetSelected(Entity entity)
		{
			if (entity)
			{
				std::string name = entity.GetComponent<NameComponent>().Name;
				POL_INFO("Selected %s", name.c_str());
			}
			SelectedEntity = entity;
		}
		Entity GetSelected() const { return SelectedEntity; }

	public:
		Ref<Scene> ActiveScene;
		Ref<Scene> EditorScene, RuntimeScene;
		Ref<Framebuffer> ViewportFramebuffer;
		glm::vec2 ViewportSize = { 1280, 720 };

		EditorCamera EditorCamera;
		std::function<void(Event&)> EventCallback;
	private:
		Entity SelectedEntity;

	};

	enum class PanelType : uint8_t
	{
		SceneHierarchy,
		Properties,
		Assets,
		Console,
		Viewport,
		Preview
	};

	class EditorPanel
	{
	public:
		virtual ~EditorPanel() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnDraw() = 0;
		virtual void OnMousePressedEvent(MouseButtonPressedEvent& event) {};

		PanelType GetPanelType() const { return m_PanelType; }
		uint32_t GetInstanceID() const { return m_InstanceID; }

		bool IsOpen() const { return m_Open; }
		void Close()		{ m_Open = false; }

		static constexpr bool AllowMultiple() { return false; }

		void SetContext(EditorContext& ctx) { m_Context = &ctx; }

		std::string GetImGuiWindowName() const
		{
			return m_Title + "##" +
				std::to_string(static_cast<uint32_t>(m_PanelType)) + "_" +
				std::to_string(m_InstanceID);
		}

	protected:
		EditorPanel(PanelType type, uint32_t instanceID, std::string title)
			: m_PanelType(type), m_InstanceID(instanceID), m_Title(title) {}

		bool m_Open = true;
		EditorContext* m_Context = nullptr;

	private:
		std::string m_Title;
		PanelType m_PanelType;
		uint32_t m_InstanceID;
	};
}