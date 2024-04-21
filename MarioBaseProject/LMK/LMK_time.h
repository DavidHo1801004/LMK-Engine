#ifndef LMK_TIME_H_
#define LMK_TIME_H_

#include "LMK_stdinc.h"

LMK_BEGIN
class Time {
public:
	inline Time() {
		m_lastFrame = std::chrono::system_clock::now();
		m_currFrame = std::chrono::system_clock::now();

		m_deltaTime = m_currFrame - m_lastFrame;
	}

public: // Static Functions
	_NODISCARD inline float DeltaTime() noexcept {
		return m_deltaTime.count();
	}

	inline void UpdateDeltaTime() noexcept {
		m_currFrame = std::chrono::system_clock::now();
		m_deltaTime = m_currFrame - m_lastFrame;
		m_lastFrame = m_currFrame;
	}

private: // Properties
	std::chrono::time_point<std::chrono::system_clock> m_lastFrame;
	std::chrono::time_point<std::chrono::system_clock> m_currFrame;

	std::chrono::duration<float> m_deltaTime;
};
LMK_END

#endif // !LMK_TIME_H_
