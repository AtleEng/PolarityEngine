#include "polpch.h"
#include "Audio.h"

#define MINIAUDIO_IMPLEMENTATION
#include <mini_audio/mini_audio.h>

namespace Polarity
{
	ma_engine Audio::s_engine;
	float Audio::s_MasterVolume = 1;
	bool Audio::s_IsMuted = false;

	//=== AudioSource =================================================================================//
	AudioSource::AudioSource(ma_engine *pEngine, const std::string& filePath)
	{
		POL_PROFILE_FUNCTION();

		if (ma_sound_init_from_file(pEngine, filePath.c_str(), 0, nullptr, nullptr, &m_sound) != MA_SUCCESS)
		{
			POL_CORE_ERROR("Audio: Failed to load AudioSource '%s'!", filePath.c_str());
			m_loaded = false;
			return;
		}
		m_Path = filePath;
		m_loaded = true;

		ma_sound_get_length_in_seconds(&m_sound, &m_totalDuration);
	}
	AudioSource::~AudioSource()
	{
		POL_PROFILE_FUNCTION();

		if (m_loaded)
			ma_sound_uninit(&m_sound);
	}
	
	//=== Audio =================================================================================//
	void Audio::Init()
	{
		POL_PROFILE_FUNCTION();

		POL_CORE_INFO("Audio: initializing...");

		if (ma_engine_init(nullptr, &s_engine) != MA_SUCCESS)
		{
			POL_CORE_FATAL("Audio: Failed to initialize miniaudio engine!");
			return;
		}
	}

	Ref<AudioSource> Audio::Create(const std::string& filePath)
	{
		POL_PROFILE_FUNCTION();

		return CreateRef<AudioSource>(&s_engine ,filePath);
	}

	void Audio::SetMasterVolume(float volume)
	{
		s_MasterVolume = volume;
		ma_engine_set_volume(&s_engine, volume);
	}

	float Audio::GetMasterVolume()
	{
		return s_MasterVolume;
	}

	void Audio::MuteMasterVolume(bool enable)
	{
		s_IsMuted = enable;
		ma_engine_set_volume(&s_engine, enable ? 0.0f : s_MasterVolume);
	}

	void Audio::Play(const Ref<AudioSource>& source, float gain, float pitch, bool loop)
	{
		POL_PROFILE_FUNCTION();

		if (!source || !source->m_loaded)
		{
			POL_CORE_WARN("Audio: AudioSource is null");
			return;
		}
		ma_sound_set_volume(&source->m_sound, gain);
		ma_sound_set_pitch(&source->m_sound, pitch);
		ma_sound_set_looping(&source->m_sound, loop);

		ma_sound_seek_to_pcm_frame(&source->m_sound, 0);
		ma_sound_start(&source->m_sound);
	}

	void Audio::Stop(const Ref<AudioSource>& source)
	{
		POL_PROFILE_FUNCTION();

		if (!source || !source->m_loaded)
		{
			POL_CORE_WARN("Audio: null AudioSource reference");
			return;
		}
		ma_sound_stop(&source->m_sound);
	}
}