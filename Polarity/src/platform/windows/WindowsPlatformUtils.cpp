#include "polpch.h"
#include "engine/utils/PlatformUtils.h"

//Windows only
#include <commdlg.h> 
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "engine/core/Application.h"

namespace Polarity
{
	std::string FileDialogs::OpenFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
	}

	std::string FileDialogs::SaveFile(const char* filter)
	{
		OPENFILENAMEA ofn;
		CHAR szFile[260] = { 0 };

		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn) == TRUE)
			return ofn.lpstrFile;

		return std::string();
	}

	float Time::GetTime()
	{
		return glfwGetTime();
	}

	void* DynamicLib::LoadDynamicLib(const char* dll)
	{
		HMODULE result = LoadLibraryA(dll);
		POL_CORE_ASSERT(result, "Windows Utils: Failed to load dll: %s!", dll)
		return result;
	}

	void* DynamicLib::LoadDynamicFunction(void* dll, const char* funName)
	{
		FARPROC proc = GetProcAddress((HMODULE)dll, funName);
		POL_CORE_ASSERT(proc, "Windows Utils: Failed to load function: %s from DLL!", funName);

		return (void*)proc;
	}

	bool DynamicLib::FreeDynimicLib(void* dll)
	{
		BOOL freeResult = FreeLibrary((HMODULE)dll);
		POL_CORE_ASSERT(freeResult, "Windows Utils: Failed to free lib!");

		return (bool)freeResult;
	}

}