#include "polpch.h"
#include "HierarcyPanel.h"
#include "ImGuiEx.h"

#include "imgui/imgui.h"
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

namespace Polarity
{
	void HierarcyPanel::OnDraw()
	{
		auto& plusIcon = UIIcons::Get(UIIcon::Plus);
		uint32_t texID = plusIcon->GetTexture()->GetRendererID();
		const glm::vec2* uvs = plusIcon->GetTexCoords();

		ImGui::Begin(GetImGuiWindowName().c_str(), &m_Open);

		ImGui::TextUnformatted("Name");

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
		float lineHeight = ImGui::GetTextLineHeight();
		ImVec2 btnSize(lineHeight, lineHeight);
		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - btnSize.x);

		if (ImGuiEx::DrawButtonImage(UIIcons::Get(UIIcon::Plus), "iconPlus"))
		{
			ImGui::OpenPopup("AddEntity");
		}
		if (ImGui::BeginPopup("AddEntity"))
		{
			ImGui::TextDisabled("Add Entity");
			ImGui::Separator();

			DrawAddEntityMenu();

			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();

		ImGui::Separator();

		for (auto entity : m_Context->ActiveScene->GetView<NameComponent>())
		{
			DrawEntityNode(entity);
		}

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_Context->SetSelected({});

		if (ImGui::BeginPopupContextWindow("##PopFocus", ImGuiPopupFlags_MouseButtonRight))
		{
			if (m_HoveredEntity)
			{
				if (ImGui::MenuItem("Copy TODO")) {}
				if (ImGui::MenuItem("Paste TODO")) {}
				if (ImGui::MenuItem("Duplicate TODO")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("Delete"))
					m_Context->ActiveScene->DestroyEntity(m_HoveredEntity.GetHandle());
			}

			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupContextWindow("##PopUnfocus", ImGuiPopupFlags_MouseButtonRight |
			ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Paste")) {}
			ImGui::Separator();
			if (ImGui::BeginMenu("Add.."))
			{
				DrawAddEntityMenu();
				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void HierarcyPanel::DrawEntityNode(Entity entity)
	{
		std::string name = entity.GetComponent<NameComponent>().Name;

		ImGuiTreeNodeFlags flags = ((m_Context->GetSelected() == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)entity.GetHandle(), flags, name.c_str());
		if (ImGui::IsItemHovered())
		{
			m_HoveredEntity = entity;
		}
		if (ImGui::IsItemClicked())
		{
			m_Context->SetSelected(entity);
		}
		if (opened)
		{
			ImGui::TreePop();
		}
	}

	void HierarcyPanel::DrawAddEntityMenu()
	{
		if (ImGui::MenuItem("Camera"))
		{
			auto& entity = m_Context->ActiveScene->CreateEntity("Camera");
			entity.AddComponent<CameraComponent>();
		}
		if (ImGui::MenuItem("Sprite"))
		{
			auto& entity = m_Context->ActiveScene->CreateEntity("Sprite");
			entity.AddComponent<SpriteComponent>();
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Empty"))
		{
			m_Context->ActiveScene->CreateEntity("Empty");
		}
	}
}
