#include "ScriptRegistry.h"
#include "Player.h"
#include "Cam.h"

static ScriptRegistry s_Registry;

template<typename T>
ScriptableEntity* CreateInstance()
{
    return new T();
}

template<typename T>
void RegisterScript(const std::string& name)
{
    s_Registry.Factories[name] = &CreateInstance<T>;
}

extern "C"
{
    POL_EXPORT_FN void RegisterAllScripts()
    {
        RegisterScript<Player>("Player");
        RegisterScript<Cam>("Cam");
    }

    POL_EXPORT_FN ScriptableEntity* CreateScript(const char* name)
    {
        auto it = s_Registry.Factories.find(name);
        if (it != s_Registry.Factories.end())
            return it->second();

        return nullptr;
    }

    POL_EXPORT_FN void DestroyScript(ScriptableEntity* ptr)
    {
        delete ptr;
    }

    POL_EXPORT_FN void GetScriptNames(std::vector<const char*>& outNames)
    {
        for (auto& [name, _] : s_Registry.Factories)
            outNames.push_back(name.c_str());
    }
}