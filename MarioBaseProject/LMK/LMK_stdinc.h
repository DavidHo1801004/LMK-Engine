// 
// \file LMK_stdinc.h
// 
// This is the standard include header for LMK Engine.
// 

#ifndef LMK_STDINC_H_
#define LMK_STDINC_H_

#include "LMK_root.h"

// +--------------------------------------------------------------------------------+
// | LIBRARY HEADER INCLUSION														|
// +--------------------------------------------------------------------------------+

#if HAVE_CPP14	// C++14
	#include <windows.h>

	#include <cstdio>
	#include <cstdint>
	#include <cstring>
	#include <cmath>

	#include <iostream>
	#include <fstream>
	#include <streambuf>
	#include <sstream>

	#include <chrono>
	#include <atomic>

	#include <typeinfo>
	#include <string>
	#include <vector>
	#include <list>
	#include <array>
	#include <map>

	#include <thread>

	#include <functional>
	#include <algorithm>

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

//
// Includes SDL library extensions.
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