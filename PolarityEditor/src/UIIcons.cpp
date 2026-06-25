#include "UIIcons.h"
#include <array>
#include <cassert>

namespace Polarity {
	Ref<Texture2D> UIIcons::s_IconAtlas = nullptr;

	std::array<Ref<SubTexture2D>, static_cast<size_t>(UIIcon::Count)> s_icons;
	bool s_initialized = false;

	void UIIcons::Init() {
		if (s_initialized)
		{
			POL_ERROR("UIIcons already initialized");
			return;
		}

		s_IconAtlas = Texture2D::Create("assets/textures/icons.png");

		s_icons[static_cast<size_t>(UIIcon::Logo)] =
			SubTexture2D::CreateFromUniformGrid(s_IconAtlas, { 1, 3 }, 18);

		s_icons[static_cast<size_t>(UIIcon::Settings)] =
			SubTexture2D::CreateFromUniformGrid(s_IconAtlas, { 5, 1 }, 18);

		s_icons[static_cast<size_t>(UIIcon::DotMenu)] =
			SubTexture2D::CreateFromUniformGrid(s_IconAtlas, { 4, 1 }, 18);

		s_icons[static_cast<size_t>(UIIcon::Lock)] =
			SubTexture2D::CreateFromUniformGrid(s_IconAtlas, { 0, 0 }, 18);

		s_icons[static_cast<size_t>(UIIcon::Unlock)] =
			SubTexture2D::CreateFromUniformGrid(s_IconAtlas, { 1, 0 }, 18);

		s_icons[static_cast<size_t>(UIIcon::Plus)] =
			SubTexture2D::CreateFromUniformGrid(s_IconAtlas, { 2, 1 }, 18);

		s_icons[static_cast<size_t>(UIIcon::Minus)] =
			SubTexture2D::CreateFromUniformGrid(s_IconAtlas, { 1, 1 }, 18);

		s_icons[static_cast<size_t>(UIIcon::Folder)] =
			SubTexture2D::CreateFromUniformGrid(s_IconAtlas, { 7, 2 }, 18);

		s_icons[static_cast<size_t>(UIIcon::File)] =
			SubTexture2D::CreateFromUniformGrid(s_IconAtlas, { 7, 1 }, 18);

		s_icons[static_cast<size_t>(UIIcon::Play)] =
			SubTexture2D::CreateFromUniformGrid(s_IconAtlas, { 2, 0 }, 18);

		s_icons[static_cast<size_t>(UIIcon::Pause)] =
			SubTexture2D::CreateFromUniformGrid(s_IconAtlas, { 3, 0 }, 18);

		s_icons[static_cast<size_t>(UIIcon::Warn)] =
			SubTexture2D::CreateFromUniformGrid(s_IconAtlas, { 5, 0 }, 18);

		s_icons[static_cast<size_t>(UIIcon::Music)] =
			SubTexture2D::CreateFromUniformGrid(s_IconAtlas, { 6, 1 }, 18);

		s_initialized = true;
	}

	Ref<SubTexture2D> UIIcons::Get(UIIcon icon) 
	{
		if(s_initialized)
			return s_icons[static_cast<size_t>(icon)];

		POL_ERROR("UIIcons used before Init");
		return nullptr;
	}
}