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

		std::vector<Scope<EditorPanel>> Panels;
	};

	class EditorPanel
	{
	public:
		virtual ~EditorPanel() = default;

		bool IsOpen = true;
		bool ShowInViewMenu = true;

		virtual const char* GetName() const = 0;
		virtual void OnImGuiRender(EditorContext& ctx) = 0;

		bool Open = true;
	};
}