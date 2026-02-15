#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    class PropertitiesPanel : public EditorPanel
    {
    public:
        static constexpr PanelType StaticPanelID = PanelType::Properties;
        static constexpr bool AllowMultiple() { return true; }

        PropertitiesPanel(uint32_t instanceID)
            : EditorPanel(StaticPanelID, instanceID, "Propertities") {}

        //void OnAttach() override;
        //void OnDetach() override;
        void OnDraw() override;

    private:
        char m_NameBuf[256] = "";
        Entity m_lastSelected = {};

        bool m_Locked = false;
    };
}