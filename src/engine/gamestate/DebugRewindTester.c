/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "DebugRewindTester.h"

#include "../utils/Macros.h"
#include "../core/Func.h"
#include "../input/RecordingTool.h"
#include "../utils/Logger.h"
#include "../utils/Utils.h"

#define STATE_WAITING 0
#define STATE_REWINDING 1

typedef struct RewindTesterData
{
	int32_t mBeginLevelFrameCount;
	int32_t mEndLevelFrameCount;
	int32_t mStoredPlayerReadCounter;
	int32_t mState;
	int32_t mCounter;
	bool mIsRewinding;
} RewindTesterData;

static RewindTesterData _mData;

static bool IsActive(void)
{
#ifdef DEBUG_DEF_USE_REWIND_TESTER
	return true;
#else
	return false;
#endif
}

bool DebugRewindTester_NeedToRewind(void)
{
	if (!IsActive())
	{
		return false;
	}

	return _mData.mIsRewinding;
}
void DebugRewindTester_Reset(void)
{
	Utils_memset(&_mData, 0, sizeof(RewindTesterData));
}
void DebugRewindTester_Update(void)
{
	if (!IsActive())
	{
		return;
	}

#ifdef DEBUG_DEF_USE_REWIND_TESTER
	int32_t rewindInterval = DEBUG_DEF_REWIND_TESTER_REWIND_INTERVAL;
	int32_t rewindLength = DEBUG_DEF_REWIND_TESTER_REWIND_LENGTH;
#else 
	int32_t rewindInterval = 0;
	int32_t rewindLength = 0;
#endif

	switch (_mData.mState)
	{
	case STATE_WAITING:
		if (_mData.mCounter == (rewindInterval - rewindLength))
		{
			_mData.mBeginLevelFrameCount = Get_LevelFrameCount();
			_mData.mStoredPlayerReadCounter = RecordingTool_GetPlayerReadCounter();
		}
		if (_mData.mCounter >= rewindInterval)
		{
			_mData.mIsRewinding = true;
			_mData.mState = STATE_REWINDING;
			_mData.mCounter = 0;
		}
		else
		{
			_mData.mCounter += 1;
		}
		break;
	case STATE_REWINDING:
		RecordingTool_SetPlayerReadCounter(_mData.mStoredPlayerReadCounter);
		if (_mData.mCounter >= rewindLength)
		{
			_mData.mEndLevelFrameCount = Get_LevelFrameCount();
			if (_mData.mBeginLevelFrameCount != _mData.mEndLevelFrameCount)
			{
				Logger_LogInformation("Rewind failure!");
			}
			_mData.mIsRewinding = false;
			_mData.mState = STATE_WAITING;
			_mData.mCounter = 0;
		}
		else
		{
			_mData.mCounter += 1;
		}
		break;
	}
}
