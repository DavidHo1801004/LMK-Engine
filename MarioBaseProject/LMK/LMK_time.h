#ifndef LMK_TIME_H_
#define LMK_TIME_H_

#include "LMK_stdinc.h"

LMK_BEGIN
class Time {
public:
	inline Time() {
		lastFrame = currFrame;
	}

public:
	_NODISCARD inline float DeltaTime()  {

	}

private:
	std::chrono::time_point<std::chrono::system_clock> lastFrame;
	std::chrono::time_point<std::chrono::system_clock> currFrame;
};
LMK_END

#endif // !LMK_TIME_H_
