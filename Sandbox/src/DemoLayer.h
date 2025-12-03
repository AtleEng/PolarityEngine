#pragma once
#include <Polarity.h>
#include <array>

#include "CamController.h"
#include "ParticleSystem.h"
#include "Grid.h"

using namespace Polarity;

class DemoLayer : public Layer
{
public:
	DemoLayer();
	virtual ~DemoLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(Timestep tS) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Event& event) override;

	bool OnKeyPressedEvent(KeyPressedEvent& event);

private:
	CamController  m_cameraController;
	ShaderLibrary  m_shaderLibrary;

	ParticleSystem m_particleSystem;
	ParticleProps m_particle;

	glm::vec3 m_pos		 = glm::vec3(0.0f);
	glm::vec2 m_size	 = glm::vec2(1.0f);
	glm::vec4 m_color	 = glm::vec4(0.8f, 0.2f, 0.3f, 1.0f);
	float	  m_rotation = 0;

	std::array<Ref<SubTexture2D>, 5> m_spritemap;
	Ref<Texture2D> m_atlasTex;
	Ref<Texture2D> m_gridTex;
	Ref<Texture2D> m_logoTex;

	Ref<AudioSource> m_clickSound;

	struct Tile
	{
		Ref<SubTexture2D> sprite;
	};
	Grid<Tile, 16> m_worldGrid;

	bool isDebugging = false;
};

