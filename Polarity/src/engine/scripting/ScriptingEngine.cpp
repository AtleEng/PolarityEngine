#include "polpch.h"
#include "ScriptingEngine.h"

#include "engine/utils/FileUtils.h"
#include "engine/utils/PlatformUtils.h"

namespace Polarity
{
	void* ScriptEngine::s_DLL;
	long long ScriptEngine::m_LastEditTimestamp;
	std::vector<std::string> ScriptEngine::s_ScriptNames;

	void ScriptEngine::Init()
	{

	}

	void ScriptEngine::Update(const std::filesystem::path& dllPath)
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

			POL_TRACE("Loading DLL..");
			s_DLL = DynamicLib::LoadDynamicLib(dllPath.string().c_str());
			POL_CORE_ASSERT(s_DLL, "Failed to load dll");

			POL_TRACE("DLL Loaded");
			auto registerScripts = (void(*)())DynamicLib::LoadDynamicFunction(s_DLL, "RegisterAllScripts");
			registerScripts();

			std::vector<const char*> outNames = {};
			auto getScriptsNames = (void(*)(std::vector<const char*>&))DynamicLib::LoadDynamicFunction(s_DLL, "GetScriptNames");
			getScriptsNames(outNames);

			s_ScriptNames.clear();
			for (int i = 0; i < outNames.size(); i++)
			{
				s_ScriptNames.push_back(outNames[i]);
				POL_CORE_TRACE(s_ScriptNames[i].c_str());
			}

			
		}
	}
	void ScriptEngine::Shutdown()
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

	const std::vector<std::string>& ScriptEngine::GetScriptNames()
	{
		return s_ScriptNames;
	}

}
