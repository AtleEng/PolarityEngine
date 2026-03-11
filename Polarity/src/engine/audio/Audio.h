#pragma once

#include "engine/core/Log.h"

#include <mini_audio/mini_audio.h>

namespace Polarity {

	class AudioSource
	{
	public:
		AudioSource(ma_engine* pEngine, const std::string& filePath);
		~AudioSource();

		const std::string& GetPath() const { return m_Path; }
		bool IsLoaded() const { return m_loaded; }

	private:
		AudioSource() = default;

		std::string m_Path;
		bool m_loaded = false;
		
		ma_sound m_sound{};
		float m_totalDuration = 0; //seconds

		friend class Audio;
	};

	class Audio
	{
	public:
		static void Init();

		static Ref<AudioSource> Create(const std::string& filePath);

		// Play
		static void Play(const Ref<AudioSource>& source, float gain = 1.0f, float pitch = 1.0f, bool loop = false);

		//Stop
		static void Stop(const Ref<AudioSource>& source);

		// Master volume
		static void  SetMasterVolume(float volume);
		static float GetMasterVolume();
		static void  MuteMasterVolume(bool enable);
	private:
		static ma_engine s_engine;

		static float s_MasterVolume;
		static bool s_IsMuted;
	};
}