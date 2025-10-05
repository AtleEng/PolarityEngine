#include <Atlas.h>

class TestLayer : public Atlas::Layer
{
public:
	TestLayer()
		: Layer("Test")
	{

	}

	void OnUpdate() override
	{
		//LOG_DEBUG("TestLayer: Update");
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