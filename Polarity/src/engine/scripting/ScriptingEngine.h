#pragma once
#include "engine/scene/ScriptableEntity.h"
#include "engine/scripting/ScriptingTypes.h"

#include <filesystem>
#include <string>
#include <map>

namespace Polarity
{
    class ScriptEngine
    {
    public:
        static void Init();
        static void Shutdown();

        static void Load(const std::filesystem::path& dllPath);
        static bool Reload(const std::filesystem::path& dllPath);

        static void OnRuntimeStart(Scene* scene);
        static void OnRuntimeStop();

        static ScriptableEntity* Create(const std::string& name);
        static void Destroy(ScriptableEntity* instance);

        static const Ref<ScriptTemplate> GetScript(std::string& name);
        static const std::vector<ScriptTemplate> GetScripts();

    private:
        static void* s_DLL;
        static long long m_LastEditTimestamp;
        static std::vector<ScriptTemplate> s_Scripts;
    };
}