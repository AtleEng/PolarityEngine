#pragma once
#include "engine/scene/ScriptableEntity.h"
#include "engine/scripting/ScriptingTypes.h"

#include <filesystem>
#include <string>
#include <map>

namespace Polarity
{
    // An instance of a specific script
    class ScriptAsset
    {
    public:
        ScriptAsset(const ScriptTemplate& scriptTemplate);
        ~ScriptAsset();

        static Ref<ScriptAsset> Create(const ScriptTemplate& scriptTemplate);

        const ScriptTemplate& m_Template;

        std::vector<ScriptField> m_Fields;
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

        static const ScriptTemplate* GetScript(std::string& name);
        static const std::vector<ScriptTemplate> GetScripts();

    private:
        static void* s_DLL;
        static long long m_LastEditTimestamp;
        static std::unordered_map<std::string, ScriptTemplate> s_Scripts;

        static ScriptableEntity* (*s_CreateScript)(const char*);
        static void (*s_DestroyScript)(ScriptableEntity*);
    };
}