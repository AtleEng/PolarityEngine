#pragma once
#include "engine/core/Log.h"

#include <chrono>

namespace Polarity
{
	struct ProfileResult
	{
		std::string Name;

		std::chrono::microseconds ElapsedTime;
	};

	template<typename Fn>
	class InstrumentationTimer
	{
	public:
		InstrumentationTimer(const char* name, Fn&& func)
			: m_name(name), m_func(func), m_stopped(false)
		{
			m_startTimepoint = std::chrono::steady_clock::now();
		}

		~InstrumentationTimer()
		{
			if (!m_stopped)
				Stop();
		}

		void Stop()
		{
			auto endTimepoint = std::chrono::steady_clock::now();

			long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch().count();
			long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

			m_stopped = true;

			float duration = (end - start) * 0.001f;
			m_func({ m_name, duration });
		}
	private:
		const char* m_name;
		Fn m_func;
		std::chrono::time_point<std::chrono::steady_clock> m_startTimepoint;
		bool m_stopped;
	};

}