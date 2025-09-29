#include <Atlas.h>


class Sandbox : public Atlas::Application
{
public:
	Sandbox()
	{
		LOG_INFO("App initializing...");
	}
	~Sandbox()
	{

	}

};

Atlas::Application* Atlas::CreateApplication()
{
	return new Sandbox();
}