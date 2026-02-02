/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "AudioEngine.h"

#ifdef AUDIO_FAUDIO

#include "FAudio.h"
#include "../utils/Logger.h"

static FAudio* _mFAudioContext;
static FAudioMasteringVoice* _mMasteringVoiceContext;
static bool _mHasInit;

int32_t AudioEngine_Init(void)
{
	if (_mHasInit)
	{
		return 0;
	}

	if (FAudioCreate(&_mFAudioContext, 0, FAUDIO_DEFAULT_PROCESSOR))
	{
		Logger_LogError("Unable to init FAudio!");
		return -1;
	}

	if (FAudio_CreateMasteringVoice(_mFAudioContext, &_mMasteringVoiceContext, FAUDIO_DEFAULT_CHANNELS, FAUDIO_DEFAULT_SAMPLERATE, 0, 0, NULL))
	{
		Logger_LogError("Unable to create mastering voice!");
		return -1;
	}

	_mHasInit = true;
	return 0;
}

void* AudioEngine_GetContext(void)
{
	return _mFAudioContext;
}

#endif