#include "polpch.h"
#include "HierarcyPanel.h"

#include "imgui/imgui.h"
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace Polarity
{
	void HierarcyPanel::OnDraw()
	{
		ImGui::Begin(GetImGuiWindowName().c_str(), &m_Open);

		ImGui::TextUnformatted("Name of Scene TODO");

		float lineHeight = ImGui::GetTextLineHeightWithSpacing();
		ImVec2 btnSize(lineHeight, lineHeight);
		ImGui::SameLine(ImGui::GetWindowWidth() - lineHeight);

		if (ImGui::Button("+", btnSize))
		{
			auto entity = m_Context->ActiveScene->CreateEntity();
			entity.AddComponent<SpriteComponent>();
		}

		ImGui::Separator();

		for (auto entity : m_Context->ActiveScene->GetView<NameComponent>())
		{
			DrawEntityNode(entity, m_Context);
		}

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_Context->SelectedEntity = {};

		if (ImGui::BeginPopupContextWindow("##PopFocus", ImGuiPopupFlags_MouseButtonRight))
		{
			if (m_HoveredEntity)
			{
				if (ImGui::MenuItem("Copy TODO")) {}
				if (ImGui::MenuItem("Paste TODO")) {}
				if (ImGui::MenuItem("Duplicate TODO")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("Delete"))
					m_Context->ActiveScene->DestroyEntity(m_HoveredEntity.GetID());
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupContextWindow("##PopUnfocus", ImGuiPopupFlags_MouseButtonRight |
			ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Create empty"))
				m_Context->ActiveScene->CreateEntity("Empty");

			ImGui::EndPopup();
		}

		ImGui::End();
	}
	void HierarcyPanel::DrawEntityNode(Entity entity, EditorContext* ctx)
	{
		std::string name = entity.GetComponent<NameComponent>().Name;

		ImGuiTreeNodeFlags flags = ((ctx->SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)entity.GetID(), flags, name.c_str());
		if (ImGui::IsItemHovered())
		{
			m_HoveredEntity = entity;
		}
		if (ImGui::IsItemClicked())
		{
			LOG_INFO("Selected %s", name.c_str());
			ctx->SelectedEntity = entity;
		}
		if (opened)
		{
			ImGui::TreePop();
		}
	}
}
