#pragma once

#include <Polarity.h>

namespace Polarity {

	enum class UIIcon {
		Logo,
		Settings,
		DotMenu,
		Lock,
		Unlock,
		Plus,
		Minus,

		Count
	};

	struct UIIcons {
		static Ref<Texture2D> s_IconAtlas;

		static void Init();

		// Access an icon texture
		static Ref<SubTexture2D> Get(UIIcon icon);
	};

}
