#include "polpch.h"
#include "Cam.h"


	void CamControll::OnCreate()
	{
		POL_INFO("%s Created!!!", GetComponent<NameComponent>().Name.c_str());
	}

	void CamControll::OnDestroy()
	{
		POL_INFO("%s Destroyed!!!", GetComponent<NameComponent>().Name.c_str());
	}

	void CamControll::OnUpdate(Timestep ts)
	{

		auto& position = GetComponent<TransformComponent>().Position;

		if (Input::IsKeyPressed(Key::A))
			position.x -= speed * ts;
		if (Input::IsKeyPressed(Key::D))
			position.x += speed * ts;
		if (Input::IsKeyPressed(Key::W))
			position.y += speed * ts;
		if (Input::IsKeyPressed(Key::S))
			position.y -= speed * ts;
	}

	// Factory function
	extern "C"
	{
		POL_EXPORT_FN ScriptableEntity* CreateScript()
		{
			return new CamControll();
		}

		POL_EXPORT_FN void DestroyScript(ScriptableEntity* ptr)
		{
			delete ptr;
		}
	}

