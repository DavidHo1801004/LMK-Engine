#pragma once
#ifndef _LMK_DELEGATE_H_
#define _LMK_DELEGATE_H_

#include "LMK_stdinc.h"

LMK_BEGIN
//
// Custom class for C# style delegates.
//
template <typename ...Args>
class Action {

	//
	// NOTE:
	// This is just a quick and safe implementation for the sole purpose of finishing the project on time. (no seriously)
	// I am aware of several problems:
	//	1.	A unique id have to be stored for each time the user wants to bind a function to an Action<> 
	//		can impact workflow and memory usage.
	//	2.	The user having to manually uses std::bind() on every Bind() call with member function is not 
	//		a very good design choice.
	// 

public:
	// An unsigned integral type.
	using action_id_type	= uint16_t;

private: // Typedef
	// A function pointer type.
	using function_type = std::function<void(Args...)>;

	// A custom struct for storing a function pointer and its id.
	struct ActionBinder {
		action_id_type id;
		function_type func;
	};

	using storage_type = std::vector<ActionBinder>;

public: // Constructors & Destructors
	inline Action()
		: m_curID(0) {}

public: // Functions
	//
	// Bind a new function pointer to the callback list.
	// 
	// @return
	//		An ID corresponding to the assigned function pointer.
	//
	[[nodiscard]] inline action_id_type Bind(const function_type& _func) {
		m_functions.push_back({ m_curID, _func });
		return m_curID++;
	}

	//
	// Unbind a function pointer from the callback list using its ID.
	//
	inline void Unbind(action_id_type _id) {
		m_functions.erase(std::find_if(m_functions.begin(), m_functions.end(), 
			[_id](const ActionBinder& _binder) {
				return _binder.id == _id;
			}));
	}

	//
	// Invoke all attached function pointers in the callback list.
	// 
	inline void Invoke(Args... args) const noexcept {
		for (auto& binder : m_functions)
			binder.func();
	}

private: // Properties
	storage_type m_functions;
	action_id_type m_curID;
};
LMK_END

#endif // !_LMK_DELEGATE_H_ 