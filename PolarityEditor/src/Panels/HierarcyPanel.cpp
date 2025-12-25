#include "polpch.h"
#include "HierarcyPanel.h"

#include "imgui/imgui.h"

namespace Polarity
{
	void HierarcyPanel::OnImGuiRender(EditorContext& ctx)
	{
		ImGui::Begin("Hierarcy", nullptr);

		if (ImGui::Button("Spawn"))
		{
			auto entity = ctx.ActiveScene->Spawn();
			entity.AddComponent<SpriteComponent>();
		}
		ImGui::SameLine();
		if (ImGui::Button("Kill"))
		{
			ctx.ActiveScene->Kill("NewEntity");
		}
		ImGui::SameLine();
		if (ImGui::Button("List"))
		{
			ctx.ActiveScene->List();
		}
		ImGui::Separator();

		for (auto entity : ctx.ActiveScene->GetView<NameComponent>())
		{
			DrawEntityNode(entity, ctx);
		}

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			ctx.SelectedEntity = {};

		ImGui::End();
	}
	void HierarcyPanel::DrawEntityNode(Entity entity, EditorContext& ctx)
	{
		std::string name = entity.GetComponent<NameComponent>().Name;

		ImGuiTreeNodeFlags flags = ((ctx.SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)entity.GetID(), flags, name.c_str());
		if (ImGui::IsItemClicked())
		{
			LOG_INFO("Selected %s", name.c_str());
			ctx.SelectedEntity = entity;
		}
		if (opened)
		{
			ImGui::TreePop();
		}
	}
}
