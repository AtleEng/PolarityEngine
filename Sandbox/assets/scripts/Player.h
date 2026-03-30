#pragma once
#include <Polarity.h>

using namespace Polarity;

class Player : public ScriptableEntity
{
public:
	float speed = 5.0f;

	void OnCreate();

	void OnDestroy();

	void OnUpdate(Timestep ts);

};
