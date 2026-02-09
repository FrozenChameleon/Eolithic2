/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "GameState.h"
#include "../systems/System.h"

typedef struct InputPlayer InputPlayer;
typedef struct SpriteBatch SpriteBatch;

enum GameStateManagerDebugInfo
{
	GAMESTATEMANAGER_DEBUG_INFO_SHOW_NOTHING = 0,
	GAMESTATEMANAGER_DEBUG_INFO_SHOW_QUICK_STATS = 1,
	GAMESTATEMANAGER_DEBUG_INFO_SHOW_THING_VIEWER = 2
};

enum GameStateManagerGameState
{
	GAMESTATEMANAGER_GAME_STATE_NORMAL = 0,
	GAMESTATEMANAGER_GAME_STATE_PAUSED = 1
};

void GameStateManager_Ctor(void);
int32_t GameStateManager_GetGlobalSystemsLength(void);
System** GameStateManager_GetGlobalSystems(void);
void GameStateManager_AddGlobalSystem(System* sys);
int32_t GameStateManager_GetStateSystemsLength(void);
System** GameStateManager_GetStateSystems(void);
void GameStateManager_AddStateSystem(System* sys);
GameState* GameStateManager_GetGameState(void);
void GameStateManager_UpdateLastRenderPosition(void);
void GameStateManager_Tick(void);
void GameStateManager_Draw(SpriteBatch* spriteBatch);
void GameStateManager_DrawHud(SpriteBatch* spriteBatch);
void GameStateManager_DrawDebugHud(SpriteBatch* spriteBatch);
void GameStateManager_InitDefaultNormalState(void);
void GameStateManager_SetGameState(int32_t value);
void GameStateManager_SetGameState2(int32_t value, bool forceNow);
void GameStateManager_DebugDrawInfoHelper(int32_t* counter, SpriteBatch* spriteBatch, const char* text);
Camera* GameStateManager_GetCurrentRenderCamera(void);
void GameStateManager_DebugForceLoadMapNow(const char* map);
void GameStateManager_SetupLoadMap(const char* s);
void GameStateManager_SetupReloadMap(void);
const char* GameStateManager_GetCurrentFileName(void);
void GameStateManager_DebugForceReloadMapNow(void);
void GameStateManager_LoadMap(const char* mapToLoad);
bool GameStateManager_IsNormalState(void);
bool GameStateManager_IsPausedState(void);
void GameStateManager_HandleGameStateChange(void);
void GameStateManager_HandleLoadNextMap(void);
bool GameStateManager_JustChangedGameStateThisFrame(void);
void GameStateManager_HandleJustChangedGameStateThisFrame(void);
void GameStateManager_SetCurrentGameStateForRenderCamera(int32_t value);
int32_t GameStateManager_GetCurrentGameState(void);
void GameStateManager_SetCurrentGameState(int32_t value);
void GameStateManager_IncrementTicksSinceMapLoad(void);
uint64_t GameStateManager_GetTicksSinceMapLoad(void);
int32_t GameStateManager_GetUniqueMapSeed(void);
void GameStateManager_SetUniqueMapSeed(int32_t value);
ComponentPack* GameStateManager_GetComponentPack(ComponentType ctype);
bool GameStateManager_HasComponent(ComponentType ctype, Entity entity);
void* GameStateManager_GetComponent(ComponentType ctype, Entity entity); //Just another name for Set
void* GameStateManager_Set(ComponentType ctype, Entity entity);
void* GameStateManager_SetAndInit(ComponentType ctype, Entity entity);
void GameStateManager_Init(ComponentType ctype, Entity entity);
void GameStateManager_Unset(ComponentType ctype, Entity entity);
void GameStateManager_UnsetAll(ComponentType ctype);
bool GameStateManager_IsAnyEntityInPack(ComponentType ctype);
void* GameStateManager_GetFirstSetComponent(ComponentType ctype);
Entity GameStateManager_GetFirstSetEntity(ComponentType ctype);
void* GameStateManager_TryGetComponent(ComponentType ctype, Entity entity, bool* wasSuccessful);
void* GameStateManager_TryGetFirstSetComponent(ComponentType ctype, bool* wasSuccessful);

#ifdef EDITOR_MODE
void GameStateManager_SaveComponentSizes(void);
#endif
