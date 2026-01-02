#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    class InspectorPanel : public EditorPanel
    {
    public:
        const char* GetName() const override { return "Inspector"; }

        void OnStart() override;
        void OnDraw() override;
    private:
        char m_textBuffer[256] = ""; //byt namn
        ECS::Entity m_lastSelected = ECS::INVALID_ENTITY;
    };
}