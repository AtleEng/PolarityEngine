#pragma once

namespace Polarity
{
    #define POL_EXPOSE_VAR(...)
    
    class ScriptableEntity;
    class ScriptAsset;

    using ScriptFactoryFn = ScriptableEntity * (*)();

    enum class FieldType
    {
        None = 0,
        Float,
        Int,
        Bool
    };

    // A field in a script
    struct ScriptField
    {
        std::string Name;
        FieldType Type;
        size_t Offset;
    };
    // An instance of a specific field in a script
    struct ScriptFieldInstance
    {
        ScriptField Field;

        ScriptFieldInstance()
        {
            memset(m_Buffer, 0, sizeof(m_Buffer));
        }

        void* GetData()
        {
            return m_Buffer;
        }

        const void* GetData() const
        {
            return m_Buffer;
        }

    private:
        uint8_t m_Buffer[16];

        friend class ScriptEngine;
        friend class ScriptableEntity;
    };
    // A script loaded from the dll
    struct ScriptTemplate
    {
        std::string Name;
        ScriptFactoryFn Factory;
        std::vector<ScriptField> Fields;
    };

}