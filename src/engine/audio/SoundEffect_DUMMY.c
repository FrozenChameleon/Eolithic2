/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#ifdef AUDIO_DUMMY

#include "SoundEffect.h"

void SoundEffect_Init(void)
{
}

static float GetVolumeForSoundEffect(const char* sound)
{
	return 0;
}
static uint64_t GetPlaybackTimeBufferForSoundEffect(const char* name)
{
	return 0;
}
static bool IsRewinding(void)
{
	return false;
}
static uint64_t GetLastFramePlayed(const char* sound)
{
	return 0;
}
static bool RefreshLoopStatus(const char* sound, int32_t loopNumber)
{
	return false;
}
static bool IsDisabledPermanently(void)
{
	return false;
}

SoundEffect* SoundEffect_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	return NULL;
}
void SoundEffect_Dispose(SoundEffect* music)
{
}
uint64_t SoundEffect_GetDefaultSoundPlaybackTimeBuffer(void)
{
	return 0;
}
void SoundEffect_SetDefaultSoundPlaybackTimeBuffer(uint64_t value)
{
}
void SoundEffect_SetPlaybackTimeBufferForSoundEffect(const char* name, uint64_t time)
{
}
void SoundEffect_SetSfxMuted(bool value)
{
}
uint64_t SoundEffect_GetCurrentFrame(void)
{
	return 0;
}
void SoundEffect_Tick(void)
{
}
void SoundEffect_PauseAllSounds(int32_t priority)
{
}
void SoundEffect_ResumeAllSounds(int32_t priority)
{
}
void SoundEffect_StopAllPausedSounds(void)
{
}
void SoundEffect_StopSound(const char* sound)
{
}
void SoundEffect_StopAllSounds(void)
{
}
void SoundEffect_PlaySound(const char* sound)
{
}
void SoundEffect_PlaySoundWithForcedVolume(const char* sound, float forcedVolume)
{
}
void SoundEffect_LoopSound(const char* sound, int32_t loopNumber)
{
}
float SoundEffect_GetVolumeHelper(const char* cvar, const char* name, VolumeData* volumeData)
{
	return 0;
}
const char* SoundEffect_GetExtension(void)
{
	return NULL;
}
const char* SoundEffect_GetDirectories(void)
{
	return NULL;
}

#endif

typedef int compiler_warning_compliance;