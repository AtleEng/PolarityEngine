#pragma once
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
	};

	class EditorPanel
	{
	public:
		virtual ~EditorPanel() = default;

		bool m_IsOpen = true;
		bool m_ShowInViewMenu = true;

		virtual const char* GetName() const = 0;
		virtual void OnStart() = 0;
		virtual void OnDraw() = 0;

		void SetContext(EditorContext& ctx) { m_Context = &ctx; }
	protected:
		EditorContext* m_Context = nullptr;
	};
}