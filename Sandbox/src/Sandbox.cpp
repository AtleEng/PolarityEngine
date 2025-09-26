#include <Atlas.h>


class Sandbox : public Atlas::Application
{
public:
	Sandbox()
	{
		Log(LogPrefix::Info, "App initializing...");
	}
	~Sandbox()
	{

	}

};

Atlas::Application* Atlas::CreateApplication()
{
	return new Sandbox();
}