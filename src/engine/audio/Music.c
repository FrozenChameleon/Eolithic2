/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#ifndef AUDIO_DUMMY

#include "Music.h"

#include "../utils/Macros.h"
#include "../utils/Utils.h"
#include "VolumeData.h"
#include "MusicInstance.h"
#include "../globals/Globals.h"
#include "../utils/Cvars.h"
#include "SoundEffect.h"
#include "../gamestate/GameStateManager.h"

static VolumeData _mVolumeData;
static MusicInstance _mCurrentMusic;
static int32_t _mMusicPausePriority;
static bool _mDoNotAllowUpdatesWhilePaused;
static bool _mIsMusicMuted;
static bool _mHackToIgnoreNextFadeOutMusic;
static bool _mHasInit;

static bool IsRewinding(void)
{
	if (GameState_IsRewinding(GameStateManager_GetGameState()))
	{
		return true;
	}
	return false;
}
static bool IsDisabledPermanently(void)
{
	if (!_mHasInit)
	{
		return true;
	}

	return Globals_IsAudioDisabledPermanently();
}

void Music_Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	MusicInstance_Init(&_mCurrentMusic);
	VolumeData_Init(&_mVolumeData, true);
	VolumeData_Load(&_mVolumeData);

	_mHasInit = true;
}
float Music_GetVolumeForMusic(const char* name)
{
	if (IsDisabledPermanently() || _mIsMusicMuted)
	{
		return 0;
	}

	return SoundEffect_GetVolumeHelper(CVARS_USER_VOLUME_MUSIC, name, &_mVolumeData);
}
bool Music_IsPlaying(void)
{
	if (IsDisabledPermanently())
	{
		return false;
	}

	return MusicInstance_IsPlaying(&_mCurrentMusic);
}
Music* Music_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	Music* music = (Music*)Utils_calloc(1, sizeof(Music));
	music->_mWaveFileData = WaveFileData_FromStream(br);
	return music;
}
void Music_Dispose(Music* music)
{
	WaveFileData_Dispose(music->_mWaveFileData);
	music->_mWaveFileData = NULL;

	Utils_free(music);
}
bool Music_HackToIgnoreNextFadeOutMusic(void)
{
	return _mHackToIgnoreNextFadeOutMusic;
}
void Music_SetHackToIgnoreNextFadeOutMusic(bool value)
{
	_mHackToIgnoreNextFadeOutMusic = value;
}
const char* Music_GetExtension(void)
{
	return ".wav";
}
const char* Music_GetDirectories(void)
{
	return NULL;
}
void Music_SetMusicMuted(bool value)
{
	_mIsMusicMuted = value;
}
bool Music_DoNotAllowUpdatesWhilePaused(void)
{
	return _mDoNotAllowUpdatesWhilePaused;
}
void Music_SetDoNotAllowUpdatesWhilePaused(bool value)
{
	_mDoNotAllowUpdatesWhilePaused = value;
}
void Music_PauseMusic(int32_t priority)
{
	if (IsDisabledPermanently() || IsRewinding() || (priority <= _mMusicPausePriority))
	{
		return;
	}

	MusicInstance_Pause(&_mCurrentMusic);
	_mMusicPausePriority = priority;
}
void Music_ResumeMusic(int32_t priority)
{
	if (IsDisabledPermanently() || IsRewinding() || (priority < _mMusicPausePriority))
	{
		return;
	}

	MusicInstance_Resume(&_mCurrentMusic);
	_mMusicPausePriority = -1;
}
void Music_RestartMusic(void)
{
	if (IsDisabledPermanently() || IsRewinding())
	{
		return;
	}

	Music_PlayMusic(MusicInstance_CurrentTrack(&_mCurrentMusic), true, true, false, 0, false, 0);
}
void Music_PlayPreviousTrack(void)
{
	Music_PlayMusic(MusicInstance_PreviousTrack(&_mCurrentMusic), true, true, false, 0, false, 0);
}
void Music_PlayMusic(const char* nextTrack, bool isLooping, bool isForced, bool isFadeIn, int32_t fadeInTime, bool isFadeOut, int32_t fadeOutTime)
{
	if (IsDisabledPermanently() || IsRewinding())
	{
		return;
	}

	_mMusicPausePriority = -1;

	if (!isForced)
	{
		if (Utils_StringEqualTo(nextTrack, MusicInstance_CurrentTrack(&_mCurrentMusic)))
		{
			if (!MusicInstance_IsFadingOut(&_mCurrentMusic) && MusicInstance_IsPlaying(&_mCurrentMusic))
			{
				return;
			}
		}
	}

	if (isFadeOut)
	{
		MusicInstance_SetupFadeOut(&_mCurrentMusic, nextTrack, fadeOutTime);
		if (isFadeIn)
		{
			MusicInstance_SetupQueuedFadeIn(&_mCurrentMusic, fadeInTime);
		}
	}
	else
	{
		MusicInstance_Stop(&_mCurrentMusic);
		MusicInstance_Play(&_mCurrentMusic, nextTrack, isLooping, isFadeIn, 0);
		if (IsDisabledPermanently()) //Stop now if this flag is set because of playback
		{
			return;
		}
		if (isFadeIn)
		{
			MusicInstance_SetupFadeIn(&_mCurrentMusic, fadeInTime);
		}
	}
}
void Music_StopMusic(void)
{
	if (IsDisabledPermanently() || IsRewinding())
	{
		return;
	}

	MusicInstance_ClearTrackNames(&_mCurrentMusic);
	MusicInstance_Stop(&_mCurrentMusic);
	_mMusicPausePriority = -1;
}
void Music_FadeOutMusic(int32_t fadeOutTime)
{
	if (_mHackToIgnoreNextFadeOutMusic)
	{
		_mHackToIgnoreNextFadeOutMusic = false;
		return;
	}

	if (IsDisabledPermanently() || IsRewinding())
	{
		return;
	}

	MusicInstance_SetupFadeOut(&_mCurrentMusic, EE_STR_EMPTY, fadeOutTime);
}
void Music_Tick(void)
{
	if (IsDisabledPermanently())
	{
		return;
	}

	MusicInstance_Update(&_mCurrentMusic);
}

#endif

typedef int compiler_warning_compliance;