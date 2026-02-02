/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Timer.h"
#include "SoundEffectInstance.h"

#ifdef AUDIO_DUMMY
typedef struct MusicInstance MusicInstance;
#else
typedef struct MusicInstance
{
	int32_t _mWillFadeInTime;
	int32_t _mState;
	bool _mPaused;
	bool _mWillFadeIn;
	char _mPreviousTrack[EE_FILENAME_MAX];
	char _mCurrentTrack[EE_FILENAME_MAX];
	char _mNextTrack[EE_FILENAME_MAX];
	Timer _mFadeTimer;
	SoundEffectInstance _mCurrentMusicInstance;
} MusicInstance;
#endif

void MusicInstance_HandleFadeIn(MusicInstance* mi, float musicVolume);
void MusicInstance_HandleFadeOut(MusicInstance* mi, float musicVolume);

enum MusicInstanceState
{
	STATE_FADE_IN = 0,
	STATE_WORKING = 1,
	STATE_FADE_OUT = 2,
	STATE_COMPLETE = 3
};

void MusicInstance_Init(MusicInstance* mi);

bool MusicInstance_IsLooped(MusicInstance* mi);
void MusicInstance_SetIsLooped(MusicInstance* mi, bool value);
const char* MusicInstance_CurrentTrack(MusicInstance* mi);
const char* MusicInstance_PreviousTrack(MusicInstance* mi);
void MusicInstance_Play(MusicInstance* mi, const char* nextTrack, bool isLooping, bool isUsingForcedVolume, float forcedVolume);
float MusicInstance_GetMusicVolume(MusicInstance* mi);
void MusicInstance_Stop(MusicInstance* mi);
void MusicInstance_Pause(MusicInstance* mi);
void MusicInstance_Resume(MusicInstance* mi);
void MusicInstance_Update(MusicInstance* mi);
void MusicInstance_SetupQueuedFadeIn(MusicInstance* mi, int32_t fadeInTime);
void MusicInstance_SetupFadeIn(MusicInstance* mi, int32_t fadeInTime);
void MusicInstance_SetupFadeOut(MusicInstance* mi, const char* nextTrack, int32_t fadeOutTime);
bool MusicInstance_IsFadingOut(MusicInstance* mi);
void MusicInstance_ClearTrackNames(MusicInstance* mi);
bool MusicInstance_IsPlaying(MusicInstance* mi);