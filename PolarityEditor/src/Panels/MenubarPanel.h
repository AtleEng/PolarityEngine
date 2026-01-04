#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    class MenubarPanel : public EditorPanel
    {
    public:
        static constexpr PanelID StaticPanelID = PanelID::Assets;
        //void OnAttach() override;
        //void OnDetach() override;
        void OnDraw() override;
    private:

    };
}
