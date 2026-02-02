/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

 /* Derived from code by Ethan Lee (Copyright 2009-2024).
  * Released under the Microsoft Public License.
  * See fna.LICENSE for details.

  * Derived from code by the Mono.Xna Team (Copyright 2006).
  * Released under the MIT License.
  * See monoxna.LICENSE for details.
  */

#ifdef AUDIO_FAUDIO

#include "SoundEffectInstance.h"

#include "FAudio.h"
#include "../utils/Logger.h"
#include "../utils/Exception.h"
#include "../../third_party/stb_ds.h"
#include "../utils/Utils.h"
#include "AudioEngine.h"
#include "../gamestate/GameStateManager.h"

static FAudioWaveFormatEx* GetFormat(SoundEffectInstance* instance)
{
	return (FAudioWaveFormatEx*)(instance->_mFormatStorage);
}
static FAudioSourceVoice* GetHandle(SoundEffectInstance* instance)
{
	return (FAudioSourceVoice*)(instance->_mHandleStorage);
}

bool SoundEffectInstance_Setup(SoundEffectInstance* sei, const char* name, WaveFileData* waveFileData)
{
	if (waveFileData == NULL)
	{
		Logger_LogInformation("Handed NULL wave file!");
		return false;
	}

	//DISPOSAL
	if (sei->_mBuffer != NULL)
	{
		Utils_free(sei->_mBuffer);
		sei->_mBuffer = NULL;
	}
	if (sei->_mFormatStorage != NULL)
	{
		Utils_free(sei->_mFormatStorage);
		sei->_mFormatStorage = NULL;
	}
	arrfree(sei->arr_queued_buffers);
	arrfree(sei->arr_queued_sizes);
	//

	bool isMusic = sei->_mIsMusic; //Hack to keep this flag even after memset!
	Utils_memset(sei, 0, sizeof(SoundEffectInstance));
	sei->_mIsMusic = isMusic;

	sei->_mHasSetup = true;
	Utils_strlcpy(sei->_mName, name, EE_FILENAME_MAX);
	sei->_mLoopNumber = -1;

	SoundEffectInstance_SetIsLooped(sei, false);
	sei->_mCurrentSample = 0;

	bool init = false;
	if (sei->_mData != NULL)
	{
		if ((sei->_mData->nSamplesPerSec != waveFileData->nSamplesPerSec) || (sei->_mData->nChannels != waveFileData->nChannels))
		{
			MString* tempString = NULL;
			MString_AssignString(&tempString, "Sound effect instance sample/channel mismatch for : ");
			MString_AddAssignString(&tempString, name);
			Logger_LogInformation(MString_Text(tempString));
			MString_Dispose(&tempString);
			init = true;
		}
	}
	else
	{
		init = true;
	}

	sei->_mData = waveFileData;
	if (init)
	{
		sei->_mBufferLength = (sei->_mData->nAvgBytesPerSec / 20); //10 = 100MS BUFFER, 20 = 50MS BUFFER
		sei->_mBuffer = (uint8_t*)Utils_calloc(sei->_mBufferLength, sizeof(uint8_t));
	}

	sei->_mInternalVolume = 1.0f;
	sei->_mInternalState = SOUNDSTATE_STOPPED;
	sei->_mSampleRate = waveFileData->nSamplesPerSec;
	sei->_mChannels = (waveFileData->nChannels == 1) ? AUDIOCHANNELS_MONO : AUDIOCHANNELS_STEREO;

	FAudioWaveFormatEx* format = (FAudioWaveFormatEx*)Utils_calloc(1, sizeof(FAudioWaveFormatEx));
	format->wFormatTag = 1;
	format->nChannels = (uint16_t)sei->_mChannels;
	format->nSamplesPerSec = (uint32_t)sei->_mSampleRate;
	format->wBitsPerSample = 16;
	format->nBlockAlign = (uint16_t)(2 * format->nChannels);
	format->nAvgBytesPerSec = format->nBlockAlign * format->nSamplesPerSec;
	format->cbSize = 0;
	sei->_mFormatStorage = format;

	return true;
}
bool SoundEffectInstance_IsSetup(const SoundEffectInstance* sei)
{
	return sei->_mHasSetup;
}
SoundState SoundEffectInstance_State(const SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup)
	{
		return SOUNDSTATE_STOPPED;
	}

	return sei->_mInternalState;
}
bool SoundEffectInstance_IsLooped(const SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup)
	{
		return false;
	}

	return sei->_mIsLooped;
}
void SoundEffectInstance_SetIsLooped(SoundEffectInstance* sei, bool value)
{
	if (!sei->_mHasSetup)
	{
		return;
	}

	sei->_mIsLooped = value;
}
void SoundEffectInstance_Play(SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup)
	{
		return;
	}

	// Wait! What if we need moar buffers?
	SoundEffectInstance_Update(sei);

	// Okay we're good
	if (sei->_mInternalState == SOUNDSTATE_PLAYING)
	{
		return;
	}
	if (sei->_mInternalState == SOUNDSTATE_PAUSED)
	{
		/* Just resume the existing handle */
		FAudioSourceVoice_Start(GetHandle(sei), 0, 0);
		sei->_mInternalState = SOUNDSTATE_PLAYING;
		return;
	}

	FAudioWaveFormatEx* format = GetFormat(sei);
	/* Create handle */
	FAudioSourceVoice* handle = NULL;
	FAudio_CreateSourceVoice((FAudio*)AudioEngine_GetContext(), &handle, format, FAUDIO_VOICE_USEFILTER, FAUDIO_DEFAULT_FREQ_RATIO, NULL, NULL, NULL);
	sei->_mHandleStorage = handle;

	if (handle == NULL)
	{
		return; /* What */
	}

	/* Apply current properties */
	FAudioVoice_SetVolume(handle, sei->_mInternalVolume, 0);

	/* For static effects, submit the buffer now */
	SoundEffectInstance_QueueInitialBuffers(sei);

	/* Play, finally. */
	FAudioSourceVoice_Start(handle, 0, 0);
	sei->_mInternalState = SOUNDSTATE_PLAYING;
	sei->_mHasStarted = true;
}
void SoundEffectInstance_Pause(SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup)
	{
		return;
	}

	FAudioSourceVoice* handle = GetHandle(sei);
	if ((handle != NULL) && (sei->_mInternalState == SOUNDSTATE_PLAYING))
	{
		FAudioSourceVoice_Stop(handle, 0, 0);
		sei->_mInternalState = SOUNDSTATE_PAUSED;
	}
}
void SoundEffectInstance_Stop(SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup)
	{
		return;
	}

	FAudioSourceVoice* handle = GetHandle(sei);

	if (handle == NULL)
	{
		return;
	}

	FAudioSourceVoice_Stop(handle, 0, 0);
	FAudioSourceVoice_FlushSourceBuffers(handle);
	FAudioVoice_DestroyVoice(handle);
	handle = NULL;
	sei->_mHandleStorage = NULL;
	sei->_mInternalState = SOUNDSTATE_STOPPED;
	SoundEffectInstance_ClearBuffers(sei);
}
void SoundEffectInstance_Update(SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup)
	{
		return;
	}

	if (sei->_mInternalState != SOUNDSTATE_PLAYING)
	{
		// Shh, we don't need you right now...
		return;
	}

	FAudioSourceVoice* handle = GetHandle(sei);
	if (handle != NULL)
	{
		FAudioVoiceState state;
		FAudioSourceVoice_GetState(
			handle,
			&state,
			FAUDIO_VOICE_NOSAMPLESPLAYED
		);
		while (SoundEffectInstance_PendingBufferCount(sei) > (int32_t)state.BuffersQueued)
		{
			Utils_free(sei->arr_queued_buffers[0]);
			arrdel(sei->arr_queued_buffers, 0);
		}
	}

	int32_t sampleSize = sei->_mData->nBlockAlign;
	int32_t totalSamples = sei->_mData->mWaveDataLength / sampleSize;
	int32_t maxBufferSamples = sei->_mBufferLength / sampleSize;
	bool isReverse = GameState_IsRewinding(GameStateManager_GetGameState()); //Read game state to determine reverse or not
	if (!sei->_mIsMusic)
	{
		isReverse = false;
	}
	if (isReverse)
	{
		if (!GameState_IsThereAnyRewindTimeRemaining(GameStateManager_GetGameState()))
		{
			return;
		}
	}

	if (sei->_mInternalState != SOUNDSTATE_PLAYING)
	{
		return;
	}

	if (!SoundEffectInstance_IsLooped(sei))
	{
		if ((isReverse && (sei->_mCurrentSample == 0)) || 
			(!isReverse && (sei->_mCurrentSample == (totalSamples - 1))))
		{
			if (SoundEffectInstance_PendingBufferCount(sei) == 0)
			{
				if (sei->_mInternalState == SOUNDSTATE_PLAYING)
				{
					SoundEffectInstance_Stop(sei);
				}
			}
			return;
		}
	}

	if (SoundEffectInstance_PendingBufferCount(sei) > 2)
	{
		return;
	}

	SoundEffectInstance_FillBuffer(sei, isReverse, maxBufferSamples);

	if (SoundEffectInstance_IsLooped(sei))
	{
		if (isReverse)
		{
			if (sei->_mCurrentSample == 0)
			{
				sei->_mCurrentSample = (totalSamples - 1);
			}
		}
		else
		{
			if (sei->_mCurrentSample == (totalSamples - 1))
			{
				sei->_mCurrentSample = sei->_mData->samplerLoopStart;
			}
		}
	}
}
void SoundEffectInstance_Dispose(SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup || sei->_mIsDisposed)
	{
		return;
	}

	SoundEffectInstance_Stop(sei);
	sei->_mIsDisposed = true;
}
int32_t SoundEffectInstance_GetLoopNumber(const SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup)
	{
		return 0;
	}

	return sei->_mLoopNumber;
}
void SoundEffectInstance_SetLoopNumber(SoundEffectInstance* sei, int32_t value)
{
	if (!sei->_mHasSetup)
	{
		return;
	}

	sei->_mLoopNumber = value;
}
const char* SoundEffectInstance_GetName(const SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup)
	{
		return NULL;
	}

	return sei->_mName;
}
uint64_t SoundEffectInstance_GetFramePlayed(const SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup)
	{
		return 0;
	}

	return sei->_mFramePlayed;
}
void SoundEffectInstance_Resume(SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup)
	{
		return;
	}

	if (sei->_mInternalState != SOUNDSTATE_PAUSED)
	{
		return;
	}

	//SoundState state = sei->_mInternalState; // Triggers a query, update
	FAudioSourceVoice* handle = GetHandle(sei);
	if (handle == NULL)
	{
		// XNA4 just plays if we've not started yet.
		SoundEffectInstance_Play(sei);
	}
	else if (sei->_mInternalState == SOUNDSTATE_PAUSED)
	{
		FAudioSourceVoice_Start(handle, 0, 0);
		sei->_mInternalState = SOUNDSTATE_PLAYING;
	}
}
float SoundEffectInstance_GetVolume(const SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup)
	{
		return 0;
	}

	return sei->_mInternalVolume;
}
void SoundEffectInstance_SetVolume(SoundEffectInstance* sei, float value)
{
	if (!sei->_mHasSetup)
	{
		return;
	}

	FAudioVoice* handle = GetHandle(sei);
	sei->_mInternalVolume = value;
	if (sei->_mHandleStorage != NULL)
	{
		FAudioVoice_SetVolume(
			handle,
			sei->_mInternalVolume,
			0
		);
	}
}
void SoundEffectInstance_QueueInitialBuffers(SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup)
	{
		return;
	}

	FAudioWaveFormatEx* format = GetFormat(sei);
	FAudioSourceVoice* handle = GetHandle(sei);
	FAudioBuffer buffer = { 0 };
	int64_t len = arrlen(sei->arr_queued_buffers);
	for (int32_t i = 0; i < len; i += 1)
	{
		buffer.AudioBytes = sei->arr_queued_sizes[i];
		buffer.pAudioData = sei->arr_queued_buffers[i];
		buffer.PlayLength = (
			buffer.AudioBytes /
			(uint32_t)sei->_mChannels /
			(uint32_t)(format->wBitsPerSample / 8)
			);
		FAudioSourceVoice_SubmitSourceBuffer(
			handle,
			&buffer,
			NULL
		);
	}
	arrsetlen(sei->arr_queued_sizes, 0);
}
void SoundEffectInstance_ClearBuffers(SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup)
	{
		return;
	}

	for (int32_t i = 0; i < arrlen(sei->arr_queued_buffers); i += 1)
	{
		Utils_free(sei->arr_queued_buffers[i]);
	}
	arrsetlen(sei->arr_queued_buffers, 0);
	arrsetlen(sei->arr_queued_sizes, 0);
}
void SoundEffectInstance_FillBuffer(SoundEffectInstance* sei, bool isReverse, int32_t amountOfSamples)
{
	if (!sei->_mHasSetup)
	{
		return;
	}

	//OeConsole.LogMessage("begin" + _mCurrentSample);

	Utils_memset(sei->_mBuffer, 0, sizeof(uint8_t) * sei->_mBufferLength);

	int32_t sampleSize = SoundEffectInstance_GetSampleSize(sei);
	int32_t totalSamples = SoundEffectInstance_GetTotalSamples(sei);
	int32_t byteCount = 0;
	for (int32_t i = 0; i < amountOfSamples; i += 1)
	{
		if ((isReverse && (sei->_mCurrentSample > 0)) || 
			(!isReverse && sei->_mCurrentSample < (totalSamples - 1)))
		{
			for (int32_t j = 0; j < sampleSize; j += 1)
			{
				int32_t bufferLoc = i * sampleSize + j;
				int32_t dataLoc = sei->_mCurrentSample * sampleSize + j;
				
				sei->_mBuffer[bufferLoc] = FixedByteBuffer_GetBuffer(sei->_mData->mWaveData)[dataLoc];
				byteCount += 1;
			}
			if (isReverse)
			{
				sei->_mCurrentSample -= 1;
			}
			else
			{
				sei->_mCurrentSample += 1;
			}
		}
	}

	if (byteCount != 0)
	{
		SoundEffectInstance_SubmitBuffer(sei, sei->_mBuffer, byteCount);
	}

	//OeConsole.LogMessage("end" + _mCurrentSample);
}
void SoundEffectInstance_SubmitBuffer(SoundEffectInstance* sei, uint8_t* buffer, int32_t bufferLength)
{
	if (!sei->_mHasSetup)
	{
		return;
	}

	uint8_t* next = (uint8_t*)Utils_calloc(bufferLength, sizeof(uint8_t));
	Utils_memcpy(next, buffer, sizeof(uint8_t) * bufferLength);
	arrput(sei->arr_queued_buffers, next);

	if (sei->_mInternalState != SOUNDSTATE_STOPPED)
	{
		FAudioBuffer buf = { 0 };
		buf.AudioBytes = (uint32_t)bufferLength;
		buf.pAudioData = next;
		buf.PlayLength = (
			buf.AudioBytes /
			(uint32_t)sei->_mChannels /
			(uint32_t)(GetFormat(sei)->wBitsPerSample / 8)
			);
		FAudioSourceVoice_SubmitSourceBuffer(
			GetHandle(sei),
			&buf,
			NULL
		);
	}
	else
	{
		arrput(sei->arr_queued_sizes, (uint32_t)bufferLength);
	}
}
int32_t SoundEffectInstance_GetSampleSize(const SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup)
	{
		return 0;
	}

	return (int32_t)sei->_mData->nBlockAlign;
}
int32_t SoundEffectInstance_GetTotalSamples(const SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup)
	{
		return 0;
	}

	return (sei->_mData->mWaveDataLength / SoundEffectInstance_GetSampleSize(sei));
}
int64_t SoundEffectInstance_PendingBufferCount(const SoundEffectInstance* sei)
{
	if (!sei->_mHasSetup)
	{
		return 0;
	}

	return arrlen(sei->arr_queued_buffers);
}

#endif

typedef int compiler_warning_compliance;