#include <Polarity.h>
#include <engine/core/EntryPoint.h>

#include "DemoLayer.h"


class Sandbox : public Polarity::Application
{
public:
	Sandbox()
	{
		LOG_INFO("Starting application ...\n");

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