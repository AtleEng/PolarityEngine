#pragma once
#include "engine/scene/ScriptableEntity.h"
#include "engine/scripting/ScriptingTypes.h"

#include <filesystem>
#include <string>
#include <map>

namespace Polarity
{
    using CreateFn = ScriptableEntity * (*)(const char*);

    class ScriptEngine
    {
    public:
        // Check if the DLL should reload, reload and return true if it does
        static bool Update(const std::filesystem::path& dllPath);

        // Create a ScriptableEntity from the DLL from name
        static ScriptableEntity* CreateInstance(const std::string& name);
        // Get a loaded ScriptTemplate from DLL from name
        static const Ref<ScriptTemplate> GetScript(std::string& name);
        // Get all loaded ScriptTemplates
        static const std::vector<ScriptTemplate> GetScripts();
        // Apply all Fields in a ScriptComponent to its Instance (ScriptableEntity)
        static void ApplyFieldsToInstance(ScriptComponent& component);
    private:
        // Load the DLL from path and regester all scripts in it
        static void Load(const std::filesystem::path& dllPath);
        // Copy file and waits for it to be copied (~1 sec)
        static bool TryCopyWithRetry(const std::filesystem::path& src, const std::filesystem::path& dst);
    private:
        static void* s_DLL;
        static long long m_LastEditTimestamp;
        static std::vector<ScriptTemplate> s_Scripts;
        static CreateFn s_CreateScript;
    };
}