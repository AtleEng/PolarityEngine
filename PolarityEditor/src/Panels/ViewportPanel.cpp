#include "polpch.h"
#include "ViewportPanel.h"

#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"

namespace Polarity
{
	void ViewportPanel::OnDraw()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });

		ImGui::Begin(GetImGuiWindowName().c_str(), &m_Open, ImGuiWindowFlags_NoCollapse);

		m_ViewportFocused = ImGui::IsWindowFocused();
		m_ViewportHovered = ImGui::IsWindowHovered();

		ImVec2 viewportMin = ImGui::GetCursorScreenPos();
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		glm::vec2 viewportSize = m_Context->ViewportSize;

		glm::vec2 newSize = { viewportPanelSize.x, viewportPanelSize.y };
		if (viewportSize != newSize)
		{
			m_Context->ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		}

		uint32_t textureID = m_Context->ViewportFramebuffer->GetColorAttachmentRendererID();
		ImGui::Image(
			(void*)textureID,
			ImVec2{ viewportSize.x, viewportSize.y },
			{ 0,1 },
			{ 1,0 }
		);

		// Gizmos (orthographic translate arrows is broken, maybe update thirdparty)
		Entity selectedEntity = m_Context->SelectedEntity;
		if (selectedEntity)
		{
			// camera
			auto cameraEntity = m_Context->ActiveScene->GetPrimaryCameraEntity();
			auto& camera = cameraEntity.GetComponent<CameraComponent>().Camera;
			const glm::mat4& cameraProjection = camera.GetProjection();
			glm::mat4 cameraView = glm::inverse(cameraEntity.GetComponent<TransformComponent>().GetTransform());

			ImGuizmo::SetOrthographic(camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic);
			ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());

			ImGuizmo::SetRect(
				viewportMin.x,
				viewportMin.y,
				viewportSize.x,
				viewportSize.y
			);

			// entity
			auto& tc = selectedEntity.GetComponent<TransformComponent>();
			glm::mat4 transform = tc.GetTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(Key::LeftControl);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if ((ImGuizmo::OPERATION)m_GizmoOperation == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			glm::mat4 delta = glm::mat4(1.0f);

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoOperation, (ImGuizmo::MODE)m_GizmoMode, glm::value_ptr(transform),
				glm::value_ptr(delta), snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 position, rotation, scale;
				Math::DecomposeTransform(transform, position, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.Rotation;
				tc.Position = position;
				tc.Rotation += deltaRotation;
				tc.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();
	}
}