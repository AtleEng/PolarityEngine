#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    class HierarcyPanel : public EditorPanel
    {
    public:
        const char* GetName() const override { return "Hierarcy"; }

        void OnStart() override;
        void OnDraw() override;
    private:
        void DrawEntityNode(Entity entity, EditorContext* ctx);
    private:
        Entity m_HoveredEntity;
    };
}