/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GameState.h"

#include "GameStateManager.h"
#include "ReplayDataManager.h"
#include "DebugRewindTester.h"
#include "../core/Func.h"
#include "../utils/Utils.h"
#include "../utils/Cvars.h"
#include "../core/GameHelper.h"
#include "../leveldata/Particle.h"
#include "../resources/ResourceManagerList.h"
#include "../leveldata/ParticleInstance.h"
#include "../leveldata/ParticleInstanceSys.h"
#include "../utils/Logger.h"
#include "../input/Keys.h"
#include "../input/Input.h"
#include "../input/ActionList.h"

void GameState_ClearReplayCache(GameState* gs)
{
	gs->_mCurrentReplayFrame = 0;
	if (gs->_mReplayDataManager != NULL)
	{
		ReplayDataManager_Clear(gs->_mReplayDataManager);
	}
	DebugRewindTester_Reset();
}

void GameState_Ctor(GameState* gs, const char* name, bool disableRewinding)
{
	Utils_memset(gs, 0, sizeof(GameState));

	if (Cvars_GetAsBool(CVARS_ENGINE_DISABLE_REPLAYS))
	{
		disableRewinding = true;
	}

	gs->_mCurrentReplayFrame = 0;
	gs->_mDisableRewindingPermanently = disableRewinding;
	Utils_strlcpy(gs->_mName, name, EE_FILENAME_MAX);
	GameStateData_Ctor(&gs->_mData);
	GameStateData_Ctor(&gs->_mForGameState);
	if (!disableRewinding)
	{
		gs->_mReplayDataManager = ReplayDataManager_Create(name);
	}
#ifdef EDITOR_MODE
	GameStateData_Ctor(&gs->_mDebugSaveState);
#endif
}

GameStateData* GameState_GetGameStateData(GameState* gs)
{
	return &gs->_mData;
}
const char* GameState_GetName(GameState* gs)
{
	return gs->_mName;
}
void GameState_SaveComponentSizes(GameState* gs)
{
	//GameState_SaveComponentSizesHelper(true); //UNUSED
	//GameState_SaveComponentSizesHelper(false); //UNUSED
}

ParticleInstance* GameState_GetParticleInstance(GameState* gs, const char* name, float x, float y)
{
	Particle* particleData = (Particle*)ResourceManager_GetResourceData(ResourceManagerList_Particle(), name);
	if (particleData == NULL)
	{
		Logger_LogInformation("Particle missing:");
		Logger_LogInformation(name);
		return ParticleInstance_Dummy();
	}

	ComponentPack* pack = Get_ComponentPack(C_ParticleInstance);
	ComponentPack_SetMaximumCapacity(pack, GLOBAL_DEF_PARTICLE_LIMIT);
	if (ComponentPack_Length(pack) >= GLOBAL_DEF_PARTICLE_LIMIT)
	{
		return (ParticleInstance*)ComponentPack_GetDummy(pack);
	}
	Entity particleEntity = Get_DummyEntityUniqueToPack2(pack, "Particle Instances");
	ParticleInstance* particleInstance = (ParticleInstance*)ComponentPack_Set2(pack, particleEntity, true);
	ParticleInstanceSys_Setup(particleInstance, name, particleData, x, y);
	return particleInstance;
}

void GameState_RemoveEntity(GameState* gs, Entity entity)
{
	GameStateData_RemoveEntity(&gs->_mData, entity);
}
void GameState_FillListWithEntitiesInPlay(GameState* gs, EntitySearch* list)
{
	GameStateData_FillListWithEntitiesInPlay(&gs->_mData, list);
}
int32_t GameState_GetAmountOfEntitiesInPlay(GameState* gs)
{
	return GameStateData_GetAmountOfEntitiesInPlay(&gs->_mData);
}
void GameState_Do_SendBroadcast(GameState* gs, int32_t type, int32_t packet1, int32_t packet2, int32_t packet3)
{
	int32_t stateSystemsLen = GameStateManager_GetStateSystemsLen();
	System** stateSystems = GameStateManager_GetStateSystems();
	for (int32_t i = 0; i < stateSystemsLen; i += 1)
	{
		System* stateSys = stateSystems[i];
		stateSys->_mReceiveBroadcast(stateSys->_mData, type, packet1, packet2, packet3);
	}
}
Entity GameState_BuildNewEntity(GameState* gs)
{
	return GameStateData_BuildNewEntity(&gs->_mData);
}

void GameState_Update(GameState* gs)
{
	if (gs->_mClearReplayCacheNextFrame)
	{
		GameState_ClearReplayCache(gs);
		gs->_mClearReplayCacheNextFrame = false;
	}

	if (gs->_mDisableRewindingPermanently || gs->_mDisableRewindTemporarily)
	{
		GameHelper_UpdateStateSystems();
	}
	else
	{
		bool isRewindingThisFrame = GameHelper_HandleRewindBeforeUpdatingStateSystems();
		if (!isRewindingThisFrame)
		{
			GameHelper_UpdateStateSystems();
			gs->_mCurrentReplayFrame += 1;
			if (gs->_mCurrentReplayFrame == 0) //Overflow check
			{
				GameState_ClearReplayCache(gs);
			}
		}
	}
}
void GameState_SetDisableRewindTemporarily(GameState* gs, bool value)
{
	gs->_mDisableRewindTemporarily = value;
}
void GameState_SetGameStateToNotRewinding(GameState* gs)
{
	gs->_mIsRewinding = false;
}
bool GameState_IsRewinding(GameState* gs)
{
	return gs->_mIsRewinding;
}
bool GameState_Rewind(GameState* gs)
{
	gs->_mIsRewinding = true;

	if (gs->_mCurrentReplayFrame == 0)
	{
		return false;
	}

	uint64_t lowestFrame = ReplayDataManager_GetLowestReplayFrame(gs->_mReplayDataManager);
	if (gs->_mCurrentReplayFrame <= lowestFrame)
	{
		return false;
	}

	gs->_mCurrentReplayFrame -= 1;

	ReplayDataManager_RewindToSnapshot(gs->_mReplayDataManager, gs->_mCurrentReplayFrame, &gs->_mData);

	GameStateManager_UpdateLastRenderPosition();

	return true;
}
void GameState_GiveReplayInput(GameState* gs, const GameStateInputData* inputData)
{
	if (gs->_mReplayDataManager == NULL)
	{
		return;
	}

	ReplayDataManager_GiveReplayInput(gs->_mReplayDataManager, gs->_mCurrentReplayFrame, inputData);
}
const GameStateInputData* GameState_GetReplayInput(GameState* gs)
{
	return ReplayDataManager_GetReplayInput(gs->_mReplayDataManager);
}
void GameState_ClearReplayCacheNextFrame(GameState* gs)
{
	gs->_mClearReplayCacheNextFrame = true;
}
void GameState_Draw(GameState* gs, SpriteBatch* spriteBatch)
{
	GameHelper_DrawStateSystems(spriteBatch);
}
void GameState_DrawHud(GameState* gs, SpriteBatch* spriteBatch)
{
	GameHelper_DrawHudStateSystems(spriteBatch);
}
void GameState_DrawDebugHud(GameState* gs, SpriteBatch* spriteBatch)
{
	GameHelper_DrawDebugHudStateSystems(spriteBatch);
}

void GameState_Load(GameState* gs, const char* levelDataToLoad)
{
#ifdef EDITOR_MODE
	gs->_mIsRewindLooping = false;
#endif

	GameState_ClearReplayCache(gs);

	GameStateData_Reset(&gs->_mData);

	GameHelper_InitGameState(levelDataToLoad);
}
Entity GameState_GetEntityInPlay(GameState* gs, int32_t entityNumber)
{
	return GameStateData_GetEntityInPlay(&gs->_mData, entityNumber);
}
void GameState_UpdateReplayFrame(GameState* gs)
{
	ReplayDataManager_Snapshot(gs->_mReplayDataManager, gs->_mCurrentReplayFrame, &gs->_mData);
}
bool GameState_HasCreatedForGameSaveState(GameState* gs)
{
	return gs->_mHasSavedForGameSaveState;
}
void GameState_ClearForGameSaveState(GameState* gs)
{
	gs->_mHasSavedForGameSaveState = false;
}
void GameState_CreateForGameSaveState(GameState* gs)
{
	gs->_mHasSavedForGameSaveState = true;
	GameStateData_CopyTo(&gs->_mData, &gs->_mForGameState);
}
void GameState_UseForGameSaveState(GameState* gs)
{
	if (!gs->_mHasSavedForGameSaveState)
	{
		return;
	}

	GameStateData_CopyTo(&gs->_mForGameState, &gs->_mData);
}
int32_t GameState_GetRemainingRewindTime(GameState* gs)
{
	return (int32_t)(gs->_mCurrentReplayFrame - ReplayDataManager_GetLowestReplayFrame(gs->_mReplayDataManager));
}
bool GameState_IsThereAnyRewindTimeRemaining(GameState* gs)
{
	return GameState_GetRemainingRewindTime(gs) > 0;
}
float GameState_GetPercentageOfRemainingRewindTime(GameState* gs)
{
	int32_t current = GameState_GetRemainingRewindTime(gs);
	int32_t max = REPLAYDATAMANAGER_REPLAY_BUFFER_SIZE;
	return ((float)current / max);
}
ComponentPack* GameState_GetComponentPack(GameState* gs, ComponentType ctype)
{
	return GameStateData_GetComponentPack(&gs->_mData, ctype);
}
bool GameState_HasComponent(GameState* gs, ComponentType ctype, Entity entity)
{
	return GameStateData_HasComponent(&gs->_mData, ctype, entity);
}
void* GameState_GetComponent(GameState* gs, ComponentType ctype, Entity entity) //Just another name for Set
{
	return GameStateData_Set(&gs->_mData, ctype, entity);
}
void* GameState_Set(GameState* gs, ComponentType ctype, Entity entity)
{
	return GameStateData_Set(&gs->_mData, ctype, entity);
}
void* GameState_SetAndInit(GameState* gs, ComponentType ctype, Entity entity)
{
	return GameStateData_SetAndInit(&gs->_mData, ctype, entity);
}
void GameState_Init(GameState* gs, ComponentType ctype, Entity entity)
{
	GameStateData_Init(&gs->_mData, ctype, entity);
}
void GameState_Unset(GameState* gs, ComponentType ctype, Entity entity)
{
	GameStateData_Unset(&gs->_mData, ctype, entity);
}
void GameState_UnsetAll(GameState* gs, ComponentType ctype)
{
	GameStateData_UnsetAll(&gs->_mData, ctype);
}
bool GameState_IsAnyEntityInPack(GameState* gs, ComponentType ctype)
{
	return GameStateData_IsAnyEntityInPack(&gs->_mData, ctype);
}
void* GameState_GetFirstSetComponent(GameState* gs, ComponentType ctype)
{
	return GameStateData_GetFirstSetComponent(&gs->_mData, ctype);
}
Entity GameState_GetFirstSetEntity(GameState* gs, ComponentType ctype)
{
	return GameStateData_GetFirstSetEntity(&gs->_mData, ctype);
}
void* GameState_TryGetComponent(GameState* gs, ComponentType ctype, Entity entity, bool* wasSuccessful)
{
	return GameStateData_TryGetComponent(&gs->_mData, ctype, entity, wasSuccessful);
}
void* GameState_TryGetFirstSetComponent(GameState* gs, ComponentType ctype, bool* wasSuccessful)
{
	return GameStateData_TryGetFirstSetComponent(&gs->_mData, ctype, wasSuccessful);
}
#if EDITOR_MODE
bool GameState_HandleDebugRewindLooping(GameState* gs)
{
	bool rewindLoopJustStarted = false;
	if (Input_IsKeyTapped(KEYS_F5))
	{
		gs->_mIsRewindLooping = !gs->_mIsRewindLooping;
		if (gs->_mIsRewindLooping)
		{
			rewindLoopJustStarted = true;
		}
		else
		{
			GameState_ClearReplayCache(gs);
		}
	}

	if (!gs->_mIsRewindLooping)
	{
		return false;
	}

	//We are rewind looping now...
	gs->_mIsRewinding = true;

	ReplayDataManager_RewindLoop(gs->_mReplayDataManager, rewindLoopJustStarted, &gs->_mData);

	return true;
}
bool GameState_HandleDebugSaveStates(GameState* gs)
{
	if (Input_GetPlayerOneAction(ACTIONLIST_GAME_LS)->mIsTapped)
	{
		GameState_CreateDebugSaveState(gs);
	}

	if (Input_GetPlayerOneAction(ACTIONLIST_GAME_RS)->mIsTapped)
	{
		GameState_UseDebugSaveState(gs);
		return true;
	}

	return false;
}
void GameState_CreateDebugSaveState(GameState* gs)
{
	gs->_mHasSavedDebugSaveState = true;
	GameStateData_CopyTo(&gs->_mData, &gs->_mDebugSaveState);
	Logger_LogInformation("Debug Quicksave");
}
void GameState_UseDebugSaveState(GameState* gs)
{
	if (!gs->_mHasSavedDebugSaveState)
	{
		return;
	}
	GameStateData_CopyTo(&gs->_mDebugSaveState, &gs->_mData);
	Logger_LogInformation("Debug Quickload");
}
#endif