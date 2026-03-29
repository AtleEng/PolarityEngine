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
		//POL_INFO("%s Update!!!", GetComponent<NameComponent>().Name.c_str());
		auto& position = GetComponent<TransformComponent>().Position;
		//position.x -= speed * ts;

		if (m_Input->IsKeyDown(Key::A))
			position.x -= speed * ts;
		if (m_Input->IsKeyDown(Key::D))
			position.x += speed * ts;
		if (m_Input->IsKeyDown(Key::W))
			position.y += speed * ts;
		if (m_Input->IsKeyDown(Key::S))
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

