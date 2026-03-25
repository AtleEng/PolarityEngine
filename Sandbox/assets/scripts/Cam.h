#pragma once

#include <Polarity.h>

using namespace Polarity;


	class CamControll : public ScriptableEntity
	{
	public:
		float speed = 5.0f;

		void OnCreate()
		{

		}

		void OnDestroy()
		{

		}

		void OnUpdate(Timestep ts)
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
	private:
		float test = 1.0f;
	};
