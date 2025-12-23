#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    class MenubarPanel : public EditorPanel
    {
    public:
        const char* GetName() const override { return "MenubarPanel"; }

        void OnImGuiRender(EditorContext& ctx) override;
    private:

    };
}
