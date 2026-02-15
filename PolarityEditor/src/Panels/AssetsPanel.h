#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    class AssetsPanel : public EditorPanel
    {
    public:
        static constexpr PanelType StaticPanelID = PanelType::Assets;

        AssetsPanel()
            : EditorPanel(StaticPanelID, 0, "Assets") {}

        void OnDraw() override;

    private:
        
    };
}