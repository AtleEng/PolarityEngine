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
			if (ImGui::CollapsingHeader("Transform"))
			{
				ImGui::DragFloat2("Position", glm::value_ptr(transform[3]), 0.1f);
				ImGui::DragFloat2("Size", glm::value_ptr(transform[2]), 0.1f);
				ImGui::DragFloat("Rotation", glm::value_ptr(transform[1]), 0.1f);
			}
		}
		if (entity.HasComponent<CameraComponent>())
		{
			auto& camera = entity.GetComponent<CameraComponent>();
			if (ImGui::CollapsingHeader("Camera"))
			{
				ImGui::Checkbox("Main Camera", &camera.Primary);
				ImGui::Checkbox("Fixed Aspect Ratio", &camera.FixedAspectRatio);
				ImGui::Text("AspectRatio: %f", camera.Camera.GetAspectRatio());
			}
		}
		if (entity.HasComponent<SpriteComponent>())
		{
			auto& sprite = entity.GetComponent<SpriteComponent>();
			if (ImGui::CollapsingHeader("Sprite"))
			{
				ImGui::InputText("Texture", m_textBuffer, IM_ARRAYSIZE(m_textBuffer));
				static float scale;
				ImGui::DragFloat("Scale", &sprite.Scale, 0.1f);
				static glm::vec4 color;
				ImGui::ColorEdit4("Tint", glm::value_ptr(sprite.Color));
			}
		}

		ImGui::End();
	}
}
