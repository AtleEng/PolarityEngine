#pragma once

#include <random>

namespace Polarity
{
	class RandomEngine
	{
	public:
		RandomEngine()
			: m_Engine(std::random_device{}()) {}

		RandomEngine(uint32_t seed)
			: m_Engine(seed) {}

		int Int(int min, int max)
		{
			std::uniform_int_distribution<int> dist(min, max);
			return dist(m_Engine);
		}

		float Float()
		{
			std::uniform_real_distribution<float> dist(0.0f, 1.0f);
			return dist(m_Engine);
		}

	private:
		std::mt19937 m_Engine;
	};

	class Random
	{
	public:
		static void Init()
		{
			s_Runtime = RandomEngine();
		}

		static int Int(int min, int max)
		{
			return s_Runtime.Int(min, max);
		}

		static float Float()
		{
			return s_Runtime.Float();
		}

	private:
		static RandomEngine s_Runtime;
	};
}