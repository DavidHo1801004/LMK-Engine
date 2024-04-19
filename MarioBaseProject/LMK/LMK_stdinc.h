// 
// \file LMK_stdinc.h
// 
// This is the standard include header for LMK Engine.
// 

#ifndef LMK_STDINC_H_
#define LMK_STDINC_H_

#include "LMK_root.h"

// +--------------------------------------------------------------------------------+
// | STANDARD LIBRARIES INCLUSION													|
// +--------------------------------------------------------------------------------+

#include <cstdint>
#include <typeinfo>
#include <stdexcept>

#include <cstdio>
#include <iostream>
#include <fstream>
#include <streambuf>
#include <sstream>

#include <cstring>
#include <string>

#include <vector>
#include <list>
#include <array>
#include <map>

#include <chrono>
#include <atomic>

#include <thread>
#include <functional>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <cmath>

#if HAVE_CPP14	// C++14
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
// | VENDOR LIBRARIES INCLUSION														|
// +--------------------------------------------------------------------------------+

//
// SDL extensions.
//
#if LMK_HAVE_SDL
	#include <SDL.h>
	#if LMK_HAVE_SDL_IMAGE
		#include <SDL_image.h>
	#endif
	#if LMK_HAVE_SDL_TTF
		#include <SDL_ttf.h>
	#endif
#endif

// +--------------------------------------------------------------------------------+
// | STANDARD VERSION INDEPENDENT MACROS											|
// +--------------------------------------------------------------------------------+

#ifndef M_PI
#define M_PI	3.14159265358979323846
#endif

#ifndef min
#define min(_a, _b)		(((_a) < (_b)) ? (_a) : (_b))
#endif

#ifndef max
#define max(_a, _b)		(((_a) > (_b)) ? (_a) : (_b))
#endif

#ifndef LMK_Clamp
// Clamp value to range.
#define LMK_Clamp(_val, _min, _max)		min(max((_val), (_min)), (_max))
#endif

#ifndef LMK_DtoR
// Convert degree to radian.
#define LMK_DtoR(_degree)	_degree / 180 * M_PI
#endif

#ifndef LMK_RtoD
// Convert radian to degree.
#define LMK_RtoD(_radian)	_radian / M_PI * 180
#endif

#ifndef LMK_InRange
// Check if value is in range.
#define LMK_InRange(_val, _min, _max)	(((_val) > (_min)) && ((_val) < (_max)))
#endif

#ifndef LMK_ArraySize
// The number of elements in an array.
#define LMK_ArraySize(_array)	(sizeof(_array) / sizeof(_array[0]))
#endif

#endif // !LMK_STDINC_H_