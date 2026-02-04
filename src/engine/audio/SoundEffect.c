/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#ifndef AUDIO_DUMMY

#include "../utils/Macros.h"
#include "SoundEffect.h"
#include "SoundEffectInstance.h"
#include "../utils/Utils.h"
#include "../../third_party/stb_ds.h"
#include "VolumeData.h"
#include "../globals/Globals.h"
#include "../core/GameUpdater.h"
#include "../utils/Logger.h"
#include "../resources/ResourceManagerList.h"
#include "../utils/Cvars.h"
#include "../gamestate/GameStateManager.h"

#define SFX_INSTANCE_LIMIT 128

typedef struct SoundEffectPlaybackTimeBuffer
{
	const char* mName;
	uint64_t mTime;
} SoundEffectPlaybackTimeBuffer;

static uint64_t DEFAULT_SOUND_PLAYBACK_TIME_BUFFER = 2;

static int32_t _mSoundPausePriority = -1;
static SoundEffectInstance _mInstances[SFX_INSTANCE_LIMIT];
static bool _mLoopStatus[SFX_INSTANCE_LIMIT];
static uint64_t _mCurrentFrame;
static VolumeData _mVolumeData;
static bool _mIsSfxMuted;
static SoundEffectPlaybackTimeBuffer* arr_sound_effect_playback_time_buffer;
static bool _mHasInit;

void SoundEffect_Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	VolumeData_Init(&_mVolumeData);
	VolumeData_Load(&_mVolumeData, false);

	_mHasInit = true;
}

static float GetVolumeForSoundEffect(const char* sound)
{
	if (_mIsSfxMuted)
	{
		return 0;
	}

	return SoundEffect_GetVolumeHelper(CVARS_USER_VOLUME_SFX, sound, &_mVolumeData);
}
static uint64_t GetPlaybackTimeBufferForSoundEffect(const char* name)
{
	int64_t len = arrlen(arr_sound_effect_playback_time_buffer);
	for (int32_t i = 0; i < len; i += 1)
	{
		SoundEffectPlaybackTimeBuffer* temp = &arr_sound_effect_playback_time_buffer[i];
		if (Utils_StringEqualTo(temp->mName, name))
		{
			return temp->mTime;
		}
	}

	return DEFAULT_SOUND_PLAYBACK_TIME_BUFFER;
}
static bool IsRewinding(void)
{
	if (GameState_IsRewinding(GameStateManager_GetGameState()))
	{
		return true;
	}
	return false;
}
static uint64_t GetLastFramePlayed(const char* sound)
{
	uint64_t highest = 0;
	for (int32_t i = 0; i < SFX_INSTANCE_LIMIT; i += 1)
	{
		SoundEffectInstance* instance = &_mInstances[i];
		if (Utils_StringEqualTo(sound, SoundEffectInstance_GetName(instance)))
		{
			if (SoundEffectInstance_GetFramePlayed(instance) > highest)
			{
				highest = SoundEffectInstance_GetFramePlayed(instance);
			}
		}
	}
	return highest;
}
static bool RefreshLoopStatus(const char* sound, int32_t loopNumber)
{
	for (int32_t i = 0; i < SFX_INSTANCE_LIMIT; i += 1)
	{
		SoundEffectInstance* instance = &_mInstances[i];
		if (Utils_StringEqualTo(sound, SoundEffectInstance_GetName(instance)))
		{
			if (SoundEffectInstance_GetLoopNumber(instance) == loopNumber)
			{
				_mLoopStatus[i] = true;
				return true;
			}
		}
	}
	return false;
}
static bool IsDisabledPermanently(void)
{
	return Globals_IsAudioDisabledPermanently();
}

SoundEffect* SoundEffect_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	SoundEffect* sfx = (SoundEffect*)Utils_calloc(1, sizeof(SoundEffect));
	sfx->_mWaveFileData = WaveFileData_FromStream(br);;
	return sfx;
}
void SoundEffect_Dispose(SoundEffect* music)
{
	WaveFileData_Dispose(music->_mWaveFileData);
	music->_mWaveFileData = NULL;

	Utils_free(music);
}

uint64_t SoundEffect_GetDefaultSoundPlaybackTimeBuffer(void)
{
	return DEFAULT_SOUND_PLAYBACK_TIME_BUFFER;
}
void SoundEffect_SetDefaultSoundPlaybackTimeBuffer(uint64_t value)
{
	DEFAULT_SOUND_PLAYBACK_TIME_BUFFER = value;
}
void SoundEffect_SetPlaybackTimeBufferForSoundEffect(const char* name, uint64_t time)
{
	int64_t len = arrlen(arr_sound_effect_playback_time_buffer);
	for (int32_t i = 0; i < len; i += 1)
	{
		SoundEffectPlaybackTimeBuffer* temp = &arr_sound_effect_playback_time_buffer[i];
		if (Utils_StringEqualTo(temp->mName, name))
		{
			temp->mTime = time;
			return;
		}
	}

	SoundEffectPlaybackTimeBuffer newBuffer = { 0 };
	newBuffer.mName = name;
	newBuffer.mTime = time;
	arrput(arr_sound_effect_playback_time_buffer, newBuffer);
}
void SoundEffect_SetSfxMuted(bool value)
{
	_mIsSfxMuted = value;
}
uint64_t SoundEffect_GetCurrentFrame(void)
{
	return _mCurrentFrame;
}
void SoundEffect_Tick(void)
{
	if (IsDisabledPermanently())
	{
		return;
	}

	for (int32_t i = 0; i < SFX_INSTANCE_LIMIT; i += 1)
	{
		SoundEffectInstance* instance = &_mInstances[i];
		if (!_mLoopStatus[i])
		{
			if (SoundEffectInstance_IsLooped(instance))
			{
				SoundEffectInstance_Stop(instance);
			}
		}
		_mLoopStatus[i] = false;
	}

	for (int32_t i = 0; i < SFX_INSTANCE_LIMIT; i += 1)
	{
		SoundEffectInstance* instance = &_mInstances[i];
		SoundEffectInstance_Update(instance);
		SoundEffectInstance_SetVolume(instance, GetVolumeForSoundEffect(SoundEffectInstance_GetName(instance)));
	}

	_mCurrentFrame += 1;
}
void SoundEffect_PauseAllSounds(int32_t priority)
{
	if (IsDisabledPermanently() || IsRewinding() || priority <= _mSoundPausePriority)
	{
		return;
	}

	for (int32_t i = 0; i < SFX_INSTANCE_LIMIT; i += 1)
	{
		SoundEffectInstance_Pause(&_mInstances[i]);
	}

	_mSoundPausePriority = priority;
}
void SoundEffect_ResumeAllSounds(int32_t priority)
{
	if (IsDisabledPermanently() || IsRewinding() || priority < _mSoundPausePriority)
	{
		return;
	}

	for (int32_t i = 0; i < SFX_INSTANCE_LIMIT; i += 1)
	{
		SoundEffectInstance_Resume(&_mInstances[i]);
	}
	_mSoundPausePriority = -1;
}
void SoundEffect_StopAllPausedSounds(void)
{
	if (IsDisabledPermanently() || IsRewinding())
	{
		return;
	}

	for (int32_t i = 0; i < SFX_INSTANCE_LIMIT; i += 1)
	{
		SoundEffectInstance* instance = &_mInstances[i];
		if (SoundEffectInstance_State(instance) == SOUNDSTATE_PAUSED)
		{
			SoundEffectInstance_Stop(instance);
		}
	}
}
void SoundEffect_StopSound(const char* sound)
{
	if (IsDisabledPermanently() || IsRewinding())
	{
		return;
	}

	for (int32_t i = 0; i < SFX_INSTANCE_LIMIT; i += 1)
	{
		SoundEffectInstance* instance = &_mInstances[i];
		if (Utils_StringEqualTo(sound, SoundEffectInstance_GetName(instance)))
		{
			SoundEffectInstance_Stop(instance);
		}
	}
}
void SoundEffect_StopAllSounds(void)
{
	if (IsDisabledPermanently() || IsRewinding())
	{
		return;
	}

	for (int32_t i = 0; i < SFX_INSTANCE_LIMIT; i += 1)
	{
		SoundEffectInstance_Stop(&_mInstances[i]);
	}
	_mSoundPausePriority = -1;
}
void SoundEffect_PlaySound(const char* sound)
{
	SoundEffect_PlaySoundWithForcedVolume(sound, -1);
}
static SoundEffectInstance* SetupNextInstance(const char* sound)
{
	SoundEffect* data = (SoundEffect*)ResourceManager_GetResourceData(ResourceManagerList_SoundEffect(), sound);
	if (data == NULL)
	{
		return NULL;
	}

	for (int32_t i = 0; i < SFX_INSTANCE_LIMIT; i += 1)
	{
		SoundEffectInstance* instance = &_mInstances[i];
		if (!SoundEffectInstance_IsSetup(instance) || (SoundEffectInstance_State(instance) == SOUNDSTATE_STOPPED))
		{
			bool success = SoundEffectInstance_Setup(instance, sound, data->_mWaveFileData);
			if (!success)
			{
				MString* tempString = NULL;
				MString_AssignString(&tempString, "Unable to setup sound effect: ");
				MString_AddAssignString(&tempString, sound);
				Logger_LogInformation(MString_Text(tempString));
				MString_Dispose(&tempString);
				return NULL;
			}
			else
			{
				SoundEffectInstance_Update(instance);
				return instance;
			}
		}
	}

	if (!Globals_IsDebugGameSpeedSet())
	{
		Logger_LogWarning("Reached sound effect instance limit");
	}
	return NULL;
}
void SoundEffect_PlaySoundWithForcedVolume(const char* sound, float forcedVolume)
{
	if (IsDisabledPermanently() || IsRewinding() || GameUpdater_IsDebugAutoSpeedOn())
	{
		return;
	}

	uint64_t lastFramePlayed = GetLastFramePlayed(sound);
	uint64_t buffer = GetPlaybackTimeBufferForSoundEffect(sound);
	if (_mCurrentFrame <= lastFramePlayed + buffer)
	{
		return;
	}

	SoundEffectInstance* instance = SetupNextInstance(sound);
	if (instance == NULL)
	{
		//reached limit
		return;
	}

	SoundEffectInstance_SetVolume(instance, forcedVolume != -1 ? forcedVolume : GetVolumeForSoundEffect(sound));
	SoundEffectInstance_Update(instance);
	SoundEffectInstance_Play(instance);
}
void SoundEffect_LoopSound(const char* sound, int32_t loopNumber)
{
	if (IsDisabledPermanently() || IsRewinding())
	{
		return;
	}

	if (RefreshLoopStatus(sound, loopNumber))
	{
		return;
	}

	SoundEffectInstance* instance = SetupNextInstance(sound);
	if (instance == NULL)
	{
		//reached limit
		return;
	}

	SoundEffectInstance_SetLoopNumber(instance, loopNumber);
	SoundEffectInstance_SetVolume(instance, GetVolumeForSoundEffect(sound));
	SoundEffectInstance_SetIsLooped(instance, true);
	SoundEffectInstance_Play(instance);

	RefreshLoopStatus(sound, loopNumber);
}
float SoundEffect_GetVolumeHelper(const char* cvar, const char* name, VolumeData* volumeData)
{
	if (IsDisabledPermanently() || (name == NULL) || (Utils_StringEqualTo(name, EE_STR_EMPTY)))
	{
		return 0;
	}

	float userVolume = Cvars_GetAsInt(cvar) / 100.0f;
	float userVolumeMaster = Cvars_GetAsInt(CVARS_USER_VOLUME_MASTER) / 100.0f;
	float clipVolume = VolumeData_GetVolume(volumeData, name) / 100.0f;
	float rewindingVolume = 1.0f;
	if (GameState_IsRewinding(GameStateManager_GetGameState()))
	{
		rewindingVolume = 0.6f;
	}
	return clipVolume * userVolume * userVolumeMaster * rewindingVolume;
}
const char* SoundEffect_GetExtension(void)
{
	return ".wav";
}
const char* SoundEffect_GetDirectories(void)
{
	return NULL;
}

#endif

typedef int compiler_warning_compliance;