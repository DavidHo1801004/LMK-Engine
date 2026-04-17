#pragma once
#ifndef _LMK_MATHF_H_
#define _LMK_MATHF_H_

#include "LMK_stdinc.h"

LMK_BEGIN
//
// A math helper class.
//
class Mathf {
public: // Static functions
	//
	// 
	//
	[[nodiscard]] inline static float Clamp(float _value, float _min, float _max) noexcept {
		return LMK_Clamp(_value, _min, _max);
	}

	//
	// 
	//
	[[nodiscard]] inline static float MoveTowards(float _current, float _target, float _maxDelta) noexcept {
		if (std::abs(_target - _current) <= _maxDelta)
			return _target;
		if (_current < _target)
			return _current + _maxDelta;
		else
			return _current - _maxDelta;
	}
};
LMK_END

#endif // !_LMK_MATHF_H_
