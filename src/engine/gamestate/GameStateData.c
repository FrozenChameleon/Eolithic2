/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GameStateData.h"

#include "../utils/Logger.h"
#include "../utils/Utils.h"
#include "GameStateDataHelper.h"
#include "GameStateManager.h"

static const int32_t ENTITY_COUNTER_MAX = 2000000000;

void GameStateData_Ctor(GameStateData* gsd)
{
	Utils_memset(gsd, 0, sizeof(GameStateData));

	GameStateDataHelper_InitAllComponentPacks(gsd);
}
ComponentPack* GameStateData_GetComponentPack(GameStateData* gsd, ComponentType ctype)
{
	return &gsd->mComponentPacks[ctype];
}
void GameStateData_Reset(GameStateData* gsd)
{
	gsd->mEntityCounter = GAME_STATE_DATA_INITIAL_ENTITY_NUMBER;
	for (int32_t i = 0; i < COMPONENT_TYPE_LEN; i += 1)
	{
		ComponentPack_UnsetAll(&gsd->mComponentPacks[i]);
	}
}
Entity GameStateData_GetEntityInPlay(GameStateData* gsd, Entity entityNumber)
{
	ComponentPack* pack = GameStateData_GetComponentPack(gsd, C_TagIsInPlay);
	int32_t loc = ComponentPack_GetEntityLocation(pack, entityNumber);
	if (loc == -1)
	{
		return ENTITY_NOTHING;
	}
	return pack->Entities[loc];
}
void GameStateData_RemoveEntity(GameStateData* gsd, Entity entity)
{
	for (int32_t i = 0; i < COMPONENT_TYPE_LEN; i += 1)
	{
		ComponentPack_Unset(&gsd->mComponentPacks[i], entity);
	}
}
void GameStateData_FillListWithEntitiesInPlay(GameStateData* gsd, EntitySearch* list)
{
	ComponentPack* pack = GameStateData_GetComponentPack(gsd, C_TagIsInPlay);
	if (list->cap < pack->_mLength)
	{
		Logger_LogWarning("Not enough room to fill list with entities in play.");
		return;
	}

	int32_t counter = 0;
	PackIterator iter = PackIterator_Begin;
	while (ComponentPack_Next(pack, &iter))
	{
		list->entities[counter] = iter.mEntity;
		counter += 1;
	}

	list->len = counter;
}
int32_t GameStateData_GetAmountOfEntitiesInPlay(GameStateData* gsd)
{
	ComponentPack* pack = GameStateData_GetComponentPack(gsd, C_TagIsInPlay);
	return pack->_mLength;
}
Entity GameStateData_BuildNewEntity(GameStateData* gsd)
{
	ComponentPack* pack = GameStateData_GetComponentPack(gsd, C_TagIsInPlay);
	Entity newEntity = gsd->mEntityCounter;
	ComponentPack_Set(pack, newEntity);
	gsd->mEntityCounter += 1;
	if ((gsd->mEntityCounter >= ENTITY_COUNTER_MAX) || (gsd->mEntityCounter < 0))
	{
		gsd->mEntityCounter = GAME_STATE_DATA_INITIAL_ENTITY_NUMBER;
	}
	return newEntity;
}
void GameStateData_CopyTo(GameStateData* from, GameStateData* to)
{
	to->mEntityCounter = from->mEntityCounter;
	for (int32_t i = 0; i < COMPONENT_TYPE_LEN; i += 1)
	{
		ComponentPack_CopyTo(&from->mComponentPacks[i], &to->mComponentPacks[i]);
	}
}
bool GameStateData_IsAnyEntityInPack(GameStateData* gsd, ComponentType ctype)
{
	ComponentPack* pack = GameStateData_GetComponentPack(gsd, ctype);
	return ComponentPack_IsAnyEntityInPack(pack);
}
void* GameStateData_GetFirstSetComponent(GameStateData* gsd, ComponentType ctype)
{
	ComponentPack* pack = GameStateData_GetComponentPack(gsd, ctype);
	return ComponentPack_GetFirstSetComponent(pack);
}
Entity GameStateData_GetFirstSetEntity(GameStateData* gsd, ComponentType ctype)
{
	ComponentPack* pack = GameStateData_GetComponentPack(gsd, ctype);
	return ComponentPack_GetFirstSetEntity(pack);
}
bool GameStateData_HasComponent(GameStateData* gsd, ComponentType ctype, Entity entity)
{
	ComponentPack* pack = GameStateData_GetComponentPack(gsd, ctype);
	return ComponentPack_HasComponent(pack, entity);
}
void* GameStateData_Set(GameStateData* gsd, ComponentType ctype, Entity entity)
{
	ComponentPack* pack = GameStateData_GetComponentPack(gsd, ctype);
	return ComponentPack_Set(pack, entity);
}
void* GameStateData_SetAndInit(GameStateData* gsd, ComponentType ctype, Entity entity)
{
	void* component = GameStateData_Set(gsd, ctype, entity);
	if (entity != ENTITY_NOTHING)
	{
		GameStateData_Init(gsd, ctype, entity);
	}
	return component;
}
void GameStateData_Init(GameStateData* gsd, ComponentType ctype, Entity entity)
{
	int32_t len = GameStateManager_GetStateSystemsLength();
	System** stateSystems = GameStateManager_GetStateSystems();
	for (int32_t i = 0; i < len; i += 1)
	{
		System* stateSys = stateSystems[i];
		stateSys->_mInit(stateSys->_mData, ctype, entity);
	}
}
void GameStateData_Unset(GameStateData* gsd, ComponentType ctype, Entity entity)
{
	ComponentPack* pack = GameStateData_GetComponentPack(gsd, ctype);
	ComponentPack_Unset(pack, entity);
}
void GameStateData_UnsetAll(GameStateData* gsd, ComponentType ctype)
{
	ComponentPack* pack = GameStateData_GetComponentPack(gsd, ctype);
	ComponentPack_UnsetAll(pack);
}
void* GameStateData_TryGetFirstSetComponent(GameStateData* gsd, ComponentType ctype, bool* wasSuccessful)
{
	ComponentPack* pack = GameStateData_GetComponentPack(gsd, ctype);
	return ComponentPack_TryGetFirstSetComponent(pack, wasSuccessful);
}
void* GameStateData_TryGetComponent(GameStateData* gsd, ComponentType ctype, Entity entity, bool* wasSuccessful)
{
	ComponentPack* pack = GameStateData_GetComponentPack(gsd, ctype);
	return ComponentPack_TryGetComponent(pack, entity, wasSuccessful);
}