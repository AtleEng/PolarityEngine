#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    class AssetsPanel : public EditorPanel
    {
    public:
        const char* GetName() const override { return "Assets"; }

        void OnImGuiRender(EditorContext& ctx) override;
    private:

    };
}