#include "polpch.h"
#include "HierarcyPanel.h"

#include "imgui/imgui.h"
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace Polarity
{
	void HierarcyPanel::OnImGuiRender(EditorContext& ctx)
	{
		ImGui::Begin("Hierarcy", &m_IsOpen);

		ImGui::Text("Name of Scene TODO");

		ImGui::SameLine();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight, lineHeight };

		float space = ImGui::GetContentRegionAvail().x - (buttonSize.x * 2); // space minus buttons 

		ImGui::InvisibleButton("##DragZone", ImVec2(space, lineHeight));
		ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
		
		ImGui::SameLine();
		if (ImGui::Button("+", buttonSize))
		{
			auto entity = ctx.ActiveScene->Spawn();
			entity.AddComponent<SpriteComponent>();
		}
		ImGui::PopItemWidth();

		ImGui::SameLine();
		if (ImGui::Button("-", buttonSize))
		{
			if(ctx.SelectedEntity.IsAlive())
				ctx.ActiveScene->Kill(ctx.SelectedEntity.GetID());
		}
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::Separator();

		for (auto entity : ctx.ActiveScene->GetView<NameComponent>())
		{
			DrawEntityNode(entity, ctx);
		}

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			ctx.SelectedEntity = {};

		if (ImGui::BeginPopupContextWindow("##PopFocus", ImGuiPopupFlags_MouseButtonRight))
		{
			if (m_HoveredEntity)
			{
			if (ImGui::MenuItem("Copy TODO")){}
			if (ImGui::MenuItem("Paste TODO")){}
			if (ImGui::MenuItem("Duplicate TODO")){}
			ImGui::Separator();
			if (ImGui::MenuItem("Delete"))
				ctx.ActiveScene->Kill(m_HoveredEntity.GetID());
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupContextWindow("##PopUnfocus", ImGuiPopupFlags_MouseButtonRight |
			ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Create empty"))
				ctx.ActiveScene->Spawn("Empty");

			ImGui::EndPopup();
		}

		ImGui::End();
	}
	void HierarcyPanel::DrawEntityNode(Entity entity, EditorContext& ctx)
	{
		std::string name = entity.GetComponent<NameComponent>().Name;

		ImGuiTreeNodeFlags flags = ((ctx.SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
		bool opened = ImGui::TreeNodeEx((void*)entity.GetID(), flags, name.c_str());
		if (ImGui::IsItemHovered())
		{
			m_HoveredEntity = entity;
		}
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
