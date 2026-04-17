#pragma once
#ifndef _LMK_ANIMATION_H_
#define _LMK_ANIMATION_H_

#include "LMK_stdinc.h"
#include "LMK_coremdl.h"
#include "LMK_render2d.h"
#include "LMK_gameobj.h"

LMK_BEGIN
// +--------------------------------------------------------------------------------+
// |					 															|
// | ANIMATION CLIP				 													|
// |					 															|
// +--------------------------------------------------------------------------------+

//
// 
//
class AnimationClip {
private: // Typedef
	// Wrapper for keyframe properties
	struct KeyFrame {
		size_t frame;
		Sprite* sprite;
	};

public: // Constructors & Destructors
	//
	// Create a new AnimationClip with the given frame rate.
	// 
	// @param _frameRate:
	//		The frame rate at which keyframes are sampled.
	//
	inline AnimationClip(float _frameRate) 
		: m_frameRate(_frameRate) {}

	//
	// Create a default AnimationClip with a frame rate of 24.
	//
	inline AnimationClip()
		: AnimationClip(24.0f) {}

public: // Functions
	//
	// Add a new key frame to the animation clip with the given time and value.
	// 
	// @param _keyTime:
	//		The time to insert the new key frame at.
	// @param _sprite:
	//		The value of the key frame.
	//
	inline void AddKeyFrame(size_t _frame, const Sprite& _sprite) noexcept {
		// If new time is greater than current length.
		if (_frame > m_frameLength) {
			m_frameLength = _frame;
			m_keyFrames.emplace_back(_frame, _sprite);
		}
		// If new time is in range of current length.
		else {
			// Insert frame in the correct position
			for (auto iter = m_keyFrames.begin(); iter != m_keyFrames.end(); iter++) {
				if ((*iter).frame == _frame)
					m_keyFrames.erase(iter);

				if ((*iter).frame >= _frame)
					m_keyFrames.emplace(iter, _frame, _sprite);
			}
		}
	}

	//
	// Get the frame value at _time.
	// 
	// @return 
	//		A Sprite at _time.
	//
	[[nodiscard]] inline Sprite* GetKeyAt(size_t _frame) const noexcept {
		// If _time is greater than the length of the animation -> return 
		if (_frame > m_frameLength) {
			if (loop) {
				_frame = _frame % m_frameLength;
			}
			return m_keyFrames.back().sprite;
		}
		else {
			for (auto iter = m_keyFrames.begin(); iter != m_keyFrames.end(); iter++) {
				if ((*iter).frame > _frame)
					return (*(--iter)).sprite;
			}
		}
	}

public: // Accessors
	// Get the animation length in seconds.
	inline float length() {
		return m_frameLength / m_frameRate;
	}

	// Get the frame rate at which keyframes are sampled.
	inline float frameRate() {
		return m_frameRate;
	}

public: // Properties
	bool loop;

private:
	float m_frameRate;		// Frame rate at which keyframes are sampled.
	size_t m_frameLength;	// Animation length in frames.

	std::vector<KeyFrame> m_keyFrames;
};

// +--------------------------------------------------------------------------------+
// |					 															|
// | ANIMATOR				 														|
// |					 															|
// +--------------------------------------------------------------------------------+

//
// 
//
class Animator : public Behaviour {
private: // Typedef
	friend class LMKEngine;

public: // Functions
	//
	// Play an animation clip.
	//
	inline void Play(const AnimationClip& _clip) {
		m_elapsedTime = 0;
		m_currentClip = _clip;
	}

	//
	// Advance the current AnimationClip by _deltaTime.
	//
	inline void Advance(float _deltaTime) {
		m_elapsedTime += _deltaTime;
		size_t frame = std::round(m_currentClip.frameRate() * m_elapsedTime);
		renderer->SetSprite(*m_currentClip.GetKeyAt(frame));
	}

public: // Properties
	SpriteRenderer* renderer;

private: 
	float m_elapsedTime;
	float m_speedMultiplier;

	AnimationClip m_currentClip;
};
LMK_END

#endif // !_LMK_ANIMATION_H_
