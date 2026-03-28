#pragma once


#include <Polarity.h>

using namespace Polarity;


class CamControll : public ScriptableEntity
{
public:
	float speed = 5.0f;

	void OnCreate();

	void OnDestroy();

	void OnUpdate(Timestep ts);

private:
	float test = 1.0f;
};
