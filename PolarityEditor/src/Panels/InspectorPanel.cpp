#include "polpch.h"
#include "InspectorPanel.h"

#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>

namespace Polarity
{
	static bool DrawAxisValue(
		const char* axis,
		float& value,
		float speed,
		float resetValue,
		bool* out_committed = nullptr)
	{
		ImGui::PushID(axis);

		bool changed = false;
		bool committed = false;

		float width = ImGui::CalcItemWidth();
		ImVec2 pos = ImGui::GetCursorScreenPos();

		// Submit the drag FIRST
		changed |= ImGui::DragFloat(
			"##v",
			&value,
			speed,
			0.0f,
			0.0f,
			""
		);

		ImGuiID id = ImGui::GetItemID();
		bool temp_input = ImGui::TempInputIsActive(id);

		// Draw prefix manually when not text-editing
		if (!temp_input)
		{
			char buf[32];
			snprintf(buf, sizeof(buf), "%s%g", axis, value);
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec2 text_pos = pos;
			text_pos.x += style.FramePadding.x;

			ImGui::GetWindowDrawList()->AddText(
				text_pos,
				ImGui::GetColorU32(ImGuiCol_Text),
				buf
			);
		}

		if (ImGui::IsItemDeactivatedAfterEdit())
			committed = true;

		// Right-click reset
		if (ImGui::IsItemHovered() &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			value = resetValue;
			changed = true;
			committed = true;
		}

		if (out_committed)
			*out_committed = committed;

		ImGui::PopID();
		return changed;
	}

	static bool DrawVec3(
		const std::string& label,
		glm::vec3& values,
		float resetValue = 0.0f,
		float labelWidth = 100.0f)
	{
		ImGuiIO& io = ImGui::GetIO();

		float speed = 0.1f;
		if (io.KeyShift) speed = 0.001f;
		if (io.KeyCtrl)  speed = 1.0f;

		glm::vec3 temp = values;
		bool value_changed = false;
		bool value_committed = false;

		ImGui::PushID(label.c_str());

		if (ImGui::BeginTable("##vec3", 2, ImGuiTableFlags_SizingStretchProp))
		{
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, labelWidth);
			ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 0);

			ImGui::TableNextRow();

			// Label
			ImGui::TableNextColumn();
			ImGui::AlignTextToFramePadding();
			ImGui::TextUnformatted(label.c_str());

			// Values
			ImGui::TableNextColumn();

			float fullWidth = ImGui::GetContentRegionAvail().x;
			float spacing = ImGui::GetStyle().ItemSpacing.x;
			float axisWidth = 18.0f;
			float fieldWidth = (fullWidth - spacing * 2) / 3.0f;

			ImGui::PushItemWidth(fieldWidth);

			value_changed |= DrawAxisValue("X:", temp.x, speed, resetValue);
			value_committed |= ImGui::IsItemDeactivatedAfterEdit();
			ImGui::SameLine();

			value_changed |= DrawAxisValue("Y:", temp.y, speed, resetValue);
			value_committed |= ImGui::IsItemDeactivatedAfterEdit();
			ImGui::SameLine();

			value_changed |= DrawAxisValue("Z:", temp.z, speed, resetValue);
			value_committed |= ImGui::IsItemDeactivatedAfterEdit();

			ImGui::PopItemWidth();
			ImGui::EndTable();
		}

		// Commit like ColorEdit4
		if (value_changed)
		{
			values = temp;

			if (value_committed)
				ImGui::MarkItemEdited(ImGui::GetID(label.c_str()));
		}

		ImGui::PopID();
		return value_changed;
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
			ImVec2 avilibleRegion = ImGui::GetContentRegionAvail();
			//ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 5);

			bool open = ImGui::TreeNodeEx((void*)(name.c_str() + entity.GetID()), treeNodeFlags, name.c_str());
			float lineHeight = ImGui::GetTextLineHeightWithSpacing();
			ImVec2 btnSize(lineHeight, lineHeight);
			ImGui::SameLine(avilibleRegion.x - lineHeight * 0.8f);
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

	void InspectorPanel::OnDraw()
	{
		ImGui::Begin(GetImGuiWindowName().c_str(), &m_Open);

		if (!m_Context->SelectedEntity || !m_Context->SelectedEntity.IsAlive())
		{
			ImGui::Text("Select a entity to inspect");
			ImGui::End();
			return;
		}

		Entity entity = m_Context->SelectedEntity;
		ECS::Entity entityID = entity.GetID();


		auto& name = entity.GetComponent<NameComponent>();

		// If selection changed
		if (m_lastSelected != entityID)
		{
			memset(m_NameBuf, 0, sizeof(m_NameBuf));
			strncpy(m_NameBuf, name.Name.c_str(), sizeof(m_NameBuf) - 1);
			m_lastSelected = entityID;
		}

		if (ImGui::InputText("##NameOfEntity", m_NameBuf, IM_ARRAYSIZE(m_NameBuf)))
		{
			name.Name = m_NameBuf;
		}

		float lineHeight = ImGui::GetTextLineHeightWithSpacing();
		ImVec2 btnSize(lineHeight, lineHeight);
		ImGui::SameLine(ImGui::GetWindowWidth() - lineHeight * 4);
		bool isLocked;
		if (ImGui::Checkbox("Lock", &isLocked))
		{
			LOG_DEBUG("Lock inspector");
		}
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
			if (ImGui::MenuItem("Camera"))
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
			DrawVec3("Position", component.Position, 0.0f);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			DrawVec3("Rotation", rotation, 0.0f);
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
			ImGui::Text("Texture");
			ImGui::DragFloat("Scale", &component.Scale, 0.1f);
			ImGui::ColorEdit4("Tint", glm::value_ptr(component.Color));
		});

		DrawComponent<ScriptComponent>("Script", entity, [](auto& component)
		{
			ImGui::Text("Script");
		});

		ImGui::End();
	}
}
