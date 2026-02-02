/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "ReplayDataManager.h"

#include "GameStateData.h"
#include "../core/GameHelper.h"
#include "../utils/Utils.h"

enum
{
	REPLAYDATAMANAGER_REPLAYGAMESTATEDATA_LEN = (REPLAYDATAMANAGER_REPLAY_BUFFER_SIZE / REPLAYDATAMANAGER_REPLAY_INTERVAL)
};

static GameStateInputData _mDummy;

typedef struct ReplayDataManager
{
	bool _mIsActive;
	int32_t _mLength;
	uint64_t _mReplayFrames[REPLAYDATAMANAGER_REPLAY_BUFFER_SIZE];
	GameStateInputData _mReplayInputData[REPLAYDATAMANAGER_REPLAY_BUFFER_SIZE];
	GameStateData _mReplayGameStateData[REPLAYDATAMANAGER_REPLAYGAMESTATEDATA_LEN];
	uint64_t _mReplayFrameToGivePlayer;
	uint64_t _mDebugRewindLoopReplayFrame;
} ReplayDataManager;

int32_t ReplayDataManager_GetIndexFromReplayFrame(ReplayDataManager* rdm, uint64_t replayFrame)
{
	for (int32_t i = 0; i < rdm->_mLength; i += 1)
	{
		if (rdm->_mReplayFrames[i] == replayFrame)
		{
			return i;
		}
	}
	return -1;
}
int32_t ReplayDataManager_GetIndexForLowestFrame(ReplayDataManager* rdm)
{
	return ReplayDataManager_GetIndexForLowestFrame2(rdm, false);
}
int32_t ReplayDataManager_GetIndexForLowestFrame2(ReplayDataManager* rdm, bool mustBeKeyFrame)
{
	int32_t lowestIndex = -1;
	uint64_t lowestFrame = UINT64_MAX;
	for (int32_t i = 0; i < rdm->_mLength; i += 1)
	{
		if (rdm->_mReplayFrames[i] < lowestFrame)
		{
			if (mustBeKeyFrame)
			{
				if ((i % REPLAYDATAMANAGER_REPLAY_INTERVAL) != 0)
				{
					continue;
				}
			}
			lowestIndex = i;
			lowestFrame = rdm->_mReplayFrames[i];
		}
	}
	return lowestIndex;
}

ReplayDataManager* ReplayDataManager_Create(const char* name)
{
	ReplayDataManager* rdm = (ReplayDataManager*)Utils_calloc(1, sizeof(ReplayDataManager));
	for (int32_t i = 0; i < REPLAYDATAMANAGER_REPLAYGAMESTATEDATA_LEN; i += 1)
	{
		GameHelper_InitGameStateData(name, &rdm->_mReplayGameStateData[i]);
	}
	return rdm;
}
void ReplayDataManager_Dispose(ReplayDataManager* rdm)
{
	Utils_free(rdm);
}
void ReplayDataManager_RewindLoop(ReplayDataManager* rdm, bool justStarted, GameStateData* actualCurrentGameState)
{
	int32_t index;
	if (justStarted)
	{
		index = ReplayDataManager_GetIndexForLowestFrame2(rdm, true);
		if (index < 0)
		{
			return;
		}
		rdm->_mDebugRewindLoopReplayFrame = rdm->_mReplayFrames[index];
		GameStateData_CopyTo(&rdm->_mReplayGameStateData[index / REPLAYDATAMANAGER_REPLAY_INTERVAL], actualCurrentGameState);
	}
	else
	{
		rdm->_mDebugRewindLoopReplayFrame += 1;
		index = ReplayDataManager_GetIndexFromReplayFrame(rdm, rdm->_mDebugRewindLoopReplayFrame);
	}

	if (index < 0)
	{
		ReplayDataManager_RewindLoop(rdm, true, actualCurrentGameState);
		return;
	}
	else
	{
		rdm->_mReplayFrameToGivePlayer = rdm->_mDebugRewindLoopReplayFrame;
		GameHelper_UpdateStateSystems();
	}
}
uint64_t ReplayDataManager_GetLowestReplayFrame(ReplayDataManager* rdm)
{
	int32_t lowestIndex = ReplayDataManager_GetIndexForLowestFrame2(rdm, true);
	if (lowestIndex == -1)
	{
		return 0;
	}

	return rdm->_mReplayFrames[lowestIndex];
}
void ReplayDataManager_RewindToSnapshot(ReplayDataManager* rdm, uint64_t replayFrameToRewindTo, GameStateData* actualCurrentGameState)
{
	int32_t simulations = 1;
	uint64_t targetFrame = replayFrameToRewindTo;
	int32_t targetIndex = ReplayDataManager_GetIndexFromReplayFrame(rdm, targetFrame);
	if (targetIndex < 0)
	{
		return;
	}

	while ((targetIndex % REPLAYDATAMANAGER_REPLAY_INTERVAL) != 0)
	{
		simulations += 1;
		targetFrame -= 1;
		targetIndex = ReplayDataManager_GetIndexFromReplayFrame(rdm, targetFrame);
		if (targetIndex < 0)
		{
			return;
		}
	}

	for (int32_t i = 0; i < simulations; i += 1)
	{
		rdm->_mReplayFrameToGivePlayer = targetFrame;
		if (i == 0)
		{
			GameStateData_CopyTo(&rdm->_mReplayGameStateData[targetIndex / REPLAYDATAMANAGER_REPLAY_INTERVAL], actualCurrentGameState);
		}
		GameHelper_UpdateStateSystems();
		targetFrame += 1;
	}
}
void ReplayDataManager_Snapshot(ReplayDataManager* rdm, uint64_t currentReplayFrame, GameStateData* actualCurrentGameState)
{
	int32_t index = ReplayDataManager_GetIndexFromReplayFrame(rdm, currentReplayFrame);
	if (index < 0)
	{
		if (rdm->_mLength < REPLAYDATAMANAGER_REPLAY_BUFFER_SIZE)
		{
			index = rdm->_mLength;
			rdm->_mLength += 1;
		}
		else
		{
			index = ReplayDataManager_GetIndexForLowestFrame(rdm);
			if (index < 0)
			{
				return;
			}
		}
	}

	rdm->_mReplayFrames[index] = currentReplayFrame;
	if ((index % REPLAYDATAMANAGER_REPLAY_INTERVAL) == 0)
	{
		GameStateData_CopyTo(actualCurrentGameState, &rdm->_mReplayGameStateData[index / REPLAYDATAMANAGER_REPLAY_INTERVAL]);
	}
}
void ReplayDataManager_GiveReplayInput(ReplayDataManager* rdm, uint64_t currentReplayFrame, const GameStateInputData* replayInputData)
{
	int32_t index = ReplayDataManager_GetIndexFromReplayFrame(rdm, currentReplayFrame);
	if (index >= 0)
	{
		rdm->_mReplayInputData[index] = *replayInputData;
	}
}
const GameStateInputData* ReplayDataManager_GetReplayInput(ReplayDataManager* rdm)
{
	int32_t index = ReplayDataManager_GetIndexFromReplayFrame(rdm, rdm->_mReplayFrameToGivePlayer);
	if (index < 0)
	{
		return &_mDummy;
	}
	else
	{
		return &rdm->_mReplayInputData[index];
	}
}
void ReplayDataManager_Clear(ReplayDataManager* rdm)
{
	rdm->_mLength = 0;
}
