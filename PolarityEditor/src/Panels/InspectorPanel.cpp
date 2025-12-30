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
		ImVec2 buttonSize = {lineHeight + 3, lineHeight};

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

		if (ImGui::BeginTable("SceneHeader", 2, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Buttons", ImGuiTableColumnFlags_WidthFixed);

			ImGui::TableNextRow();

			// Label
			ImGui::TableSetColumnIndex(0);
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

			// Buttons
			ImGui::TableSetColumnIndex(1);

			float lineHeight = ImGui::GetTextLineHeightWithSpacing();
			ImVec2 btnSize(lineHeight, lineHeight);

			if (ImGui::Button("+", btnSize))
			{
				auto entity = ctx.ActiveScene->Spawn();
				entity.AddComponent<SpriteComponent>();
			}

			ImGui::EndTable();
		}
		ImGui::Separator();

		if (entity.HasComponent<TransformComponent>())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			if (ImGui::TreeNodeEx((void*)(entity.GetID() + "Transform"), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				DrawVec3("Position", transform.Position);

				glm::vec3 rot = glm::degrees(transform.Rotation);
				DrawVec3("Rotation", rot);
				transform.Rotation = glm::radians(rot);

				DrawVec3("Scale", transform.Scale, 1.0f);
				ImGui::TreePop();
			}
		}
		if (entity.HasComponent<CameraComponent>())
		{
			auto& camera = entity.GetComponent<CameraComponent>();
			if (ImGui::TreeNodeEx((void*)(entity.GetID() + "Camera"), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
			{
				ImGui::Checkbox("Primary", &camera.Primary);
				ImGui::Checkbox("Fixed Aspect Ratio", &camera.FixedAspectRatio);

				const char* projectTypeStrings[] = { "Perspective", "Orthographic" };
				const char* curProjectTypeString = projectTypeStrings[(int)camera.Camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", curProjectTypeString))
				{
					for (int i = 0; i < 2; i++)
					{
						bool isSelected = curProjectTypeString == projectTypeStrings[i];

						if (ImGui::Selectable(projectTypeStrings[i], isSelected))
						{
							curProjectTypeString = projectTypeStrings[i];
							camera.Camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
				if (camera.Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float perspectiveVerticalFov = glm::degrees(camera.Camera.GetPerspectiveVerticalFOV());
					DrawFloat("FOV", perspectiveVerticalFov, 60);
					camera.Camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

					float perspectiveNear = camera.Camera.GetPerspectiveNearClip();
					DrawFloat("Near", perspectiveNear, 0.1f);
					camera.Camera.SetPerspectiveNearClip(perspectiveNear);

					float perspectiveFar = camera.Camera.GetPerspectiveFarClip();
					DrawFloat("Far", perspectiveFar, 100.0f);
					camera.Camera.SetPerspectiveFarClip(perspectiveFar);
				}

				if (camera.Camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.Camera.GetOrthographicSize();
					DrawFloat("Size", orthoSize, 10);
					camera.Camera.SetOrthographicSize(orthoSize);

					float orthoNear = camera.Camera.GetOrthographicNearClip();
					DrawFloat("Near", orthoNear, -1.0f);
					camera.Camera.SetOrthographicNearClip(orthoNear);

					float orthoFar = camera.Camera.GetOrthographicFarClip();
					DrawFloat("Far", orthoFar, 1.0f);
					camera.Camera.SetOrthographicFarClip(orthoFar);
				}

				ImGui::TreePop();
			}
		}
		if (entity.HasComponent<SpriteComponent>())
		{
			auto& sprite = entity.GetComponent<SpriteComponent>();
			if (ImGui::TreeNodeEx((void*)(entity.GetID() + "Sprite"), ImGuiTreeNodeFlags_DefaultOpen, "Sprite"))
			{
				ImGui::InputText("Texture", m_textBuffer, IM_ARRAYSIZE(m_textBuffer));
				static float scale;
				ImGui::DragFloat("Scale", &sprite.Scale, 0.1f);
				static glm::vec4 color;
				ImGui::ColorEdit4("Tint", glm::value_ptr(sprite.Color));

				ImGui::TreePop();
			}
		}
		if (entity.HasComponent<ScriptComponent>())
		{
			auto& script = entity.GetComponent<ScriptComponent>();
			if (ImGui::TreeNodeEx((void*)(entity.GetID() + "Script"), ImGuiTreeNodeFlags_DefaultOpen, "Script"))
			{
				ImGui::Text("Script");

				ImGui::TreePop();
			}
		}

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
