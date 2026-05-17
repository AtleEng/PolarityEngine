#include "polpch.h"
#include "Cam.h"


	void Cam::OnCreate()
	{
		POL_INFO("%s Created!!!", GetComponent<NameComponent>().Name.c_str());
	}

	void Cam::OnDestroy()
	{
		POL_INFO("%s Destroyed!!!", GetComponent<NameComponent>().Name.c_str());
	}

	void Cam::OnUpdate(Timestep ts)
	{
		auto& position = GetComponent<TransformComponent>().Position;

		if (m_Input->IsKeyDown(Key::A))
			position.x -= speed * ts;
		if (m_Input->IsKeyDown(Key::D))
			position.x += speed * ts;
		if (m_Input->IsKeyDown(Key::W))
			position.y += speed * ts;
		if (m_Input->IsKeyDown(Key::S))
			position.y -= speed * ts;
	}

