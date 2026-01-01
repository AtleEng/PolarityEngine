#include "polpch.h"
#include "InspectorPanel.h"

#include "imgui/imgui.h"
#include <imgui/imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <glm/gtc/type_ptr.hpp>

namespace Polarity
{

	static void DrawVec3(const std::string& lable, glm::vec3& values, float resetValue = 0.0f, float columWidth = 100.0f)
	{
		ImGui::PushID(lable.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columWidth);
		ImGui::Text(lable.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = {lineHeight, lineHeight};

		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;

		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;

		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;

		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
	}
	
	static void DrawFloat(const std::string& lable, float& value, float resetValue = 0.0f, float columWidth = 100.0f)
	{
		ImGui::PushID(lable.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columWidth);
		ImGui::Text(lable.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(1, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3, lineHeight };

		if (ImGui::Button("*", buttonSize))
			value = resetValue;

		ImGui::SameLine();
		ImGui::DragFloat("##value", &value, 0.1f);
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			//ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 5);
			
			bool open = ImGui::TreeNodeEx((void*)(name.c_str() + entity.GetID()), treeNodeFlags, name.c_str());
			float lineHeight = ImGui::GetTextLineHeightWithSpacing();
			ImVec2 btnSize(lineHeight, lineHeight);
			ImGui::SameLine(ImGui::GetWindowWidth() - lineHeight);
			if (ImGui::Button("*", btnSize))
			{
				ImGui::OpenPopup("ComponentSettings");
			}
			//ImGui::PopStyleVar();
			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Copy TODO")) {}

				if (ImGui::MenuItem("Remove"))
					removeComponent = true;
				ImGui::EndPopup();
			}

			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}
	void InspectorPanel::OnImGuiRender(EditorContext& ctx)
	{
		ImGui::Begin("Inspector", &m_IsOpen);
		if (!ctx.SelectedEntity || !ctx.SelectedEntity.IsAlive())
		{
			ImGui::Text("Select a entity to inspect");
			ImGui::End();
			return;
		}

		Entity entity = ctx.SelectedEntity;
		ECS::Entity entityID = entity.GetID();

		
		auto& name = entity.GetComponent<NameComponent>();

		// If selection changed
		if (m_lastSelected != entityID)
		{
			memset(m_textBuffer, 0, sizeof(m_textBuffer));
			strncpy(m_textBuffer, name.Name.c_str(), sizeof(m_textBuffer) - 1);
			m_lastSelected = entityID;
		}

		if (ImGui::InputText("##NameOfEntity", m_textBuffer, IM_ARRAYSIZE(m_textBuffer)))
		{
			name.Name = m_textBuffer;
		}

			float lineHeight = ImGui::GetTextLineHeightWithSpacing();
			ImVec2 btnSize(lineHeight, lineHeight);
			ImGui::SameLine(ImGui::GetWindowWidth() - lineHeight);
			if (ImGui::Button("+", btnSize))
			{
				ImGui::OpenPopup("AddComponent");
			}
			if (ImGui::BeginPopup("AddComponent"))
			{
				if (ImGui::MenuItem("Transform"))
				{
					entity.AddComponent<TransformComponent>();
					ImGui::CloseCurrentPopup();
				}
				if(ImGui::MenuItem("Camera"))
				{ 
					entity.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Sprite"))
				{
					entity.AddComponent<SpriteComponent>();
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Script"))
				{
					entity.AddComponent<ScriptComponent>();
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{
			DrawVec3("Position", component.Position);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			DrawVec3("Rotation", rotation);
			component.Rotation = glm::radians(rotation);
			DrawVec3("Scale", component.Scale, 1.0f);
		});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
		{
			ImGui::Checkbox("Primary", &component.Primary);
			ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);

			const char* projectTypeStrings[] = { "Perspective", "Orthographic" };
			const char* curProjectTypeString = projectTypeStrings[(int)component.Camera.GetProjectionType()];
			if (ImGui::BeginCombo("Projection", curProjectTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = curProjectTypeString == projectTypeStrings[i];

					if (ImGui::Selectable(projectTypeStrings[i], isSelected))
					{
						curProjectTypeString = projectTypeStrings[i];
						component.Camera.SetProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
			if (component.Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float perspectiveVerticalFov = glm::degrees(component.Camera.GetPerspectiveVerticalFOV());
				DrawFloat("FOV", perspectiveVerticalFov, 60);
				component.Camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

				float perspectiveNear = component.Camera.GetPerspectiveNearClip();
				DrawFloat("Near", perspectiveNear, 0.1f);
				component.Camera.SetPerspectiveNearClip(perspectiveNear);

				float perspectiveFar = component.Camera.GetPerspectiveFarClip();
				DrawFloat("Far", perspectiveFar, 100.0f);
				component.Camera.SetPerspectiveFarClip(perspectiveFar);
			}

			if (component.Camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
			{
				float orthoSize = component.Camera.GetOrthographicSize();
				DrawFloat("Size", orthoSize, 10);
				component.Camera.SetOrthographicSize(orthoSize);

				float orthoNear = component.Camera.GetOrthographicNearClip();
				DrawFloat("Near", orthoNear, -1.0f);
				component.Camera.SetOrthographicNearClip(orthoNear);

				float orthoFar = component.Camera.GetOrthographicFarClip();
				DrawFloat("Far", orthoFar, 1.0f);
				component.Camera.SetOrthographicFarClip(orthoFar);
			}
		});
		
		DrawComponent<SpriteComponent>("Sprite", entity, [](auto& component)
		{
			//ImGui::InputText("Texture", m_textBuffer, IM_ARRAYSIZE(m_textBuffer));
			ImGui::DragFloat("Scale", &component.Scale, 0.1f);
			ImGui::ColorEdit4("Tint", glm::value_ptr(component.Color));
		});
		
		DrawComponent<ScriptComponent>("Script", entity, [](auto& component)
		{
			ImGui::Text("Script");
		});

		if (ImGui::BeginPopupContextWindow("##PopFocus", ImGuiPopupFlags_MouseButtonRight))
		{
				if (ImGui::MenuItem("Copy TODO")) {}
				if (ImGui::MenuItem("Paste TODO")) {}
				if (ImGui::MenuItem("Duplicate TODO")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("Delete")){}

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
}
