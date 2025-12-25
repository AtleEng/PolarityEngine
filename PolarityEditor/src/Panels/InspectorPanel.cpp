#include "polpch.h"
#include "InspectorPanel.h"

#include "imgui/imgui.h"
#include <glm/gtc/type_ptr.hpp>

namespace Polarity
{
	void InspectorPanel::OnImGuiRender(EditorContext& ctx)
	{
		ImGui::Begin("Inspector", nullptr);
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
		ImGui::Separator();

		if (entity.HasComponent<TransformComponent>())
		{
			auto& transform = entity.GetComponent<TransformComponent>().Transform;
			if (ImGui::TreeNodeEx((void*)(entity.GetID() + "Transform"), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
			{
				ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), 0.1f);

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
					if (ImGui::DragFloat("FOV", &perspectiveVerticalFov))
						camera.Camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

					float perspectiveNear = camera.Camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &perspectiveNear))
						camera.Camera.SetPerspectiveNearClip(perspectiveNear);

					float perspectiveFar = camera.Camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &perspectiveFar))
						camera.Camera.SetPerspectiveFarClip(perspectiveFar);
				}

				if (camera.Camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.Camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						camera.Camera.SetOrthographicSize(orthoSize);

					float orthoNear = camera.Camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.Camera.SetOrthographicNearClip(orthoNear);

					float orthoFar = camera.Camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
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
			auto& camera = entity.GetComponent<ScriptComponent>();
			if (ImGui::TreeNodeEx((void*)(entity.GetID() + "Script"), ImGuiTreeNodeFlags_DefaultOpen, "Script"))
			{
				

				ImGui::TreePop();
			}
		}

		ImGui::End();
	}
}
