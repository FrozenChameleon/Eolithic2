/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "SysRemoveCompleteEntities.h"

#include "../core/Func.h"
#include "../components/TagIsComplete.h"
#include "../gamestate/ComponentPack.h"
#include "../gamestate/GameStateManager.h"
#include "../gamestate/GameState.h"

static void RemoveCompleteEntities(void)
{
	ComponentPack* pack = Get_ComponentPack(C_TagIsComplete);

	PackIterator iter = PackIterator_Begin;
	while (ComponentPack_Next(pack, &iter))
	{
		GameState_RemoveEntity(GameStateManager_GetGameState(), iter.mEntity);
	}
}

static void Update(void* givenData)
{
	RemoveCompleteEntities();
}

System* SysRemoveCompleteEntities_CreateSystem(void)
{
	System* sys = System_Create();
	sys->_mUpdate = Update;
	return sys;
}
