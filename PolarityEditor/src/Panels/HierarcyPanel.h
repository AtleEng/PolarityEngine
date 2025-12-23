#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    class HierarcyPanel : public EditorPanel
    {
    public:
        const char* GetName() const override { return "Hierarcy"; }

        void OnImGuiRender(EditorContext& ctx) override;
    private:
        
    };
}