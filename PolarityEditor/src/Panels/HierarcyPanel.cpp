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

		if (ImGui::BeginTable("SceneHeader", 2, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Buttons", ImGuiTableColumnFlags_WidthFixed);

			ImGui::TableNextRow();

			// Label
			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted("Name of Scene TODO");

			// Buttons
			ImGui::TableSetColumnIndex(1);

			float lineHeight = ImGui::GetTextLineHeightWithSpacing();
			ImVec2 btnSize(lineHeight, lineHeight);

			if (ImGui::Button("+", btnSize))
			{
				auto entity = ctx.ActiveScene->Spawn();
				entity.AddComponent<SpriteComponent>();
			}

			ImGui::SameLine();

			if (ImGui::Button("-", btnSize))
			{
				if (ctx.SelectedEntity.IsAlive())
					ctx.ActiveScene->Kill(ctx.SelectedEntity.GetID());
			}

			ImGui::EndTable();
		}

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
