#pragma once


#include <Polarity.h>

using namespace Polarity;


class Cam : public ScriptableEntity
{
public:
	float speed = 5.0f;

	void OnCreate();

	void OnDestroy();

	void OnUpdate(Timestep ts);
};
