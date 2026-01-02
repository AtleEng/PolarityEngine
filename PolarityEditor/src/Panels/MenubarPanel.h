#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    class MenubarPanel : public EditorPanel
    {
    public:
        const char* GetName() const override { return "MenubarPanel"; }

        void OnStart() override {}
        void OnDraw() override;
    private:

    };
}
