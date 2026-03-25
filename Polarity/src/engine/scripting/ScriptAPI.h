#pragma once

#include <string>
#include <unordered_map>
#include <functional>

namespace Polarity {

    class ScriptableEntity;

    // Factory function type
    using ScriptFactory = std::function<ScriptableEntity* ()>;

    class ScriptRegistry
    {
    public:
        void Register(const std::string& name, ScriptFactory factory)
        {
            m_Factories[name] = factory;
        }

        ScriptableEntity* Create(const std::string& name)
        {
            if (m_Factories.find(name) == m_Factories.end())
                return nullptr;

            return m_Factories[name]();
        }

    private:
        std::unordered_map<std::string, ScriptFactory> m_Factories;
    };

}