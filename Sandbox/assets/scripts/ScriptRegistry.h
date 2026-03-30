#pragma once
#include <Polarity.h>
using namespace Polarity;

using ScriptFactoryFn = ScriptableEntity * (*)();

struct ScriptRegistry
{
	std::unordered_map<std::string, ScriptFactoryFn> Factories;
};