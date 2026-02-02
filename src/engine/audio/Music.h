/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "WaveFileData.h"
#include "VolumeData.h"

#ifdef AUDIO_DUMMY
typedef struct Music Music;
#else
typedef struct Music
{
	WaveFileData* _mWaveFileData;
} Music;
#endif

void Music_Init(void);
bool Music_HackToIgnoreNextFadeOutMusic(void);
void Music_SetHackToIgnoreNextFadeOutMusic(bool value);
const char* Music_GetExtension(void);
const char* Music_GetDirectories(void);
void Music_SetMusicMuted(bool value);
bool Music_DoNotAllowUpdatesWhilePaused(void);
void Music_SetDoNotAllowUpdatesWhilePaused(bool value);
void Music_PauseMusic(int32_t priority);
void Music_ResumeMusic(int32_t priority);
void Music_RestartMusic(void);
void Music_PlayPreviousTrack(void);
void Music_PlayMusic(const char* nextTrack, bool isLooping, bool isForced, bool isFadeIn, int32_t fadeInTime, bool isFadeOut, int32_t fadeOutTime);
void Music_StopMusic(void);
void Music_FadeOutMusic(int32_t fadeOutTime);
void Music_Tick(void);
float Music_GetVolumeForMusic(const char* name);
bool Music_IsPlaying(void);
Music* Music_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);
void Music_Dispose(Music* music);