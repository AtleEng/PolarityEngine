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
		Folder,
		File,

		Count
	};

	struct UIIcons {
		static Ref<Texture2D> s_IconAtlas;

		static void Init();

		// Get a ref to an icon texture for atlas
		static Ref<SubTexture2D> Get(UIIcon icon);
	};

}
