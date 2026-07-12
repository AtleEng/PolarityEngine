#pragma once

#include <Polarity.h>

namespace Polarity {

	enum class UIIcon {
		Logo,
		Settings,
		Select,
		Play,
		Pause,
		X,
		Warn,

		Folder,
		File,
		Music,
		Image,
		Font,
		Prefab,
		Script,
		Scene,

		ArrowLeft,
		ArrowRight,
		ArrowBack,
		Minus,
		Plus,
		MenuHamburger,
		MenuDot,

		Lock,
		Unlock,
		ViewIcon,
		ViewList,
		Filter,
		Refresh,

		Count
	};

	struct UIIcons {
		static Ref<Texture2D> s_IconAtlas;

		static void Init();

		// Get a ref to an icon texture for atlas
		static Ref<SubTexture2D> Get(UIIcon icon);
	};

}
