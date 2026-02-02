/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "EntitySearch.h"

#include "../utils/Utils.h"
#include "../core/Func.h"
#include "../gamestate/GameStateManager.h"
#include "../gamestate/GameState.h"
#include "../utils/Logger.h"

void EntitySearch_AddEntityToSearch(EntitySearch* search, Entity addThis)
{
	for (int32_t i = 0; i < search->cap; i += 1)
	{
		if (search->entities[i] == ENTITY_NOTHING)
		{
			search->entities[i] = addThis;
			search->len += 1;
			return;
		}
	}

	Logger_LogWarning("Unable to add entity to search!");
}
bool EntitySearch_Check(bool isValid, bool isReverse)
{
	if ((isValid && !isReverse) || (!isValid && isReverse))
	{
		return true;
	}
	return false;
}
EntitySearch* EntitySearch_CreateNewBlankSearchSetCapacity(int32_t cap)
{
	EntitySearch* search = (EntitySearch*)Utils_CallocArena(1, sizeof(EntitySearch), UTILS_ALLOCATION_ARENA_JUST_THIS_FRAME);
	search->entities = (Entity*)Utils_CallocArena(cap, sizeof(Entity), UTILS_ALLOCATION_ARENA_JUST_THIS_FRAME);
	search->cap = cap;
	search->len = 0;
	return search;
}
EntitySearch* EntitySearch_CreateNewBlankSearch(void)
{
	GameState* activeGameState = Get_ActiveGameState();
	int32_t amountOfEntitiesInPlay = GameState_GetAmountOfEntitiesInPlay(activeGameState);
	return EntitySearch_CreateNewBlankSearchSetCapacity(amountOfEntitiesInPlay * 2);
}
EntitySearch* EntitySearch_SearchForEntitiesWithThisName(const EntitySearch* lookThroughThis, EntitySearch* addToThis, const char* name, bool isReverse)
{
	for (int32_t i = 0; i < lookThroughThis->len; i += 1)
	{
		Entity entity = lookThroughThis->entities[i];
		const char* entityName = Get_Name(entity);
		if (EntitySearch_Check(Utils_StringEqualTo(name, entityName), isReverse))
		{
			EntitySearch_AddEntityToSearch(addToThis, entity);
		}
	}
	return addToThis;
}
EntitySearch* EntitySearch_SearchForEntitiesWithThisParentNumber(const EntitySearch* lookThroughThis, EntitySearch* addToThis, int32_t value, bool isReverse)
{
	for (int32_t i = 0; i < lookThroughThis->len; i += 1)
	{
		Entity entity = lookThroughThis->entities[i];
		if (EntitySearch_Check(Get_ParentNumber(entity) == value, isReverse))
		{
			EntitySearch_AddEntityToSearch(addToThis, entity);
		}
	}
	return addToThis;
}
EntitySearch* EntitySearch_SearchForEntitiesWithThisIntTag(const EntitySearch* lookThroughThis, EntitySearch* addToThis, int32_t value, bool isReverse)
{
	for (int32_t i = 0; i < lookThroughThis->len; i += 1)
	{
		Entity entity = lookThroughThis->entities[i];
		if (EntitySearch_Check(Get_IntTag(entity) == value, isReverse))
		{
			EntitySearch_AddEntityToSearch(addToThis, entity);
		}
	}
	return addToThis;
}
EntitySearch* EntitySearch_SearchForEntitiesWithThisComponent(ComponentType ctype, const EntitySearch* lookThroughThis, EntitySearch* addToThis, bool isReverse)
{
	for (int32_t i = 0; i < lookThroughThis->len; i += 1)
	{
		Entity entity = lookThroughThis->entities[i];
		bool hasComponent = GameStateManager_HasComponent(ctype, entity);
		if (EntitySearch_Check(hasComponent, isReverse))
		{
			EntitySearch_AddEntityToSearch(addToThis, entity);
		}
	}
	return addToThis;
}

EntitySearch* EntitySearch_CreateNewAllEntitySearch(void)
{
	GameState* activeGameState = Get_ActiveGameState();
	int32_t amountOfEntitiesInPlay = GameState_GetAmountOfEntitiesInPlay(activeGameState);
	EntitySearch* search = EntitySearch_CreateNewBlankSearchSetCapacity(amountOfEntitiesInPlay);
	GameState_FillListWithEntitiesInPlay(activeGameState, search);
	return search;
}
