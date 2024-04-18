//
// \file LMK_root.h
//
// The root header for the LMK extension.
// This header tries to get a standard set of platform, compiler defines
// and defines standard configurations for LMK features.
//

#ifndef LMK_ROOT_H_
#define LMK_ROOT_H_

// +--------------------------------------------------------------------------------+
// | LANGUAGE VERSION SPECIFIC DEFINES												|
// +--------------------------------------------------------------------------------+

//
// We define the C++ language standard to the larger of _MSVC_LANG and __cplusplus
// since Visual Studio doesn't change the value of the macro __cplusplus beyond 199711L.
// 
// For documentation regarding changes for __cplusplus macro with Visual Studio, see:
// https://learn.microsoft.com/en-us/cpp/build/reference/zc-cplusplus?view=msvc-140
// 
// For documentation regarding _MSVC_LANG macro, see:
// https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170
//
#if defined(__cplusplus)
	#if defined(_MSVC_LANG) && (_MSVC_LANG > __cplusplus)
		#define CPP_VER _MSVC_LANG
	#else
		#define CPP_VER __cplusplus
	#endif
#else	
	// If no C++ supports available.
	#error LMK Engine: Missing C++ stadard support.
#endif

//
// The default C++ language standard targeted by most compilers is C++14 (CPP_VER = 201402L).
// 
// However, in case this program was compiled by an older C++ compiler standard:
// (see https://github.com/cpredef/predef/blob/master/Standards.md)
//
#ifndef HAVE_CPP14
	#if CPP_VER > 201103L
		#define HAVE_CPP14 1
	#else
		#define HAVE_CPP14 0
		#error	LMK Engine: C++14 or better is required.
	#endif
#endif

#ifndef HAVE_CPP17
	#if HAVE_CPP14 && CPP_VER > 201402L
		#define HAVE_CPP17 1
	#else
		#define HAVE_CPP17 0
	#endif
#endif

#ifndef HAVE_CPP20
	#if HAVE_CPP17 && CPP_VER > 201703L
		#define HAVE_CPP20 1
	#else
		#define HAVE_CPP20 0
	#endif
#endif

#ifndef HAVE_CPP23
	#if HAVE_CPP20 && CPP_VER > 202002L
		#define HAVE_CPP23 1
	#else
		#define HAVE_CPP23 0
	#endif
#endif

// +--------------------------------------------------------------------------------+
// | SDL RELATED DEFINES															|
// +--------------------------------------------------------------------------------+

//
// Some LMK features does not requires SDL to works properly.
//
// We use this macro to control which LMK features are functional 
// according to SDL availability.
//
#if defined(__has_include) && __has_include(<SDL.h>)
	#define LMK_HAVE_SDL	1
	// Check for SDL extensions
	#if __has_include(<SDL_image.h>)
		#define LMK_HAVE_SDL_IMAGE	1
	#else
		#define LMK_HAVE_SDL_IMAGE	0
	#endif
	#if __has_include(<SDL_ttf.h>)
		#define LMK_HAVE_SDL_TTF	1
	#else
		#define LMK_HAVE_SDL_TTF	0
	#endif
#else
	#define LMK_HAVE_SDL	0
#endif

// +--------------------------------------------------------------------------------+
// | LMK ENGINE SPECIFIC MACROS														|
// +--------------------------------------------------------------------------------+

//
// Macro for encapsulating LMK features to the default lmk:: namespace.
//
#define LMK_BEGIN	namespace lmk {
#define LMK_END		}

//
// Macro for encapsulating implemental LMK features to the default ::impl:: namespace.
// This is used to hide features that should not be manually called by the user.
//
#define IMPL_BEGIN	namespace impl {
#define IMPL_END	}

#endif // !LMK_ROOT_H_