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
			NameComponent& name = entity.GetComponent<NameComponent>();
			std::string label = name.Name + "##" + std::to_string(entity.GetID());
			if (ImGui::Button(label.c_str()))
			{
				LOG_INFO("Selected %s", name.Name.c_str());
				ctx.SelectedEntity = entity;
			}
		}

		ImGui::End();
	}
}
