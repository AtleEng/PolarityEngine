#pragma once
#include "EditorPanel.h"

#include <algorithm>

namespace Polarity
{
    // TODO

    class GamePreviewPanel : public EditorPanel
    {   
    public:
        static constexpr PanelID StaticPanelID = PanelID::Preview;

        GamePreviewPanel()
            : EditorPanel(StaticPanelID, 0, "Preview") {}

        void OnDraw() override;
 
        bool IsFocused() { return m_ViewportFocused; }
        bool IsHovered() { return m_ViewportHovered; }

    private:
        bool m_ViewportFocused = false;
        bool m_ViewportHovered = false;
    };
}