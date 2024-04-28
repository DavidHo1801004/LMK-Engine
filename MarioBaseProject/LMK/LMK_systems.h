#pragma once
#ifndef _LMK_SYSTEMS_H_
#define _LMK_SYSTEMS_H_

#include "LMK_stdinc.h"

LMK_BEGIN
class LMKEngine;

// +--------------------------------------------------------------------------------+
// |																				|
// | TAGS & LAYERS SYSTEM															|
// |																				|
// +--------------------------------------------------------------------------------+

//
// 
//
class TagsLayersSystem {
public: // Typedef
	using tag_size_t = uint64_t;
	using layer_size_t = uint32_t;

	enum LayersID {
		LAYER_0		= 0x00000001,
		LAYER_1		= 0x00000002,
		LAYER_2		= 0x00000004,
		LAYER_3		= 0x00000008,
		LAYER_4		= 0x00000010,
		LAYER_5		= 0x00000020,
		LAYER_6		= 0x00000040,
		LAYER_7		= 0x00000080,
		LAYER_8		= 0x00000100,
		LAYER_9		= 0x00000200,
		LAYER_10	= 0x00000400,
		LAYER_11	= 0x00000800,
		LAYER_12	= 0x00001000,
		LAYER_13	= 0x00002000,
		LAYER_14	= 0x00004000,
		LAYER_15	= 0x00008000,
		LAYER_16	= 0x00010000,
		LAYER_17	= 0x00020000,
		LAYER_18	= 0x00040000,
		LAYER_19	= 0x00080000,
		LAYER_20	= 0x00100000,
		LAYER_21	= 0x00200000,
		LAYER_22	= 0x00400000,
		LAYER_23	= 0x00800000,
		LAYER_24	= 0x01000000,
		LAYER_25	= 0x02000000,
		LAYER_26	= 0x04000000,
		LAYER_27	= 0x08000000,
		LAYER_28	= 0x10000000,
		LAYER_29	= 0x20000000,
		LAYER_30	= 0x40000000,
		LAYER_31	= 0x80000000,
	};

public:
	//
	//
	//
	_NODISCARD inline static layer_size_t GetLayer(std::string _name) {
		return 0;
	}

private:
	static std::unordered_set<tag_size_t, std::string> m_tagSet;
	static std::unordered_set<layer_size_t, std::string> m_layerSet;
};

// +--------------------------------------------------------------------------------+
// |																				|
// | TIME SYSTEM																	|
// |																				|
// +--------------------------------------------------------------------------------+

//
// A static class provides time related functions in the LMK Engine.
//
class Time {
private:
	friend class LMKEngine;

public:
	Time() = delete;	// To avoid initialization instances of this class.

public: // Static Functions
	//
	// The interval in seconds from the last frame to the current one.
	//
	_NODISCARD inline static float DeltaTime() noexcept {
		return m_deltaTime.count() * 10;
	}

	//
	// The average number of frames updated per second based on process time.
	//
	_NODISCARD inline static float FrameRate() noexcept {
		return 1 / m_deltaTime.count() * 10;
	}

private:
	//
	// This should only be called by the LMK Engine within the main Update() loop.
	//
	inline static void UpdateDeltaTime() noexcept {
		m_currFrame = std::chrono::system_clock::now();
		m_deltaTime = m_currFrame - m_lastFrame;
		m_lastFrame = m_currFrame;
	}

public: // Properties
	static float m_timeScale;

private:
	static std::chrono::time_point<std::chrono::system_clock> m_lastFrame;
	static std::chrono::time_point<std::chrono::system_clock> m_currFrame;
	static std::chrono::duration<float> m_deltaTime;
};

std::chrono::time_point<std::chrono::system_clock> Time::m_lastFrame;
std::chrono::time_point<std::chrono::system_clock> Time::m_currFrame;
std::chrono::duration<float> Time::m_deltaTime;
LMK_END

#endif // !_LMK_SYSTEMS_H_