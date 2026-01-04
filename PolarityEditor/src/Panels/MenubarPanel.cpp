#include "polpch.h"
#include "MenubarPanel.h"

#include "imgui/imgui.h"
#include "engine/scene/SceneSerializer.h"

namespace Polarity
{
	void MenubarPanel::OnDraw()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 4.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 8.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 2.0f));
		if (ImGui::BeginMenuBar())
		{
			float titleHeight = 24.0f; // TODO make a bigger title
			/*
			uint32_t texID = m_logoTex->GetRendererID();
			ImGui::Image((void*)texID, ImVec2{ titleHeight, titleHeight },
				ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			*/

			if (ImGui::BeginMenu("File")) // all commands to manage files
			{
				if (ImGui::MenuItem("New TODO        Ctrl+N"));
				if (ImGui::MenuItem("Open TODO       Ctrl+O"))
				{
					SceneSerializer serializer(m_Context->ActiveScene);
					serializer.DeSerialize("assets/scenes/Example.pol");
				}
				if (ImGui::MenuItem("Save TODO       Ctrl+S"))
				{
					SceneSerializer serializer(m_Context->ActiveScene);
					serializer.Serialize("assets/scenes/Example.pol");
				}
				if (ImGui::MenuItem("Build TODO      Ctrl+B"));
				if (ImGui::MenuItem("Play TODO       F5"));
				ImGui::Separator();
				if (ImGui::MenuItem("Quit")) Application::Get().Shutdown();

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit")) // all commands to edit project
			{
				if (ImGui::MenuItem("Project Settings TODO"));
				if (ImGui::MenuItem("Editor Settings TODO"));

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Windows"))
			{
				/*
				for (auto& panel : m_Context.Panels)
				{
					if (panel->m_ShowInViewMenu)
					{
					ImGui::MenuItem(
						panel->GetName(),
						nullptr,
						&panel->m_IsOpen
					);
					}
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Reset Layout")) //TODO
				{
					for (auto& panel : m_Context.Panels)
					{
						panel->m_IsOpen = true;
					}
				}
				*/
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				ImGui::MenuItem("General");
				ImGui::EndMenu();
			}
			/*
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0)); // Drag area 
			float space = ImGui::GetContentRegionAvail().x - (32 * 3); // space minus buttons 

			ImGui::InvisibleButton("##DragZone", ImVec2(space, titleHeight));


			static bool dragging = false;
			static ImVec2 dragStartMouse;
			static glm::ivec2 dragStartWindow;

			if (ImGui::IsItemActive())
			{
				if (!dragging)
				{
					dragging = true;
					dragStartMouse = ImGui::GetMousePos();
					dragStartWindow = Application::Get().GetWindow().GetPosition();
				}

				ImVec2 mouseNow = ImGui::GetMousePos();
				ImVec2 delta = { mouseNow.x - dragStartMouse.x + 4, mouseNow.y - dragStartMouse.y + 31 };

				Application::Get().GetWindow().SetPosition({
					dragStartWindow.x + (int)delta.x,
					dragStartWindow.y + (int)delta.y
					});
			}
			else
			{
				dragging = false;
			}
			ImVec4* colors = ImGui::GetStyle().Colors;
			colors[ImGuiCol_Button] = ImVec4(0.11f, 0.13f, 0.13f, 1.00f);
			// --- Window buttons --- 
			if (ImGui::Button("-", { 32, titleHeight }))
				LOG_DEBUG("Minimize!");
			if (ImGui::Button("[]", { 32, titleHeight }))
				LOG_DEBUG("Maximize!");
			if (ImGui::Button("X", { 32, titleHeight }))
				Application::Get().Shutdown();

			colors[ImGuiCol_Button] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
			ImGui::PopStyleVar();

			*/
			ImGui::EndMenuBar();
		}
		ImGui::PopStyleVar(3);
	}
}