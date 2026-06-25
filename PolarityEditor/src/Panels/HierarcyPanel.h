#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    class HierarcyPanel : public EditorPanel
    {
    public:
        static constexpr PanelType StaticPanelID = PanelType::SceneHierarchy;
        static constexpr bool AllowMultiple() { return false; }

        HierarcyPanel()
            : EditorPanel(StaticPanelID, 0, "SceneHierarcy") {}

        //void OnAttach() override;
        void OnDraw() override;

    private:
        void DrawEntityNode(Entity entity);
        void DrawAddEntityMenu();
    private:
        Entity m_HoveredEntity;
    };
}