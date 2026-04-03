#pragma once
#include <Polarity.h>
using namespace Polarity;

struct ScriptRegistry
{
	std::unordered_map<std::string, ScriptClass> Scripts;
};