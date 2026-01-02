#pragma once
#include "../EditorLayer.h"
#include "EditorPanel.h"

namespace Polarity {

class PanelManager
{
public:
	static void Init();

	template<typename T, typename... Args>
static Ref<T> AddPanel(Args&&... args)
{
    static_assert(std::is_base_of_v<EditorPanel, T>);

    auto panel = CreateRef<T>(std::forward<Args>(args)...);
    panel->OnStart();
    m_Panels.emplace_back(panel);
    return panel;
}

	static void OpenPanel();
	static void ClosePanel();

	template<typename T>
	static T* GetPanel()
	{
        static_assert(std::is_base_of_v<EditorPanel, T>);

        for (const auto& panel : m_Panels)
        {
            if (auto casted = std::dynamic_pointer_cast<T>(panel))
                return casted.get();
        }
        return nullptr;
	}

	static void OnDraw();

	static void SetContext(EditorContext& ctx);
private:
	static std::vector<Ref<EditorPanel>> m_Panels;
};
}
