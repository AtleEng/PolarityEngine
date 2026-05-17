#pragma once

#include <string>

namespace Polarity
{
	class FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};

	class Time
	{
	public:
		static float GetTime();
	};

	class DynamicLib
	{
	public:
		static void* LoadDynamicLib(const char* dll);
		static void* LoadDynamicFunction(void* dll, const char* funName);
		static bool  FreeDynimicLib(void* dll);
	};
}
