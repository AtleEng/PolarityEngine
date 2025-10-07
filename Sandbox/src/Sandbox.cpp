#include <Atlas.h>
using namespace Atlas;

class TestLayer : public Atlas::Layer
{
public:
	TestLayer()
		: Layer("Test")
	{

	}

	void OnUpdate() override
	{
		
		if (Input::IsKeyPressed(Key::Space))
			LOG_DEBUG("Jump!");

		if (Input::IsMouseButtonPressed(Mouse::Button0))
		{
			auto [x, y] = Input::GetMousePosition();
			LOG_DEBUG("Klicked at: [%f %f]", x, y);
		}
		
	}
	void OnEvent(Atlas::Event& event) override
	{
		//LOG_DEBUG("%s", event.GetName());
	}
};

class Sandbox : public Atlas::Application
{
public:
	Sandbox()
	{
		LOG_INFO("App initializing...");

		PushLayer(new TestLayer());
		PushOverlay(new Atlas::ImGuiLayer());
	}
	~Sandbox()
	{

	}

};

Atlas::Application* Atlas::CreateApplication()
{
	return new Sandbox();
}