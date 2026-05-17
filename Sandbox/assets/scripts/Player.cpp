#include "polpch.h"
#include "Player.h"


void Player::OnCreate()
{
	POL_INFO("%s Created!!!", GetComponent<NameComponent>().Name.c_str());
}

void Player::OnDestroy()
{
	POL_INFO("%s Destroyed!!!", GetComponent<NameComponent>().Name.c_str());
}

void Player::OnUpdate(Timestep ts)
{
	auto& position = GetComponent<TransformComponent>().Position;

	if (m_Input->IsKeyDown(Key::Left))
		position.x -= speed * ts;
	if (m_Input->IsKeyDown(Key::Right))
		position.x += speed * ts;
	if (m_Input->IsKeyDown(Key::Up))
		position.y += speed * ts;
	if (m_Input->IsKeyDown(Key::Down))
		position.y -= speed * ts;
}