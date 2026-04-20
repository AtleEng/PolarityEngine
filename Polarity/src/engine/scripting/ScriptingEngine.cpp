#include "polpch.h"
#include "ScriptingEngine.h"

#include "engine/utils/FileUtils.h"
#include "engine/utils/PlatformUtils.h"

namespace Polarity
{
	void* ScriptEngine::s_DLL;
	long long ScriptEngine::m_LastEditTimestamp;
	std::vector<ScriptTemplate> ScriptEngine::s_Scripts;
	CreateFn ScriptEngine::s_CreateScript = nullptr;

	bool ScriptEngine::Update(const std::filesystem::path& dllPath)
	{
		long long currentTimestamp = GetTimestamp(dllPath.string().c_str());
		if (currentTimestamp > m_LastEditTimestamp)
		{
			POL_CORE_TRACE("Detected new version of DLL Reloading...");
			m_LastEditTimestamp = currentTimestamp;

			//Copy dll to dll_load
			std::filesystem::path tempDir = "bin/temp";

			// ensure directory exists
			std::filesystem::create_directories(tempDir);

			// generate unique filename
			static int version = 0;
			version++;

			std::string filename = dllPath.stem().string();
			std::string ext = dllPath.extension().string();

			std::filesystem::path tempPath =
				tempDir / (filename + "_" + std::to_string(version) + ext);

			if (!TryCopyWithRetry(dllPath, tempPath))
			{
				POL_CORE_ERROR("Failed to copy DLL (timeout)");
				return false;
			}

			if (s_DLL)
			{
				bool freeResult = DynamicLib::FreeDynimicLib(s_DLL);
				POL_CORE_ASSERT(freeResult, "Failed to free DLL");
				s_CreateScript = nullptr;

				POL_CORE_TRACE("Freed old DLL");
			}

			Load(tempPath);
			return true;
		}
		return false;
	}

	void ScriptEngine::Load(const std::filesystem::path& dllPath)
	{
		POL_CORE_TRACE("Loading DLL %s...", dllPath.stem().string().c_str());
		s_DLL = DynamicLib::LoadDynamicLib(dllPath.string().c_str());
		POL_CORE_ASSERT(s_DLL, "Failed to load dll");

		auto registerScripts = (void(*)())DynamicLib::LoadDynamicFunction(s_DLL, "RegisterAllScripts");
		registerScripts();

		auto getScripts = (void(*)(std::vector<ScriptTemplate>&))DynamicLib::LoadDynamicFunction(s_DLL, "GetScripts");
		getScripts(s_Scripts);

		s_CreateScript = (CreateFn)
			DynamicLib::LoadDynamicFunction(s_DLL, "CreateScript");

		POL_CORE_TRACE("DLL %s Loaded", dllPath.stem().string().c_str());
	}

	bool ScriptEngine::TryCopyWithRetry(const std::filesystem::path& src, const std::filesystem::path& dst)
	{
		for (int i = 0; i < 20; i++)
		{
			try
			{
				std::filesystem::copy_file(
					src,
					dst,
					std::filesystem::copy_options::overwrite_existing
				);
				return true;
			}
			catch (const std::filesystem::filesystem_error&)
			{
				Sleep(50);
			}
		}
		return false;
	}
	
	ScriptableEntity* ScriptEngine::CreateInstance(const std::string& name) //TODO cache per script to avoid string comperison
	{
		return s_CreateScript(name.c_str());
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
	
	void ScriptEngine::ApplyFieldsToInstance(ScriptComponent& component)
	{
		if (!component.Instance)
		{
			POL_CORE_WARN("ScriptEngine: ApplyFieldsToInstance failed, ScriptComponent doesn't have a instance!");
			return;
		}

		for (auto& fieldInstance : component.StoredFields)
		{
			const ScriptField& field = fieldInstance.Field;

			ScriptableEntity* instance = component.Instance.get();
			uint8_t* instancePtr = reinterpret_cast<uint8_t*>(instance);
			void* destination = instancePtr + field.Offset;
			const void* source = fieldInstance.GetData();

			switch (field.Type)
			{
			case FieldType::Float:
				*(float*)destination = *(float*)source;
				break;
			case FieldType::Int:
				*(int*)destination = *(int*)source;
				break;
			case FieldType::Bool:
				*(bool*)destination = *(bool*)source;
				break;
			}
		}
	}
}
