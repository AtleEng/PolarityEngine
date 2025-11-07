#include <Polarity.h>

#include "imgui/imgui.h"

using namespace Polarity;

class TestLayer : public Polarity::Layer
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
	void OnEvent(Polarity::Event& event) override
	{
		//LOG_DEBUG("%s", event.GetName());
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Test");
		ImGui::Text("Hello World");
		ImGui::End();
	}
};

class Sandbox : public Polarity::Application
{
public:
	Sandbox()
	{
		LOG_INFO("Starting application...\n");

		PushLayer(new TestLayer());
	}
	~Sandbox()
	{

	}

};

Polarity::Application* Polarity::CreateApplication()
{
	return new Sandbox();
}