#include "polpch.h"
#include "Audio.h"

#define MINIAUDIO_IMPLEMENTATION
#include <mini_audio/mini_audio.h>

namespace Polarity
{
	ma_engine Audio::s_engine;

	//=== AudioSource =================================================================================//
	AudioSource::AudioSource(ma_engine *pEngine, const std::string& filePath)
	{
		if (ma_sound_init_from_file(pEngine, filePath.c_str(), 0, nullptr, nullptr, &m_sound) != MA_SUCCESS)
		{
			LOG_ERROR("Failed to load audioSource: %s", filePath.c_str());
		}

		m_loaded = true;
		ma_sound_get_length_in_seconds(&m_sound, &m_totalDuration);
	}
	AudioSource::~AudioSource()
	{
		if (m_loaded)
			ma_sound_uninit(&m_sound);
	}
	void AudioSource::SetGain(float gain)
	{
		m_gain = gain;
		ma_sound_set_volume(&m_sound, gain);
	}
	void AudioSource::SetPitch(float pitch)
	{
		m_pitch = pitch;
		ma_sound_set_pitch(&m_sound, pitch);
	}
	void AudioSource::SetLoop(bool loop)
	{
		m_loop = loop;
		ma_sound_set_looping(&m_sound, loop);
	}
	
	//=== Audio =================================================================================//
	void Audio::Init()
	{
		LOG_INFO("Audio initializing...");

		if (ma_engine_init(nullptr, &s_engine) != MA_SUCCESS)
		{
			LOG_MAJOR_ERROR("Failed to initialize miniaudio engine!");
			return;
		}
	}

	Ref<AudioSource> Audio::Create(const std::string& filePath)
	{
		LOG_INFO("Creating AudioSource: %s", filePath.c_str());
		return CreateRef<AudioSource>(&s_engine ,filePath);
	}

	void Audio::Play(const Ref<AudioSource>& source)
	{
		if (!source || !source->m_loaded)
		{
			LOG_WARN("Null audio reference");
			return;
		}
		ma_sound_start(&source->m_sound);
	}

	void Audio::Stop(const Ref<AudioSource>& source)
	{
		if (!source || !source->m_loaded)
		{
			LOG_WARN("Null audio reference");
			return;
		}
		ma_sound_stop(&source->m_sound);
	}
}