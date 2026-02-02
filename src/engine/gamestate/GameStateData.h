/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "../../game/ComponentType.h"
#include "ComponentPack.h"
#include "EntitySearch.h"

#define GAME_STATE_DATA_INITIAL_ENTITY_NUMBER 1

typedef struct GameStateData
{
	int32_t mEntityCounter;
	ComponentPack mComponentPacks[COMPONENT_TYPE_LEN];
} GameStateData;

void GameStateData_Ctor(GameStateData* gsd);
ComponentPack* GameStateData_GetComponentPack(GameStateData* gsd, ComponentType ctype);
void GameStateData_Reset(GameStateData* gsd);
Entity GameStateData_GetEntityInPlay(GameStateData* gsd, Entity entityNumber);
void GameStateData_RemoveEntity(GameStateData* gsd, Entity entity);
void GameStateData_FillListWithEntitiesInPlay(GameStateData* gsd, EntitySearch* list);
int32_t GameStateData_GetAmountOfEntitiesInPlay(GameStateData* gsd);
Entity GameStateData_BuildNewEntity(GameStateData* gsd);
void GameStateData_CopyTo(GameStateData* from, GameStateData* to);
bool GameStateData_IsAnyEntityInPack(GameStateData* gsd, ComponentType ctype);
void* GameStateData_GetFirstSetComponent(GameStateData* gsd, ComponentType ctype);
Entity GameStateData_GetFirstSetEntity(GameStateData* gsd, ComponentType ctype);
bool GameStateData_HasComponent(GameStateData* gsd, ComponentType ctype, Entity entity);
void* GameStateData_Set(GameStateData* gsd, ComponentType ctype, Entity entity);
void* GameStateData_SetAndInit(GameStateData* gsd, ComponentType ctype, Entity entity);
void GameStateData_Init(GameStateData* gsd, ComponentType ctype, Entity entity);
void GameStateData_Unset(GameStateData* gsd, ComponentType ctype, Entity entity);
void GameStateData_UnsetAll(GameStateData* gsd, ComponentType ctype);
void* GameStateData_TryGetFirstSetComponent(GameStateData* gsd, ComponentType ctype, bool* wasSuccessful);
void* GameStateData_TryGetComponent(GameStateData* gsd, ComponentType ctype, Entity entity, bool* wasSuccessful);