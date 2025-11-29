#pragma once

#include "engine/core/Log.h"

#include <mini_audio/mini_audio.h>

namespace Polarity {

	class AudioSource
	{
	public:
		AudioSource(ma_engine* pEngine, const std::string& filePath);
		~AudioSource();

		bool IsLoaded() const { return m_loaded; }

		void SetGain(float gain);
		void SetPitch(float pitch);
		void SetLoop(bool loop);
	private:
		AudioSource() = default;

		ma_sound m_sound{};
		bool m_loaded = false;
		
		float m_totalDuration = 0; //seconds

		float m_gain = 1.0f;
		float m_pitch = 1.0f;
		bool m_loop = false;

		friend class Audio;
	};
	class Audio
	{
	public:
		static void Init();

		static Ref<AudioSource> Create(const std::string& filePath);

		static void Play(const Ref<AudioSource>& source);
		static void Stop(const Ref<AudioSource>& source);
	private:
		static ma_engine s_engine;
	};
}