#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    class AssetsPanel : public EditorPanel
    {
    public:
        const char* GetName() const override { return "Assets"; }

        void OnStart() override {}
        void OnDraw() override;
    private:

    };
}