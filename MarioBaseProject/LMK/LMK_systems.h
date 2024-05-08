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
public: // Typedef
	using time_point_t	= std::chrono::time_point<std::chrono::system_clock>;
	using time_percis_t = float;
	using duration_t	= std::chrono::duration<time_percis_t>;

	friend class LMKEngine;

public: // Constructors & Destructors
#pragma region Singleton
	Time(const Time&) = delete;

private:
#pragma warning (disable : 26495)
	inline Time() {
		m_lastFrame = std::chrono::system_clock::now();
		m_currFrame = std::chrono::system_clock::now();
	}
#pragma warning (default : 26495)

public:
#pragma endregion

public: // Static Functions
	//
	// The interval in seconds from the last frame to the current one.
	//
	_NODISCARD inline static float DeltaTime() noexcept {
		return Instance.m_deltaTime;
	}

	//
	// The average number of frames updated per second based on process time.
	//
	_NODISCARD inline static float FrameRate() noexcept {
		return 1.0f / Instance.m_deltaTime;
	}

private:
#pragma warning (disable : 4244)
	//
	// This should only be called by the LMK Engine within the main Update() loop.
	//
	inline static void UpdateDeltaTime() noexcept {
		Instance.m_currFrame = std::chrono::system_clock::now();
		Instance.m_deltaDuration = Instance.m_currFrame - Instance.m_lastFrame;
		Instance.m_lastFrame = Instance.m_currFrame;

		Instance.m_deltaTime = Instance.m_deltaDuration.count();
	}
#pragma warning (default : 4244)

public: // Properties
	static float timeScale;

private:
	static Time Instance;

	time_point_t	m_lastFrame;
	time_point_t	m_currFrame;
	duration_t		m_deltaDuration;
	time_percis_t	m_deltaTime;
};

Time Time::Instance;
LMK_END

#endif // !_LMK_SYSTEMS_H_