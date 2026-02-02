/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "SystemFunc.h"

#include "../gamestate/PackIterator.h"
#include "../core/Func.h"

void SystemFunc_InitHelper(ComponentType ctype, ComponentType givenType, SystemFunc_InitHelperFunc initRoutine, Entity owner)
{
	if (ctype != givenType)
	{
		return;
	}

	initRoutine(owner, GameStateManager_GetComponent(ctype, owner));
}
void SystemFunc_UpdateHelper(ComponentType ctype, SystemFunc_UpdateHelperFunc updateRoutine)
{
	ComponentPack* pack = GameStateManager_GetComponentPack(ctype);
	if (!ComponentPack_IsAnyEntityInPack(pack))
	{
		return;
	}

	PackIterator iter = PackIterator_Begin;
	while (ComponentPack_Next(pack, &iter))
	{
		if (!Is_UpdateDisabled(iter.mEntity))
		{
			updateRoutine(iter.mEntity, iter.mComponent);
		}
	}
}
void SystemFunc_DrawHelper(ComponentType ctype, SystemFunc_DrawHelperFunc drawRoutine, SpriteBatch* spriteBatch)
{
	ComponentPack* pack = GameStateManager_GetComponentPack(ctype);
	if (!ComponentPack_IsAnyEntityInPack(pack))
	{
		return;
	}

	PackIterator iter = PackIterator_Begin;
	while (ComponentPack_Next(pack, &iter))
	{
		if (!Is_DrawDisabled(iter.mEntity))
		{
			drawRoutine(iter.mEntity, iter.mComponent, spriteBatch);
		}
	}
}
void SystemFunc_DrawHudHelper(ComponentType ctype, SystemFunc_DrawHelperFunc drawHudRoutine, SpriteBatch* spriteBatch)
{
	ComponentPack* pack = GameStateManager_GetComponentPack(ctype);
	if (!ComponentPack_IsAnyEntityInPack(pack))
	{
		return;
	}

	PackIterator iter = PackIterator_Begin;
	while (ComponentPack_Next(pack, &iter))
	{
		drawHudRoutine(iter.mEntity, iter.mComponent, spriteBatch);
	}
}
void SystemFunc_DrawDebugHudHelper(ComponentType ctype, SystemFunc_DrawHelperFunc drawDebugHudRoutine, SpriteBatch* spriteBatch)
{
	ComponentPack* pack = GameStateManager_GetComponentPack(ctype);
	if (!ComponentPack_IsAnyEntityInPack(pack))
	{
		return;
	}

	PackIterator iter = PackIterator_Begin;
	while (ComponentPack_Next(pack, &iter))
	{
		drawDebugHudRoutine(iter.mEntity, iter.mComponent, spriteBatch);
	}
}
void SystemFunc_UpdateLastRenderPositionHelper(ComponentType ctype, GameState* gameState, SystemFunc_UpdateLastRenderPositionFunc updateLastRenderPositionsRoutine)
{
	ComponentPack* pack = GameStateManager_GetComponentPack(ctype);
	if (!ComponentPack_IsAnyEntityInPack(pack))
	{
		return;
	}

	PackIterator iter = PackIterator_Begin;
	while (ComponentPack_Next(pack, &iter))
	{
		updateLastRenderPositionsRoutine(iter.mComponent);
	}
}
void SystemFunc_ReceiveBroadcastHelper(ComponentType ctype, SystemFunc_ReceiveBroadcastHelperFunc receiveBroadcastRoutine, 
	int32_t broadcastType, int32_t packet1, int32_t packet2, int32_t packet3)
{
	ComponentPack* pack = GameStateManager_GetComponentPack(ctype);
	if (!ComponentPack_IsAnyEntityInPack(pack))
	{
		return;
	}

	PackIterator iter = PackIterator_Begin;
	while (ComponentPack_Next(pack, &iter))
	{
		receiveBroadcastRoutine(iter.mEntity, iter.mComponent, broadcastType, packet1, packet2, packet3);
	}
}