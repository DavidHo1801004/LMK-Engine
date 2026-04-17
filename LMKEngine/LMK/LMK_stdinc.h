// 
// \file LMK_stdinc.h
// 
// This is the standard include header for LMK Engine.
// 

#pragma once
#ifndef _LMK_STDINC_H_
#define _LMK_STDINC_H_

#include "LMK_root.h"

// +--------------------------------------------------------------------------------+
// |																				|
// | STANDARD LIBRARIES INCLUSION													|
// |																				|
// +--------------------------------------------------------------------------------+

#include <stdexcept>
#include <cassert>

#include <cstdint>
#include <iomanip>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <sstream>

#include <cstring>
#include <string>

#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include <chrono>
#include <atomic>

#include <memory>
#include <thread>
#include <functional>
#include <algorithm>
#include <typeindex>

#define _USE_MATH_DEFINES
#include <cmath>

#if HAVE_CPP14	// C++14
	// For experimental libraries that got implemented in C++17
	#if !HAVE_CPP17
		#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
		#include <experimental/filesystem>
		#undef _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
		namespace fsl = std::experimental::filesystem::v1;
	#endif
#endif

#if HAVE_CPP17	// C++17
	#include <optional>

	#include <filesystem>
	namespace fsl = std::filesystem;
#endif	

#if HAVE_CPP20 	// C++20
	#include <format>
	#include <coroutine>
#endif

#if HAVE_CPP23 	// C++23
	//
	// This is currently left blank for future implementations.
	//
#endif

// +--------------------------------------------------------------------------------+
// |																				|
// | VENDOR LIBRARIES INCLUSION														|
// |																				|
// +--------------------------------------------------------------------------------+

//
// SDL extensions.
//
#if LMK_HAVE_SDL
	#include <SDL.h>
	#pragma comment(lib, "SDL2.lib")
	#pragma comment(lib, "SDL2main.lib")
	#if LMK_HAVE_SDL_IMAGE
		#include <SDL_image.h>
		#pragma comment(lib, "SDL2_image.lib")
	#endif
	#if LMK_HAVE_SDL_TTF
		#include <SDL_ttf.h>
		#pragma comment(lib, "SDL2_ttf.lib")
	#endif
	#if LMK_HAVE_SDL_MIXER
		#include <SDL_mixer.h>
		#pragma comment(lib, "SDL2_mixer.lib")
	#endif
#endif

// +--------------------------------------------------------------------------------+
// |																				|
// | COMPILER INDEPENDENT MACROS													|
// |																				|
// +--------------------------------------------------------------------------------+

#ifndef M_PI
	#define M_PI	3.14159265358979323846264338327950288
#endif

#ifndef LMK_Min
	// Get the minimum value between _a and _b.
	#define LMK_Min(_a, _b)		((_a < _b) ? (_a) : (_b))
#endif

#ifndef LMK_Max
	// Get the maximum value between _a and _b.
	#define LMK_Max(_a, _b)		((_a > _b) ? (_a) : (_b))
#endif

#ifndef LMK_Clamp
	// Clamp value to range.
	#define LMK_Clamp(_val, _min, _max)		LMK_Min(LMK_Max(_val, _min), _max)
#endif

#ifndef LMK_DtoR
	// Convert degree to radian.
	#define LMK_DtoR(_degree)	(_degree / 180.0f * M_PI)
#endif

#ifndef LMK_RtoD
	// Convert radian to degree.
	#define LMK_RtoD(_radian)	(_radian / M_PI * 180.0f)
#endif

#ifndef LMK_InRange
	// Check if value is in range.
	#define LMK_InRange(_val, _min, _max)	((_val > _min) && (_val < _max))
#endif

#ifndef LMK_Lerp
	// Linearly interpolates between _a and _b by _t.
	#define LMK_Lerp(_a, _b, _t)	(_a + ((_b - _a) * _t))
#endif

#ifndef LMK_ArraySize
	// The number of elements in an array.
	#define LMK_ArraySize(_array)	(sizeof(_array) / sizeof(_array[0]))
#endif

#ifndef LMK_RoundToInt
	// Round a floating point value to integer value.
	#define LMK_RoundToInt(f)	((int)(f >= 0.0 ? (f + 0.5) : (f - 0.5)))
#endif

#ifndef LMK_CORE_ASSERT
	// Core assert macro for LMK Engine.
	#define LMK_CORE_ASSERT(condition, message) \
		if (!(condition)) {		\
			assert(message);	\
			abort();			\
		}
#endif

#endif // !_LMK_STDINC_H_