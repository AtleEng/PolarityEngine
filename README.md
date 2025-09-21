# Engine
A simple game engine for 2D and 3D games


How to use:

#include <Atlas.h>


class Sandbox : public Atlas::Application
{
public:
	Sandbox()
	{

	}
	~Sandbox()
	{

	}

};

Atlas::Application* Atlas::CreateApplication()
{
	return new Sandbox();
}