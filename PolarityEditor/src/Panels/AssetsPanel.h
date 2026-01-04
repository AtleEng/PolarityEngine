#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    class AssetsPanel : public EditorPanel
    {
    public:
        static constexpr PanelID StaticPanelID = PanelID::Assets;

        AssetsPanel()
            : EditorPanel(StaticPanelID, 0, "Assets") {}

        void OnDraw() override;

    private:

    };
}