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
void RegisterScript(const std::string& name, std::initializer_list<ScriptField> fields)
{
    ScriptTemplate sc;
    sc.Name = name;
    sc.Factory = &CreateInstance<T>;
    sc.Fields = fields;

    s_Registry.Scripts[name] = sc;
}

extern "C"
{
    POL_EXPORT_FN void RegisterAllScripts()
    {
        RegisterScript<Player>("Player",{});

        RegisterScript<Cam>("Cam", {
            { "speed", FieldType::Float, offsetof(Cam, speed) },
            { "testFloat", FieldType::Float, offsetof(Cam, testFloat) },
            { "testInt", FieldType::Int, offsetof(Cam, testInt) },
            { "testBool", FieldType::Bool, offsetof(Cam, testBool) }
            });
    }

    POL_EXPORT_FN ScriptableEntity* CreateScript(const char* name)
    {
        auto it = s_Registry.Scripts.find(name);
        if (it != s_Registry.Scripts.end())
            return it->second.Factory();

        return nullptr;
    }

    POL_EXPORT_FN void DestroyScript(ScriptableEntity* ptr)
    {
        delete ptr;
    }

    POL_EXPORT_FN void GetScripts(std::vector<ScriptTemplate>& outScripts)
    {
        for (auto& [_, it] : s_Registry.Scripts)
            outScripts.push_back(it);
    }

    POL_EXPORT_FN void GetScriptNames(std::vector<const char*>& outNames)
    {
        for (auto& [name, _] : s_Registry.Scripts)
            outNames.push_back(name.c_str());
    }
}