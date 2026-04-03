#include "polpch.h"
#include "ScriptingEngine.h"

#include "engine/utils/FileUtils.h"
#include "engine/utils/PlatformUtils.h"

namespace Polarity
{
	void* ScriptEngine::s_DLL;
	long long ScriptEngine::m_LastEditTimestamp;
	std::unordered_map<std::string, ScriptClass> ScriptEngine::s_Scripts;

	void ScriptEngine::Init()
	{

	}

	void ScriptEngine::Shutdown()
	{

	}

	void ScriptEngine::Load(const std::filesystem::path& dllPath)
	{
		POL_TRACE("Loading DLL..");
		s_DLL = DynamicLib::LoadDynamicLib(dllPath.string().c_str());
		POL_CORE_ASSERT(s_DLL, "Failed to load dll");

		auto registerScripts = (void(*)())DynamicLib::LoadDynamicFunction(s_DLL, "RegisterAllScripts");
		registerScripts();
		POL_TRACE("DLL Loaded");
	}

	void ScriptEngine::Reload(const std::filesystem::path& dllPath)
	{
		long long currentTimestamp = GetTimestamp(dllPath.string().c_str());
		if (currentTimestamp > m_LastEditTimestamp)
		{
			m_LastEditTimestamp = currentTimestamp;
			if (s_DLL)
			{
				bool freeResult = DynamicLib::FreeDynimicLib(s_DLL);
				POL_CORE_ASSERT(freeResult, "Failed to free DLL");
				s_DLL = nullptr;
				POL_CORE_TRACE("Freed DLL");
			}

			Load(dllPath);

			std::vector<ScriptClass> outNames = {};
			auto getScriptsNames = (void(*)(std::vector<ScriptClass>&))DynamicLib::LoadDynamicFunction(s_DLL, "GetScripts");
			getScriptsNames(outNames);

			s_Scripts.clear();
			for (int i = 0; i < outNames.size(); i++)
			{
				s_Scripts.emplace(outNames[i].Name, outNames[i]);
				POL_CORE_TRACE(outNames[i].Name.c_str());
			}	
		}
	}

	void ScriptEngine::OnRuntimeStart(Scene* scene)
	{
	}

	void ScriptEngine::OnRuntimeStop()
	{
	}

	ScriptableEntity* ScriptEngine::Create(const std::string& name)
	{
		auto createScript = (ScriptableEntity * (*)(const char*))DynamicLib::LoadDynamicFunction(s_DLL, "CreateScript");
		return createScript(name.c_str());
	}

	void ScriptEngine::Destroy(ScriptableEntity* instance)
	{
	}

	const ScriptClass* ScriptEngine::GetScript(std::string& name)
	{
		auto it = s_Scripts.find(name);
		if (it == s_Scripts.end())
		{
			POL_ERROR("Script: %s not found!", name.c_str());
			return nullptr;
		}

		return &it->second;
	}

	const std::vector<std::string> ScriptEngine::GetScriptsID()
	{
		std::vector<std::string> ids;
		ids.reserve(s_Scripts.size());

		for (const auto& [key, value] : s_Scripts)
		{
			ids.push_back(key);
		}

		return ids;
	}

}
