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

	std::array<Ref<SubTexture2D>, 5> m_spritemap;
	Ref<Texture2D> m_atlasTex;
	Ref<Texture2D> m_gridTex;

	Ref<AudioSource> m_clickSound;

	struct Tile
	{
		Ref<SubTexture2D> sprite;
	};
	Grid<Tile, 16> m_worldGrid;

	bool isDebugging = false;
};

