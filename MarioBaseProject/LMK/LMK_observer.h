#pragma once
#ifndef _LMK_OBSERVER_H_
#define _LMK_OBSERVER_H_

#include "LMK_stdinc.h"

LMK_BEGIN
//
// 
//
template <typename ...Args>
class Action {
private: // Typedef
	using function_type = std::function<void(Args...)>;

public: // Constructors & Destructors
	inline Action() = default;

public: // Operators Overloads
	inline void operator+=(function_type _func) {
		_func.
	}

public: // Functions
	//
	// Invoke all attached functions of this Action.
	// 
	inline void Invoke(Args... args) const noexcept {
		if (m_listeners.empty()) return;

		for (function_type func : m_listeners) {
			func ? func(args...);
		}
	}

private:
	

private: // Properties
	std::vector<function_type> m_listeners;
};
LMK_END

#endif // !_LMK_OBSERVER_H_ 