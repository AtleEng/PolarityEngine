#pragma once


#include <Polarity.h>

using namespace Polarity;


class Cam : public ScriptableEntity
{
public:
	POL_EXPOSE_VAR()
	float speed = 5.0f;
	POL_EXPOSE_VAR()
	float testFloat = 1.0f;
	POL_EXPOSE_VAR()
	int testInt = 3;
	POL_EXPOSE_VAR()
	bool testBool = true;

	void OnCreate();

	void OnDestroy();

	void OnUpdate(Timestep ts);
};
