#include "polpch.h"
#include "ScriptingEngine.h"

#include "engine/utils/FileUtils.h"
#include "engine/utils/PlatformUtils.h"

namespace Polarity
{
	void* ScriptEngine::s_DLL;
	long long ScriptEngine::m_LastEditTimestamp;
	std::vector<ScriptTemplate> ScriptEngine::s_Scripts;

	void ScriptEngine::Init()
	{

	}

	void ScriptEngine::Shutdown()
	{

	}
	// Load the DLL from path and regester all scripts in it
	void ScriptEngine::Load(const std::filesystem::path& dllPath)
	{
		POL_CORE_TRACE("Scripting: Loading DLL..");
		s_DLL = DynamicLib::LoadDynamicLib(dllPath.string().c_str());
		POL_CORE_ASSERT(s_DLL, "Scripting: Failed to load dll");

		auto registerScripts = (void(*)())DynamicLib::LoadDynamicFunction(s_DLL, "RegisterAllScripts");
		registerScripts();
		POL_CORE_TRACE("Scripting: DLL Loaded");
	}

	// Check if the DLL should reload, reload and return true if it does
	bool ScriptEngine::Reload(const std::filesystem::path& dllPath)
	{
		long long currentTimestamp = GetTimestamp(dllPath.string().c_str());
		if (currentTimestamp > m_LastEditTimestamp)
		{
			m_LastEditTimestamp = currentTimestamp;
			if (s_DLL)
			{
				bool freeResult = DynamicLib::FreeDynimicLib(s_DLL);
				POL_CORE_ASSERT(freeResult, "Scripting: Failed to free DLL");
				s_DLL = nullptr;
				POL_CORE_TRACE("Scripting: Freed DLL");
			}

			Load(dllPath);

			auto getScripts = (void(*)(std::vector<ScriptTemplate>&))DynamicLib::LoadDynamicFunction(s_DLL, "GetScripts");
			getScripts(s_Scripts);
			return true;
		}
		return false;
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
		auto scriptableEntity = createScript(name.c_str());
		

		return scriptableEntity;
	}

	void ScriptEngine::Destroy(ScriptableEntity* instance)
	{
	}

	const Ref<ScriptTemplate> ScriptEngine::GetScript(std::string& name)
	{
		for (auto temp : s_Scripts)
		{
			if (temp.Name == name)
				return CreateRef<ScriptTemplate>(temp);
		}
		POL_CORE_DEBUG("Scripting: No script found with name: %s", name.c_str());
		return nullptr;
	}

	const std::vector<ScriptTemplate> ScriptEngine::GetScripts()
	{
		return s_Scripts;
	}
}
