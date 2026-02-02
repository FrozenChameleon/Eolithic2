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

#pragma once

#include "stdint.h"
#include "stdbool.h"
#include "WaveFileData.h"
#include "AudioChannels.h"
#include "SoundState.h"

#ifdef AUDIO_DUMMY
typedef struct SoundEffectInstance SoundEffectInstance;
#else
typedef struct SoundEffectInstance
{
	uint64_t _mFramePlayed;
	int32_t _mLoopNumber;
	bool _mHasSetup;
	int32_t _mCurrentSample;
	bool _mIsMusic;
	bool _mIsLooped;
	uint8_t* _mBuffer;
	int32_t _mBufferLength;
	WaveFileData* _mData;
	char _mName[EE_FILENAME_MAX];
	bool _mIsDisposed;
	int32_t _mSampleRate;
	AudioChannels _mChannels;
	SoundState _mInternalState;
	float _mInternalVolume;
	bool _mHasStarted;
	uint8_t** arr_queued_buffers;
	uint32_t* arr_queued_sizes;
	void* _mHandleStorage;
	void* _mFormatStorage;
} SoundEffectInstance;
#endif

bool SoundEffectInstance_Setup(SoundEffectInstance* sei, const char* name, WaveFileData* waveFileData);
bool SoundEffectInstance_IsSetup(const SoundEffectInstance* sei);
int32_t SoundEffectInstance_GetLoopNumber(const SoundEffectInstance* sei);
void SoundEffectInstance_SetLoopNumber(SoundEffectInstance* sei, int32_t value);
const char* SoundEffectInstance_GetName(const SoundEffectInstance* sei);
uint64_t SoundEffectInstance_GetFramePlayed(const SoundEffectInstance* sei);
void SoundEffectInstance_Resume(SoundEffectInstance* sei);
float SoundEffectInstance_GetVolume(const SoundEffectInstance* sei);
void SoundEffectInstance_SetVolume(SoundEffectInstance* sei, float value);
SoundState SoundEffectInstance_State(const SoundEffectInstance* sei);
bool SoundEffectInstance_IsLooped(const SoundEffectInstance* sei);
void SoundEffectInstance_SetIsLooped(SoundEffectInstance* sei, bool value);
void SoundEffectInstance_Play(SoundEffectInstance* sei);
void SoundEffectInstance_Pause(SoundEffectInstance* sei);
void SoundEffectInstance_Stop(SoundEffectInstance* sei);
void SoundEffectInstance_Update(SoundEffectInstance* sei);
void SoundEffectInstance_Dispose(SoundEffectInstance* sei);
void SoundEffectInstance_QueueInitialBuffers(SoundEffectInstance* sei);
void SoundEffectInstance_ClearBuffers(SoundEffectInstance* sei);
void SoundEffectInstance_FillBuffer(SoundEffectInstance* sei, bool isReverse, int32_t amountOfSamples);
void SoundEffectInstance_SubmitBuffer(SoundEffectInstance* sei, uint8_t* buffer, int32_t bufferLength);
int32_t SoundEffectInstance_GetSampleSize(const SoundEffectInstance* sei);
int32_t SoundEffectInstance_GetTotalSamples(const SoundEffectInstance* sei);
int64_t SoundEffectInstance_PendingBufferCount(const SoundEffectInstance* sei);