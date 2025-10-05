#include "atlaspch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "platform/openGL/ImGuiOpenGLRenderer.h"
#include "engine/core/Keycodes.h"

#include "engine/Application.h"

//temp
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Atlas
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{

	}
	ImGuiLayer::~ImGuiLayer()
	{

	}

	void ImGuiLayer::OnAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		io.KeyMap[ImGuiKey_Tab] = Key::Tab;
		io.KeyMap[ImGuiKey_LeftArrow] = Key::Left;
		io.KeyMap[ImGuiKey_RightArrow] = Key::Right;
		io.KeyMap[ImGuiKey_UpArrow] = Key::Up;
		io.KeyMap[ImGuiKey_DownArrow] = Key::Down;
		io.KeyMap[ImGuiKey_PageUp] = Key::PageUp;
		io.KeyMap[ImGuiKey_PageDown] = Key::PageDown;
		io.KeyMap[ImGuiKey_Home] = Key::Home;
		io.KeyMap[ImGuiKey_End] = Key::End;
		io.KeyMap[ImGuiKey_Insert] = Key::Insert;
		io.KeyMap[ImGuiKey_Delete] = Key::Delete;
		io.KeyMap[ImGuiKey_Backspace] = Key::Backspace;
		io.KeyMap[ImGuiKey_Space] = Key::Space;
		io.KeyMap[ImGuiKey_Enter] = Key::Enter;
		io.KeyMap[ImGuiKey_Escape] = Key::Escape;
		io.KeyMap[ImGuiKey_A] = Key::A;
		io.KeyMap[ImGuiKey_C] = Key::C;
		io.KeyMap[ImGuiKey_V] = Key::V;
		io.KeyMap[ImGuiKey_X] = Key::X;
		io.KeyMap[ImGuiKey_Y] = Key::Y;
		io.KeyMap[ImGuiKey_Z] = Key::Z;

		ImGui_ImplOpenGL3_Init("#version 410");
	}
	void ImGuiLayer::OnDetach()
	{

	}
	void ImGuiLayer::OnUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		float time = (float)glfwGetTime();
		io.DeltaTime = m_time > 0.0 ? (time - m_time) : (1.0f/60.0f); //Calc delta time, if invailid get 60 fps
		m_time = time;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	void ImGuiLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseButtonPressedEvent>(ATLAS_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressedEvent));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(ATLAS_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleasedEvent));
		dispatcher.Dispatch<MouseMovedEvent>(ATLAS_BIND_EVENT_FN(ImGuiLayer::OnMouseMovedEvent));
		dispatcher.Dispatch<MouseScrolledEvent>(ATLAS_BIND_EVENT_FN(ImGuiLayer::OnMouseScrolledEvent));

		dispatcher.Dispatch<KeyPressedEvent>(ATLAS_BIND_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
		dispatcher.Dispatch<KeyReleasedEvent>(ATLAS_BIND_EVENT_FN(ImGuiLayer::OnKeyReleasedEvent));
		dispatcher.Dispatch<KeyTypedEvent>(ATLAS_BIND_EVENT_FN(ImGuiLayer::OnKeyTypedEvent));

		dispatcher.Dispatch<WindowResizeEvent>(ATLAS_BIND_EVENT_FN(ImGuiLayer::OnWindowResizeEvent));
	}

	bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.GetMouseButton()] = true;

		return false;
	}
	bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.GetMouseButton()] = false;

		return false;
	}
	bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.GetX(), e.GetY());

		return false;
	}
	bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheel += e.GetYOffset();
		io.MouseWheelH += e.GetXOffset();

		return false;
	}
	bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.GetKeyCode()] = true;

		io.KeyCtrl = io.KeysDown[Key::LeftControl] || io.KeysDown[Key::RightControl];
		io.KeyShift = io.KeysDown[Key::LeftShift] || io.KeysDown[Key::RightShift];
		io.KeyAlt = io.KeysDown[Key::LeftAlt] || io.KeysDown[Key::RightAlt];
		io.KeySuper = io.KeysDown[Key::LeftSuper] || io.KeysDown[Key::RightSuper];

		return false;
	}
	bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.GetKeyCode()] = false;

		return false;
	}
	bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		int keyCode = e.GetKeyCode();
		if (keyCode > 0 && keyCode < 0x10000)
			io.AddInputCharacter((unsigned short)keyCode);

		return false;
	}
	bool ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(e.GetWidth(), e.GetHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		//temp
		glViewport(0, 0, e.GetWidth(), e.GetHeight());

		return false;
	}
}