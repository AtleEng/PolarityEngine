#pragma once
#include "engine/renderer/Texture.h"
#include "engine/renderer/Framebuffer.h"
#include "engine/scene/Scene.h"
#include "engine/scene/Entity.h"

namespace Polarity
{
	class EditorPanel;

	struct EditorContext
	{
		Ref<Scene> ActiveScene;
		Entity SelectedEntity;
		Ref<Framebuffer> ViewportFramebuffer;
		glm::vec2 ViewportSize = { 1280, 720 };

		std::vector<Ref<SubTexture2D>> Textures = {};
	};
	enum class PanelID : uint8_t
	{
		SceneHierarchy,
		Properties,
		Assets,
		Console,
		Viewport,
	};
	class EditorPanel
	{
	public:
		virtual ~EditorPanel() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnDraw() = 0;

		PanelID GetPanelID() const { return m_PanelID; }
		uint32_t GetInstanceID() const { return m_InstanceID; }

		bool IsOpen() const { return m_Open; }
		void Close() { m_Open = false; }

		static constexpr bool AllowMultiple() { return false; }

		void SetContext(EditorContext& ctx) { m_Context = &ctx; }

		std::string GetImGuiWindowName() const
		{
			return m_Title + "##" +
				std::to_string(static_cast<uint32_t>(m_PanelID)) + "_" +
				std::to_string(m_InstanceID);
		}

	protected:
		EditorPanel(PanelID id, uint32_t instanceID, std::string title)
			: m_PanelID(id), m_InstanceID(instanceID), m_Title(title) {}

		bool m_Open = true;
		EditorContext* m_Context = nullptr;

	private:
		std::string m_Title;
		PanelID m_PanelID;
		uint32_t m_InstanceID;
	};
}