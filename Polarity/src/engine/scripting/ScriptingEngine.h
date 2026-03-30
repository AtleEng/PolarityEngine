#pragma once
#include "engine/scene/ScriptableEntity.h"

#include <filesystem>

namespace Polarity
{
    class ScriptEngine
    {
    public:
        static void Init();
        static void Update(const std::filesystem::path& dllPath);
        static void Shutdown();

        static ScriptableEntity* Create(const std::string& name);
        static void Destroy(ScriptableEntity* instance);

        static const std::vector<std::string>& GetScriptNames();

    private:
        static void* s_DLL;
        static long long m_LastEditTimestamp;
        static std::vector<std::string> s_ScriptNames;

        static ScriptableEntity* (*s_CreateScript)(const char*);
        static void (*s_DestroyScript)(ScriptableEntity*);
    };
}