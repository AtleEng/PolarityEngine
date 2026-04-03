#pragma once
#include "engine/scene/ScriptableEntity.h"

#include <filesystem>
#include <string>
#include <map>

namespace Polarity
{
    #define POL_EXPOSE_VAR(...)

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

        template<typename T>
        T GetValue()
        {
            static_assert(sizeof(T) <= 16, "Type too large!");
            return *(T*)m_Buffer;
        }

        template<typename T>
        void SetValue(T value)
        {
            static_assert(sizeof(T) <= 16, "Type too large!");
            memcpy(m_Buffer, &value, sizeof(T));
        }
    private:
        uint8_t m_Buffer[16];

        friend class ScriptEngine;
        friend class ScriptableEntity;
    };
    // A script loaded from the dll
    struct ScriptClass
    {
        std::string Name;
        ScriptFactoryFn Factory;
        std::vector<ScriptField> Fields;
    };


    class ScriptEngine
    {
    public:
        static void Init();
        static void Shutdown();

        static void Load(const std::filesystem::path& dllPath);
        static void Reload(const std::filesystem::path& dllPath);

        static void OnRuntimeStart(Scene* scene);
        static void OnRuntimeStop();

        static ScriptableEntity* Create(const std::string& name);
        static void Destroy(ScriptableEntity* instance);

        static const ScriptClass* GetScript(std::string& name);
        static const std::vector<std::string> GetScriptsID();

    private:
        static void* s_DLL;
        static long long m_LastEditTimestamp;
        static std::unordered_map<std::string, ScriptClass> s_Scripts;

        static ScriptableEntity* (*s_CreateScript)(const char*);
        static void (*s_DestroyScript)(ScriptableEntity*);
    };
}