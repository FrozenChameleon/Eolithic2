/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"
#include "WaveFileData.h"
#include "VolumeData.h"
#include "../io/BufferReader.h"

#ifdef AUDIO_DUMMY
typedef struct SoundEffect SoundEffect;
#else
typedef struct SoundEffect
{
	WaveFileData* _mWaveFileData;
} SoundEffect;
#endif

void SoundEffect_Init(void);
uint64_t SoundEffect_GetDefaultSoundPlaybackTimeBuffer(void);
void SoundEffect_SetDefaultSoundPlaybackTimeBuffer(uint64_t value);
void SoundEffect_SetPlaybackTimeBufferForSoundEffect(const char* name, uint64_t time);
void SoundEffect_SetSfxMuted(bool value);
uint64_t SoundEffect_GetCurrentFrame(void);
void SoundEffect_Tick(void);
void SoundEffect_PauseAllSounds(int32_t priority);
void SoundEffect_ResumeAllSounds(int32_t priority);
void SoundEffect_StopAllPausedSounds(void);
void SoundEffect_StopSound(const char* sound);
void SoundEffect_StopAllSounds(void);
void SoundEffect_PlaySound(const char* sound);
void SoundEffect_PlaySoundWithForcedVolume(const char* sound, float forcedVolume);
void SoundEffect_LoopSound(const char* sound, int32_t loopNumber);
float SoundEffect_GetVolumeHelper(const char* cvar, const char* name, VolumeData* volumeData);
const char* SoundEffect_GetExtension(void);
const char* SoundEffect_GetDirectories(void);
SoundEffect* SoundEffect_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);
void SoundEffect_Dispose(SoundEffect* music);