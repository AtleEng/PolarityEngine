#pragma once
#include "EditorPanel.h"

namespace Polarity
{
    struct FloatPropertySettings
    {
        float Speed = 0.1f;

        bool HasMin = false;
        bool HasMax = false;

        float Min = 0.0f;
        float Max = 0.0f;

        bool HasReset = false;
        float ResetValue = 0.0f;

        const char* Tooltip = nullptr;
    };
    struct IntPropertySettings
    {
        int Speed = 1;

        bool HasMin = false;
        bool HasMax = false;

        int Min = 0;
        int Max = 0;

        bool HasReset = false;
        int ResetValue = 0;

        const char* Tooltip = nullptr;
    };
    struct BoolPropertySettings
    {
        bool HasReset = false;
        bool ResetValue = false;

        const char* Tooltip = nullptr;
    };
    struct StringPropertySettings
    {
        bool HasReset = false;
        const char* ResetValue = "";

        const char* Tooltip = nullptr;
    };
    struct AssetPropertySettings
    {
        AssetType Type = AssetType::None;

        const char* Tooltip = nullptr;
    };

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
        static bool Property(const char* label, float& value, const FloatPropertySettings& settings = {});
        static bool Property(const char* label, int& value, const IntPropertySettings& settings = {});
        static bool Property(const char* label, bool& value, const BoolPropertySettings& settings = {});
        static bool Property(const char* label, std::string& value, const StringPropertySettings& settings = {});

        static bool Property(const char* label, glm::vec2& value, const FloatPropertySettings& settings = {});
        static bool Property(const char* label, glm::vec3& value, const FloatPropertySettings& settings = {});

        static bool PropertyColor(const char* label, glm::vec4& value);
        static bool PropertyCombo(const char* label, int& currentIndex, const char* const* items, int itemCount, const char* tooltip = nullptr);
        static bool PropertyAsset(const char* label, AssetHandle& asset, const AssetPropertySettings& settings = {});

        void DrawTopbar(Entity entity);
        void DrawProperites(Entity entity);
    private:
        char m_NameBuf[256] = "";
        Entity m_lastSelected = {};

        bool m_Locked = false;
    };
}