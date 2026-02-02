/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "GameStateInputData.h"
#include "GameStateData.h"

enum
{
	REPLAYDATAMANAGER_REPLAY_BUFFER_SIZE = GLOBAL_DEF_REPLAY_BUFFER_SIZE,
	REPLAYDATAMANAGER_REPLAY_INTERVAL = GLOBAL_DEF_REPLAY_INTERVAL, //Real frame once every interval
};

typedef struct ReplayDataManager ReplayDataManager;

ReplayDataManager* ReplayDataManager_Create(const char* name);
void ReplayDataManager_Dispose(ReplayDataManager* rdm);
int32_t ReplayDataManager_GetIndexFromReplayFrame(ReplayDataManager* rdm, uint64_t replayFrame);
int32_t ReplayDataManager_GetIndexForLowestFrame(ReplayDataManager* rdm);
int32_t ReplayDataManager_GetIndexForLowestFrame2(ReplayDataManager* rdm, bool mustBeKeyFrame);
void ReplayDataManager_RewindLoop(ReplayDataManager* rdm, bool justStarted, GameStateData* actualCurrentGameState);
uint64_t ReplayDataManager_GetLowestReplayFrame(ReplayDataManager* rdm);
void ReplayDataManager_RewindToSnapshot(ReplayDataManager* rdm, uint64_t replayFrameToRewindTo, GameStateData* actualCurrentGameState);
void ReplayDataManager_Snapshot(ReplayDataManager* rdm, uint64_t currentReplayFrame, GameStateData* actualCurrentGameState);
void ReplayDataManager_GiveReplayInput(ReplayDataManager* rdm, uint64_t currentReplayFrame, const GameStateInputData* replayInputData);
const GameStateInputData* ReplayDataManager_GetReplayInput(ReplayDataManager* rdm);
void ReplayDataManager_Clear(ReplayDataManager* rdm);