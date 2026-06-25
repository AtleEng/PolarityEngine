#include "polpch.h"
#include "PropertitiesPanel.h"
#include "ImGuiEx.h"

#include "engine/scripting/ScriptingEngine.h"

#include <imgui/imgui_internal.h>

#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

namespace Polarity
{
	static bool BeginPropertyGrid()
	{
		if (ImGui::BeginTable(
			"##PropertyGrid",
			2,
			ImGuiTableFlags_SizingStretchProp |
			ImGuiTableFlags_BordersInnerV))
		{
			ImGui::TableSetupColumn(
				"Label",
				ImGuiTableColumnFlags_WidthFixed,
				120.0f);

			ImGui::TableSetupColumn(
				"Value",
				ImGuiTableColumnFlags_WidthStretch);

			return true;
		}

		return false;
	}

	static void EndPropertyGrid()
	{
		ImGui::EndTable();
	}

	static void PropertyRow(const char* label)
	{
		ImGui::TableNextRow();

		ImGui::TableSetColumnIndex(0);
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted(label);

		ImGui::TableSetColumnIndex(1);
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags =
			ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_Framed |
			ImGuiTreeNodeFlags_SpanAvailWidth |
			ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_FramePadding;

		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();
			ImVec2 avilibleRegion = ImGui::GetContentRegionAvail();
			//ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 5);

			bool open = ImGui::TreeNodeEx((void*)(name.c_str() + entity.GetHandle()), treeNodeFlags, name.c_str());
			float lineHeight = ImGui::GetTextLineHeightWithSpacing();

			ImGui::SameLine(avilibleRegion.x - lineHeight * 0.5f);
			if (ImGuiEx::DrawButtonImage(UIIcons::Get(UIIcon::Settings), "##settings"))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{

				ImGui::TextDisabled(name.c_str());
				if (ImGui::MenuItem("Reset TODO")) {}
				ImGui::Separator();

				if (ImGui::MenuItem("Copy TODO")) {}

				if (ImGui::MenuItem("Remove"))
					removeComponent = true;
				ImGui::EndPopup();
			}

			if (open)
			{
				if (BeginPropertyGrid())
				{
					uiFunction(component);
					EndPropertyGrid();
				}
				ImGui::TreePop();
			}

			if (removeComponent)
				entity.RemoveComponent<T>();
		}
	}

	void PropertitiesPanel::DrawTopbar(Entity entity)
	{
		ECS::EntityMeta& metadata = entity.GetMeta();
		auto& name = entity.GetComponent<NameComponent>();

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));

		const float buttonSize = ImGui::GetFrameHeight();
		const float spacing = ImGui::GetStyle().ItemSpacing.x;

		// Enable checkbox
		ImGui::Checkbox("##Enabled", &metadata.enabled);
		ImGui::SameLine();

		// Calculate width reserved on the right
		const float rightControlsWidth = buttonSize * 2.0f + spacing;

		float nameWidth =
			ImGui::GetContentRegionAvail().x - rightControlsWidth;

		if (nameWidth < 50.0f)
			nameWidth = 50.0f;

		ImGui::SetNextItemWidth(nameWidth);

		char buffer[256] = {};
		strncpy(buffer, name.Name.c_str(), sizeof(buffer) - 1);

		if (ImGui::InputText("##EntityName", buffer, sizeof(buffer)))
			name.Name = buffer;

		ImGui::SameLine();

		auto lockIcon = m_Locked
			? UIIcons::Get(UIIcon::Lock)
			: UIIcons::Get(UIIcon::Unlock);

		if (ImGuiEx::DrawButtonImage(lockIcon, "##Lock")) 
		{
			m_Locked = !m_Locked;
		}

		ImGui::SameLine();

		if (ImGuiEx::DrawButtonImage(
			UIIcons::Get(UIIcon::Settings),
			"##HeaderMenu"))
		{
			ImGui::OpenPopup("HeaderMenu");
		}
		if (ImGui::BeginPopup("HeaderMenu"))
		{
			ImGui::TextUnformatted("TODO");
			ImGui::EndPopup();
		}

		if (ImGui::Button("Add Component", ImVec2(-FLT_MIN, 0.0f)))
		{
			ImGui::OpenPopup("AddComponent");
		}
		if (ImGui::BeginPopup("AddComponent"))
		{
			ImGui::TextDisabled("Add Component");
			ImGui::Separator();

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

			if (ImGui::MenuItem("Sprite Renderer"))
			{
				entity.AddComponent<SpriteComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Audio Source"))
			{
				entity.AddComponent<AudioSourceComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Script"))
			{
				entity.AddComponent<ScriptComponent>();
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::PopStyleVar();
	}
	void PropertitiesPanel::DrawProperites(Entity entity)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				PropertitiesPanel::Property("Position", component.Position, { 0.01f, false, false, 0.0f, 0.0f, true, 0.0f, "Position" });

				glm::vec3 rotation = glm::degrees(component.Rotation);
				PropertitiesPanel::Property("Rotation", rotation, { 0.01f, false, false, 0.0f, 0.0f, true, 0.0f, "Rotation" });
				component.Rotation = glm::radians(rotation);

				PropertitiesPanel::Property("Scale", component.Scale, { 0.01f, false, false, 0.0f, 0.0f, true, 1.0f, "Scale" });
			});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
			{
				PropertitiesPanel::Property("Primary", component.Primary);
				PropertitiesPanel::Property("Fixed Aspect", component.FixedAspectRatio);

				ImGui::Separator();

				const char* projectTypeStrings[] = { "Perspective", "Orthographic" };
				int projectionType = (int)component.Camera.GetProjectionType();

				if (PropertyCombo(
					"Projection",
					projectionType,
					projectTypeStrings,
					IM_ARRAYSIZE(projectTypeStrings)))
				{
					component.Camera.SetProjectionType((SceneCamera::ProjectionType)projectionType);
				}
				if (component.Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float perspectiveVerticalFov = glm::degrees(component.Camera.GetPerspectiveVerticalFOV());
					PropertitiesPanel::Property("FOV", perspectiveVerticalFov, { 1.0f, true, true, 0.0f, 360.0f, true, 60.0f, "Field of view for the camera" });
					component.Camera.SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

					float perspectiveNear = component.Camera.GetPerspectiveNearClip();
					PropertitiesPanel::Property("Near", perspectiveNear, { 1.0f, true, false, 0.0f, 0.0f, true, 0.1f, "How near the camera render things" });
					component.Camera.SetPerspectiveNearClip(perspectiveNear);

					float perspectiveFar = component.Camera.GetPerspectiveFarClip();
					PropertitiesPanel::Property("Far", perspectiveFar, { 1.0f, true, false, 0.0f, 0.0f, true, 100.0f, "How far the camera render things" });
					component.Camera.SetPerspectiveFarClip(perspectiveFar);
				}

				if (component.Camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = component.Camera.GetOrthographicSize();
					PropertitiesPanel::Property("Size", orthoSize, { 0.1f, false, false, 0.0f, 0.0f, true, 10.0f, "Size of cameras view" });
					component.Camera.SetOrthographicSize(orthoSize);

					float orthoNear = component.Camera.GetOrthographicNearClip();
					PropertitiesPanel::Property("Near", orthoNear, { 0.1f, false, false, 0.0f, 0.0f, true, -1.0f, "How near the camera render things" });
					component.Camera.SetOrthographicNearClip(orthoNear);

					float orthoFar = component.Camera.GetOrthographicFarClip();
					PropertitiesPanel::Property("Far", orthoFar, { 0.1f, false, false, 0.0f, 0.0f, true, 1.0f, "How far the camera render things" });
					component.Camera.SetOrthographicFarClip(orthoFar);
				}
			});

		DrawComponent<SpriteComponent>("Sprite", entity, [](auto& component)
			{
				ImGui::Button("Texture", ImVec2(100.0f, 0.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texPath = std::filesystem::path(path);
						Ref<Texture2D> tex = Texture2D::Create(texPath.string());
						if (tex->IsLoaded())
							component.Texture = tex;
						else
							POL_WARN("Could not load texture '%s'", texPath.filename().string().c_str());
					}
					ImGui::EndDragDropTarget();
				}
				PropertitiesPanel::Property("Scale", component.Scale, { 0.1f, false, false, 0.0f, 0.0f, true, 1.0f, "The scale of the applied texture" });
				ImGui::ColorEdit4("Tint", glm::value_ptr(component.Color));
			});

		DrawComponent<AudioSourceComponent>("Audio Source", entity, [](auto& component)
			{
				ImGui::Button("Audio", ImVec2(100.0f, 0.0f));
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texPath = std::filesystem::path(path);
						Ref<AudioSource> audio = Audio::Create(texPath.string());
						if (audio->IsLoaded())
							component.Audio = audio;
						else
							POL_WARN("Could not load audio '%s'", texPath.filename().string().c_str());
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::SameLine();
				if (ImGui::Button("Play", ImVec2(100.0f, 0.0f)))
				{
					Audio::Play(component.Audio, component.Gain, component.Pitch, component.Loop);
				}

				PropertitiesPanel::Property("Gain", component.Gain, { 0.01f, true, false, 0.0f, 0.0f, true, 1.0f, "The volume of the source" });
				PropertitiesPanel::Property("Pitch", component.Pitch, { 0.01f, true, true, 0.0f, 10.0f, true, 1.0f, "The pitch of the sound" });
				PropertitiesPanel::Property("Loop", component.Loop, { true, false, "If applied the sound loops until stopped" });
			});

		DrawComponent<ScriptComponent>("Script", entity, [this](auto& component)
			{
				auto& scriptTemplates = ScriptEngine::GetScripts();

				PropertyRow("Script");
				ImGui::SetNextItemWidth(-FLT_MIN);
				const char* current = component.Name.empty()
					? "None"
					: component.Name.c_str();

				if (ImGui::BeginCombo("##Script", current))
				{
					{
						bool isSelected = (component.Name == "");

						if (ImGui::Selectable("None", isSelected))
						{
							component.Name = "";
							component.Template = nullptr;
							component.Instance = nullptr;
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					for (auto& scriptTemplate : scriptTemplates)
					{
						bool isSelected = (component.Name == scriptTemplate.Name);

						if (ImGui::Selectable(scriptTemplate.Name.c_str(), isSelected))
						{
							component.Name = scriptTemplate.Name;
							component.Template = CreateRef<ScriptTemplate>(scriptTemplate);
							component.Instance = nullptr;

							component.StoredFields.clear();
							component.StoredFields.reserve(component.Template->Fields.size());
							for (const auto& field : component.Template->Fields)
							{
								ScriptFieldInstance instance;
								instance.Field = field;
								component.StoredFields.emplace_back(std::move(instance));
							}
						}

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
				ImGui::Separator();
				
				if (component.Template)
				{
					for (auto& storedField : component.StoredFields)
					{
						const char* name = storedField.Field.Name.c_str();

						void* data = storedField.GetData();

						switch (storedField.Field.Type)
						{
						case FieldType::Float:
						{
							float& value = *static_cast<float*>(data);
							PropertitiesPanel::Property(name, value, { 0.1f, false, false, 0.0f, 0.0f, true, 0.0f, name });
							break;
						}
						case FieldType::Int:
							ImGui::DragInt(name, (int*)data); break;
						case FieldType::Bool:
							ImGui::Checkbox(name, (bool*)data); break;
						default:
							ImGui::Text("Unknown field type: %s", name);
							break;
						}
					}
					if (component.Instance)
					{
						ScriptEngine::ApplyFieldsToInstance(component);
					}
				}
			});

		ImGui::PopStyleVar();
	}

	void PropertitiesPanel::OnDraw()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
		ImGui::Begin(GetImGuiWindowName().c_str(), &m_Open);

		Entity entity;
		if (m_Locked)
		{
			entity = m_lastSelected;
		}
		else
		{
			entity = m_Context->GetSelected();
			m_lastSelected = entity;
		}

		if (!entity || !entity.IsAlive())
		{
			m_Locked = false;
			ImGui::PopStyleVar();
			ImGui::End();
			return;
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
		DrawTopbar(entity);
		DrawProperites(entity);
		ImGui::PopStyleVar();

		ImGui::PopStyleVar();
		ImGui::End();
	}

	bool PropertitiesPanel::Property(const char* label, float& value, const FloatPropertySettings& settings)
	{
		ImGui::PushID(label);
		PropertyRow(label);

		if (settings.Tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			ImGui::SetTooltip("%s", settings.Tooltip);

		ImGui::SetNextItemWidth(-FLT_MIN);

		bool changed = false;

		if (settings.HasMin || settings.HasMax)
		{
			changed = ImGui::DragFloat(
				"##Value",
				&value,
				settings.Speed,
				settings.HasMin ? settings.Min : -FLT_MAX,
				settings.HasMax ? settings.Max : FLT_MAX);
		}
		else
		{
			changed = ImGui::DragFloat(
				"##Value",
				&value,
				settings.Speed);
		}

		// Right click reset
		if (settings.HasReset &&
			ImGui::IsItemHovered() &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			value = settings.ResetValue;
			changed = true;
		}

		// TODO commands
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			// CommandStack::Push(...)
		}

		ImGui::PopID();

		return changed;
	}

	bool PropertitiesPanel::Property(const char* label, int& value, const IntPropertySettings& settings)
	{
		ImGui::PushID(label);
		PropertyRow(label);

		if (settings.Tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			ImGui::SetTooltip("%s", settings.Tooltip);

		ImGui::SetNextItemWidth(-FLT_MIN);

		bool changed = false;

		if (settings.HasMin || settings.HasMax)
		{
			changed = ImGui::DragInt(
				"##Value",
				&value,
				settings.Speed,
				settings.HasMin ? settings.Min : -FLT_MAX,
				settings.HasMax ? settings.Max : FLT_MAX);
		}
		else
		{
			changed = ImGui::DragInt(
				"##Value",
				&value,
				settings.Speed);
		}

		// Right click reset
		if (settings.HasReset &&
			ImGui::IsItemHovered() &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			value = settings.ResetValue;
			changed = true;
		}

		// TODO commands
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			// CommandStack::Push(...)
		}

		ImGui::PopID();

		return changed;
	}

	bool PropertitiesPanel::Property(const char* label, bool& value, const BoolPropertySettings& settings)
	{
		ImGui::PushID(label);
		PropertyRow(label);

		if (settings.Tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
			ImGui::SetTooltip("%s", settings.Tooltip);

		ImGui::SetNextItemWidth(-FLT_MIN);

		bool changed = false;

		changed = ImGui::Checkbox("##Value", &value);

		// Right click reset
		if (settings.HasReset &&
			ImGui::IsItemHovered() &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			value = settings.ResetValue;
			changed = true;
		}

		// TODO commands
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			// CommandStack::Push(...)
		}

		ImGui::PopID();

		return changed;
	}

	bool PropertitiesPanel::Property(const char* label, std::string& value, const StringPropertySettings& settings)
	{
		ImGui::PushID(label);
		PropertyRow(label);

		if (settings.Tooltip &&
			ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		{
			ImGui::SetTooltip("%s", settings.Tooltip);
		}

		ImGui::SetNextItemWidth(-FLT_MIN);

		bool changed = false;
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));

		strncpy(buffer,
			value.c_str(),
			sizeof(buffer) - 1);

		if (ImGui::InputText("##Value",
			buffer,
			sizeof(buffer)))
		{
			value = buffer;
			changed = true;
		}

		ImGui::PopID();
		return changed;
	}

	bool PropertitiesPanel::Property(const char* label, glm::vec2& value, const FloatPropertySettings& settings)
	{
		ImGui::PushID(label);
		PropertyRow(label);

		if (settings.Tooltip &&
			ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		{
			ImGui::SetTooltip("%s", settings.Tooltip);
		}

		bool changed = false;

		float spacing = ImGui::GetStyle().ItemSpacing.x;
		float width = (ImGui::GetContentRegionAvail().x - spacing) * 0.5f;

		ImGui::PushItemWidth(width);

		changed |= ImGui::DragFloat(
			"##X",
			&value.x,
			settings.Speed,
			settings.HasMin ? settings.Min : -FLT_MAX,
			settings.HasMax ? settings.Max : FLT_MAX);

		ImGui::SameLine();

		changed |= ImGui::DragFloat(
			"##Y",
			&value.y,
			settings.Speed,
			settings.HasMin ? settings.Min : -FLT_MAX,
			settings.HasMax ? settings.Max : FLT_MAX);

		ImGui::PopItemWidth();

		if (settings.HasReset &&
			ImGui::IsItemHovered() &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			value = glm::vec2(settings.ResetValue);
			changed = true;
		}

		ImGui::PopID();
		return changed;
	}

	bool PropertitiesPanel::Property(const char* label, glm::vec3& value, const FloatPropertySettings& settings)
	{
		ImGui::PushID(label);
		PropertyRow(label);

		if (settings.Tooltip &&
			ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		{
			ImGui::SetTooltip("%s", settings.Tooltip);
		}

		bool changed = false;

		float spacing = ImGui::GetStyle().ItemSpacing.x;
		float width =
			(ImGui::GetContentRegionAvail().x - spacing * 2.0f) / 3.0f;

		ImGui::PushItemWidth(width);

		changed |= ImGui::DragFloat(
			"##X",
			&value.x,
			settings.Speed,
			settings.HasMin ? settings.Min : -FLT_MAX,
			settings.HasMax ? settings.Max : FLT_MAX);

		ImGui::SameLine();

		changed |= ImGui::DragFloat(
			"##Y",
			&value.y,
			settings.Speed,
			settings.HasMin ? settings.Min : -FLT_MAX,
			settings.HasMax ? settings.Max : FLT_MAX);

		ImGui::SameLine();

		changed |= ImGui::DragFloat(
			"##Z",
			&value.z,
			settings.Speed,
			settings.HasMin ? settings.Min : -FLT_MAX,
			settings.HasMax ? settings.Max : FLT_MAX);

		ImGui::PopItemWidth();

		if (settings.HasReset &&
			ImGui::IsItemHovered() &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			value = glm::vec3(settings.ResetValue);
			changed = true;
		}

		ImGui::PopID();
		return changed;
	}

	bool PropertitiesPanel::PropertyColor(const char* label, glm::vec4& value)
	{
		return false;
	}
	bool PropertitiesPanel::PropertyCombo(const char* label, int& currentIndex, const char* const* items, int itemCount, const char* tooltip)
	{
		ImGui::PushID(label);
		PropertyRow(label);

		if (tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
		{
			ImGui::SetTooltip("%s", tooltip);
		}

		ImGui::SetNextItemWidth(-FLT_MIN);

		bool changed = false;

		const char* preview =
			(currentIndex >= 0 && currentIndex < itemCount)
			? items[currentIndex]
			: "None";

		if (ImGui::BeginCombo("##Value", preview))
		{
			for (int i = 0; i < itemCount; i++)
			{
				bool selected = (currentIndex == i);

				if (ImGui::Selectable(items[i], selected))
				{
					currentIndex = i;
					changed = true;
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::PopID();
		return changed;
	}
	bool PropertitiesPanel::PropertyAsset(const char* label, Ref<Texture2D>& value)
	{
		return false;
	}
}
