#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    class InspectorPanel : public EditorPanel
    {
    public:
        static constexpr PanelID StaticPanelID = PanelID::Properties;
        static constexpr bool AllowMultiple() { return true; }

        InspectorPanel(uint32_t instanceID)
            : EditorPanel(StaticPanelID, instanceID, "Propertities") {}

        //void OnAttach() override;
        //void OnDetach() override;
        void OnDraw() override;
    private:
        char m_NameBuf[256] = "";
        ECS::Entity m_lastSelected = ECS::INVALID_ENTITY;

        bool m_Locked = false;
    };
}