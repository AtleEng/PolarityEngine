#pragma once
#include "EditorPanel.h"

namespace Polarity {

	class PanelManager
	{
	public:
		void SetContext(EditorContext& ctx);
		void OnDraw();
		void OnMousePressedEvent(MouseButtonPressedEvent& e);
		void Clear();

		template<typename T, typename... Args>
		T& OpenPanel(EditorContext& ctx, Args&&... args)
		{
			static_assert(std::is_base_of_v<EditorPanel, T>);

			constexpr PanelType id = T::StaticPanelID;

			auto& list = m_Panels[id];

			if constexpr (T::AllowMultiple())
			{
				uint32_t instanceID = m_NextInstanceID++;
				auto panel = CreateRef<T>(instanceID, std::forward<Args>(args)...);
				panel->SetContext(ctx);
				panel->OnAttach();
				list.emplace_back(panel);
				return *panel;
			}
			else
			{
				if (!list.empty())
					return *static_cast<T*>(list.front().get());

				auto panel = CreateRef<T>(std::forward<Args>(args)...);
				panel->SetContext(ctx);
				panel->OnAttach();
				list.emplace_back(panel);
				return *panel;
			}
		}

		template<typename T>
		T* GetPanel()
		{
			PanelType id = T::StaticPanelID;
			auto it = m_Panels.find(id);
			if (it == m_Panels.end() || it->second.empty())
				return nullptr;

			return static_cast<T*>(it->second.front().get());
		}

	private:
		std::unordered_map<PanelType, std::vector<Ref<EditorPanel>>> m_Panels;
		uint32_t m_NextInstanceID = 1;
	};
}
