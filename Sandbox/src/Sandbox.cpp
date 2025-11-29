#include <Polarity.h>
#include <engine/core/EntryPoint.h>

#include "DemoLayer.h"


class Sandbox : public Polarity::Application
{
public:
	Sandbox()
	{
		POLARITY_PROFILE_FUNCTION();

		PushLayer(new DemoLayer());
	}
	~Sandbox()
	{

	}

};

Polarity::Application* Polarity::CreateApplication()
{
	return new Sandbox();
}