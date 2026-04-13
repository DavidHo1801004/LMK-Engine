#pragma once
#ifndef _LMK_AUDIO_H_
#define _LMK_AUDIO_H_

#include "LMK_stdinc.h"

#if LMK_HAVE_SDL_MIXER
LMK_BEGIN
// +--------------------------------------------------------------------------------+
// |					 															|
// | AUDIO CLIP				 														|
// |					 															|
// +--------------------------------------------------------------------------------+

//
// A container for audio data.
//
class AudioClip {
public: // Typedef
	using target_type = Mix_Chunk;

	friend class AudioManager;

public: // Constructors & Destructors
	//
	// Initialize a new AudioClip given a path to an audio file.
	//
	inline AudioClip(const std::string& _filePath) {
		LoadAudioData(_filePath);
	}

	inline AudioClip(const AudioClip&) = default;

	inline AudioClip() = default;

	inline ~AudioClip() {
		Mix_FreeChunk(m_data);
		m_data = nullptr;
	}

public: // Operators Overloads
	[[nodiscard]] inline operator bool() const {
		return m_data != nullptr;
	}

	[[nodiscard]] inline operator target_type*() const {
		return m_data;
	}

public: // Functions
	//
	// Load audio data to this AudioClip from the given audio file path.
	//
	inline void LoadAudioData(const std::string& _filePath) {
		m_data = Mix_LoadWAV(_filePath.c_str());
		if (!m_data) {
			std::cout << "lmk::AudioClip: Failed to load music. - " << Mix_GetError() << "\n";
		}
	}

private: // Properties
	target_type* m_data = nullptr;
};

// +--------------------------------------------------------------------------------+
// |					 															|
// | AUDIO SOURCE				 													|
// |					 															|
// +--------------------------------------------------------------------------------+

//
// Helper class for playing AudioClip.
//
class AudioManager {
public: // Constructors & Destructors
#pragma region Singleton
	AudioManager(const AudioManager&) = delete;

private:
	inline AudioManager() {
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
			std::cout << "lmk::AudioManager: Mixer could not initialize. - " << Mix_GetError() << "\n";
		}
		ExtendChannels(32); // Initialize the AudioManager with 32 audio channels.
	}
#pragma endregion

private: // Functions
	//
	// Play an AudioClip.
	// 
	// @param _clip:
	//		The AudioClip to play.
	// @param _loops:
	//		How many times to play.
	//		A value of -1 will loop the audio forever.
	// @param _volumeScale:
	//		The scale of the volume. 
	//		Values will automatically be clamped to [0..1].
	// 
	// @return
	//		If the AudioClip is valid, return the index of the channel this audio clip is playing on.
	//		Otherwise, return -1.
	// 
	inline int Play(const AudioClip& _clip, int _loops, float _volumeScale) {
		if (!_clip) return -1;

		int channel = GetFreeChannel();
		if (channel < 0) {
			// To avoid overheads of Mix_AllocateChannels, we increase the extends the channels by 16 each time.
			ExtendChannels(16);
			channel = GetFreeChannel();
		}

		Mix_Volume(channel, _volumeScale * 128);
		Mix_PlayChannel(channel, _clip, _loops);

		return channel;
	}

	[[nodiscard]] inline int GetFreeChannel() {
		for (size_t i = 0; i < m_channels; i++) {
			if (!Mix_Playing(i)) return i;
		}
		return -1;
	}

	inline void ExtendChannels(size_t _channels) {
		m_channels = Mix_AllocateChannels(m_channels + _channels);
	}

public: // Static Functions
	//
	// Plays an AudioClip, and scales the volume by _volumeScale.
	// 
	// @param _clip:
	//		The clip being played.
	// @param _volumeScale:
	//		The scale of the volume. 
	//		Values will automatically be clamped to [0..1].
	//
	inline static void PlayOneShot(const AudioClip& _clip, float _volumeScale = 1.0f) {
		Instance.Play(_clip, 0, LMK_Clamp(_volumeScale, 0, 1));
	}

	//
	// Loop an AudioClip until StopLoopedAudio() is called.
	// 
	// @param _clip:
	//		The clip being played.
	// @param _volumeScale:
	//		The scale of the volume. 
	//		Values will automatically be clamped to [0..1].
	//
	inline static void PlayLooped(const AudioClip& _clip, float _volumeScale = 1.0f) {
		Instance.m_loopChannels.push_back(Instance.Play(_clip, -1, LMK_Clamp(_volumeScale, 0, 1)));
	}

	//
	// Stop all AudioClip played with PlayLooped().
	//
	inline static void StopLoopedAudio() {
		for (int channel : Instance.m_loopChannels) {
			Mix_HaltChannel(channel);
		}
		Instance.m_loopChannels.clear();
	}

	//
	// Stop all playing AudioClip.
	//
	inline static void StopAll() {
		Mix_HaltChannel(-1);
		Instance.m_loopChannels.clear();
	}

private: // Static Properties 
	static AudioManager Instance;

private: // Properties
	size_t m_channels = 0;

	std::vector<int> m_loopChannels;
};

AudioManager AudioManager::Instance;
LMK_END
#endif // !LMK_HAVE_SDL_MIXER

#endif // !_LMK_AUDIO_H_
