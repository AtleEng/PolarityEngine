#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    class HierarcyPanel : public EditorPanel
    {
    public:
        static constexpr PanelID StaticPanelID = PanelID::SceneHierarchy;
        static constexpr bool AllowMultiple() { return false; }

        HierarcyPanel()
            : EditorPanel(StaticPanelID, 0, "SceneHierarcy") {}

        void OnDraw() override;

    private:
        void DrawEntityNode(Entity entity, EditorContext* ctx);
    private:
        Entity m_HoveredEntity;
    };
}