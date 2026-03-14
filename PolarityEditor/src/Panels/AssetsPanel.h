#pragma once
#include "EditorPanel.h"

#include <filesystem>

namespace Polarity
{
    class AssetsPanel : public EditorPanel
    {
    public:
        static constexpr PanelType StaticPanelID = PanelType::Assets;

        AssetsPanel();

        void OnDraw() override;

    private:
        std::filesystem::path m_BaseDirectory;
        std::filesystem::path m_CurrentDirectory;
    };
}