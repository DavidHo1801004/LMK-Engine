//
//+--------------------------------------- LMK Engine ---------------------------------------+
//
//	Copyright (C) 2024 Ho MInh Tri (a.k.a LazyMonkey73)
//
//	Redistribution and use in source and binary forms, with or without modification,
//	are permitted provided that the following conditions are met:
//
//		1. Redistributions or derivations of source code must retain the above copyright
//		notice, this list of conditions and the following disclaimer.
//
//		2. Redistributions or derivative works in binary form must reproduce the above
//		copyright notice.This list of conditions and the following disclaimer must be
//		reproduced in the documentation and/or other materials provided with the distribution.
//
//		3. Neither the name of the copyright holder nor the names of its contributors may
//		be used to endorse or promote products derived from this software without specific
//		prior written permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS	"AS IS" AND ANY
//	EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
//	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
//	SHALL THE COPYRIGHT	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED
//	TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
//	BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//	CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//	ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
//	SUCH DAMAGE.
//
//+-----------------------------------------------------------------------------------------------+
//

//
// \file LMK.h
//
// Main include header for the LMK Engine library.
//

#pragma once
#ifndef LMK_H_
#define LMK_H_

//
//+---------------------------------- LMK Engine library Manual ----------------------------------+
// 
// User should NOT manually include any individual headers from the LMK library 
// except for this header.
// 
// To specify which additional features of LMK Engine you wish to enable, please define 
// the following macro(s) before including this header file:
// 
//	LMK_INCLUDE_ALL		- All LMK Engine features
//	LMK_INCLUDE_SOUND	- 
// 
// Example code:
// 
//	#define LMK_INCLUDE_DEBUG
//	#define LMK_INCLUDE_2D_UTIL
//	#include "LMK.h"
// 
//	int main() {
//		// Client code...
//	}
//
#if defined(LMK_INCLUDE_ALL)
	#define LMK_INCLUDE_SOUND	
	#define LMK_INCLUDE_GIZMOS
	//
	// This is reserved for future defines
	//
#endif

//
// The core headers that implement basic classes required for LMK Engine to function.
//
#include "LMK_coremdl.h"
#include "LMK_render.h"
#include "LMK_engine.h"

#if defined(LMK_INCLUDE_SOUND)
	
#endif
#if defined(LMK_INCLUDE_GIZMOS)
	#include "LMK_gizmo.h"
#endif

#endif // !LMK_H_