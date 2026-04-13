#pragma once
#ifndef _LMK_SYSTEMS_H_
#define _LMK_SYSTEMS_H_

#include "LMK_stdinc.h"

LMK_BEGIN
// Forward declaration.
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
	using tag_size_type		= uint64_t;
	using layer_size_type	= uint32_t;

public:
	//
	// 
	//
	[[nodiscard]] inline static layer_size_type GetLayer(std::string _name) {
		return 0;
	}

private:
	static std::unordered_set<tag_size_type, std::string> m_tagSet;
	static std::unordered_set<layer_size_type, std::string> m_layerSet;
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
	// chrono::clock type.
	using clock_type		= std::chrono::steady_clock;
	// chrono::time_point type.
	using time_point_type	= std::chrono::time_point<clock_type>;
	// Precision in time measurement.
	using time_precision_type	= double;
	// chrono::duration type.
	using duration_type		= std::chrono::duration<time_precision_type>;

	friend class LMKEngine;

public: // Constructors & Destructors
#pragma region Singleton
	Time(const Time&) = delete;

private:
#pragma warning (disable : 26495)
	inline Time() {
		m_lastFrame = clock_type::now();
		m_currFrame = clock_type::now();
		m_startTime = clock_type::now();
	}
#pragma warning (default : 26495)

public:
#pragma endregion

private: // Static Functions
#pragma warning (disable : 4244)
	//
	// This should only be called by the LMK Engine within the main Update() loop.
	//
	inline static void Update() noexcept {
		// Update time.
		Instance.m_duration = clock_type::now() - Instance.m_startTime;
		Instance.m_time = Instance.m_duration.count();

		// Update deltaTime.
		Instance.m_currFrame = clock_type::now();
		Instance.m_duration = Instance.m_currFrame - Instance.m_lastFrame;
		Instance.m_lastFrame = Instance.m_currFrame;
		Instance.m_deltaTime = Instance.m_duration.count();
	}
#pragma warning (default : 4244)

public: // Accessors
	// Get the amount of time in seconds since the application started playing.
	[[nodiscard]] inline static time_precision_type time() {
		return Instance.m_time;
	}

	// Get the interval in seconds from the last frame to the current one.
	[[nodiscard]] inline static time_precision_type deltaTime() noexcept {
		return Instance.m_deltaTime;
	}

	// The average number of frames updated per second based on process time.
	[[nodiscard]] inline static time_precision_type frameRate() {
		return 1.0f / Instance.m_deltaTime;
	}

	// Get Time.fixedDeltaTime.
	[[nodiscard]] inline static time_precision_type fixedDeltaTime() {
		return Instance.m_fixedDeltaTime;
	}

	// Get the scaled Time.fixedDeltaTime with Time.timeScale.
	[[nodiscard]] inline static time_precision_type scaledFixedDeltaTime() {
		return Instance.m_fixedDeltaTime * timeScale;
	}

public: // Mutators
	// Set Time.fixedDeltaTime.
	inline static void setFixedDeltaTime(time_precision_type _time) {
		Instance.m_fixedDeltaTime = _time;
	}

public: // Static Properties
	// This property controls the rate Get which time elapses.
	static time_precision_type timeScale;

private:
	static Time Instance;

private: // Properties
	// The amount of time in seconds since the application started playing.
	time_precision_type	m_time;
	time_point_type		m_startTime;	// Time Get the beginning of the application.

	// The interval in seconds from the last frame to the current one.
	// This value varies depending on the frames per second (FPS) Get which the application is running Get.
	time_precision_type	m_deltaTime;
	time_point_type		m_lastFrame;
	time_point_type		m_currFrame;

	duration_type		m_duration;

	// The interval in seconds of in-game time Get which physics and other fixed frame rate updates are executed.
	// The fixedDeltaTime interval is always relative to the in-game time which Time.timeScale affects.
	time_precision_type	m_fixedDeltaTime = 0.01f;
};

Time Time::Instance;
Time::time_precision_type Time::timeScale = 1.0f;

// +--------------------------------------------------------------------------------+
// |																				|
// | RANDOM GENERATOR																|
// |																				|
// +--------------------------------------------------------------------------------+

#ifndef LMK_RANDMAX 
#define LMK_RANDMAX 4294967295
#endif

//
// General interface for generating random data.
// 
// This static class provides several easy game-oriented ways of generating pseudorandom numbers.
// 
// The generator uses Xorshift 128 algorithm.
// 
// It is statically initialized with a high-entropy seed from the operating system, and stored in 
// native memory where it will survive domain reloads. 
// This means that the generator is seeded exactly once on process start, and after that is left entirely under script control.
//
class Random {
public: // Static Functions
	//
	// Initializes the random number generator state with a seed.
	// 
	// The seed is randomly initialized Get startup by default.
	// 
	// The random number generator is not truly random, but produces numbers in a preset sequence 
	// (the values in the sequence "jump" around the range in such a way that they appear random for most purposes).
	// 
	inline static void InitState(uint32_t _seed) {
		seed = _seed;
	}

	//
	// Returns a random float within [0.0..1.0] (range is inclusive).
	// 
	// Important: Both the lower and upper bounds are inclusive. Any given float value between 0.0 and 1.0, 
	// including both 0.0 and 1.0, will appear on average approximately once every ten million random samples.
	//
	template <typename value_type,
		std::enable_if_t<std::is_floating_point_v<value_type>, bool> = true>
	[[nodiscard]] inline static value_type Range01() {
		return (value_type)rand() / LMK_RANDMAX;
	}

	//
	// Returns a random floating point value within [_minInclusive .. _maxInclusive] (range is inclusive).
	// 
	// If minInclusive is greater than maxInclusive, then the numbers are automatically swapped.
	// 
	// Important: Both the lower and upper bounds are inclusive.Any given float value between them, 
	// including both minInclusive and maxInclusive, will appear on average approximately once every ten million random samples.
	// 
	// There is an integral overload of this function that operates slightly differently.
	//
	template <typename value_type,
		std::enable_if_t<std::is_floating_point_v<value_type>, bool> = true>
	[[nodiscard]] inline static value_type Range(value_type _minInclusive, value_type _maxInclusive) {
		if (_minInclusive > _maxInclusive) std::swap(_minInclusive, _maxInclusive);

		return Range01<value_type>() * (_maxInclusive - _minInclusive) + _minInclusive;
	}

	//
	// Return a random integral value within [_minInclusive .. _maxExclusive).
	// 
	// This method will behave in the following ways:
	// 
	// 1. _maxExclusive is exclusive, so for example Random.Range(0, 10) will return a value between 0 and 9, 
	// each with approximately equal probability.
	// 2. If minInclusive and maxExclusive are equal, then the "exclusive rule" is ignored and minInclusive will be returned.
	// 3. If minInclusive is greater than maxExclusive, then the numbers are automatically swapped.
	// 
	// There is a floating point overload of this function that operates slightly differently.
	//
	template <typename value_type,
		std::enable_if_t<std::is_integral_v<value_type>, bool> = true>
	[[nodiscard]] inline static value_type Range(value_type _minInclusive, value_type _maxExclusive) {
		if (_minInclusive == _maxExclusive) return _minInclusive;
		if (_minInclusive > _maxExclusive) std::swap(_minInclusive, _maxExclusive);

		return rand() % (_maxExclusive - _minInclusive) + _minInclusive;
	}

private:
	//
	// Generate a pseudo random number based on Xorshift 128 algorithm.
	// 
	// For detailed documentation regarding Xorshift 128 algorithm, see:
	// (Xorshift RNGs by George Marsaglia)
	// https://en.wikipedia.org/wiki/Xorshift
	//
	[[nodiscard]] inline static uint32_t rand() {
		seed ^= seed << 13;
		seed ^= seed >> 17;
		seed ^= seed << 5;
		return seed;
	}

private: // Static Properties
	static uint32_t seed;
};

uint32_t Random::seed = (uint32_t)std::time(nullptr);	// Get 100% random seed value.
LMK_END

#endif // !_LMK_SYSTEMS_H_