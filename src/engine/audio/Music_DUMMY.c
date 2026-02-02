/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#ifdef AUDIO_DUMMY

#include "Music.h"

void Music_Init(void)
{
}
float Music_GetVolumeForMusic(const char* name)
{
	return 0;
}
bool Music_IsPlaying(void)
{
	return false;
}
Music* Music_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	return NULL;
}
void Music_Dispose(Music* music)
{
}
bool Music_HackToIgnoreNextFadeOutMusic(void)
{
	return false;
}
void Music_SetHackToIgnoreNextFadeOutMusic(bool value)
{
}
const char* Music_GetExtension(void)
{
	return NULL;
}
const char* Music_GetDirectories(void)
{
	return NULL;
}
void Music_SetMusicMuted(bool value)
{
}
bool Music_DoNotAllowUpdatesWhilePaused(void)
{
	return false;
}
void Music_SetDoNotAllowUpdatesWhilePaused(bool value)
{
}
void Music_PauseMusic(int32_t priority)
{
}
void Music_ResumeMusic(int32_t priority)
{
}
void Music_RestartMusic(void)
{
}
void Music_PlayPreviousTrack(void)
{
}
void Music_PlayMusic(const char* nextTrack, bool isLooping, bool isForced, bool isFadeIn, int32_t fadeInTime, bool isFadeOut, int32_t fadeOutTime)
{
}
void Music_StopMusic(void)
{
}
void Music_FadeOutMusic(int32_t fadeOutTime)
{
}
void Music_Tick(void)
{
}

#endif

typedef int compiler_warning_compliance;