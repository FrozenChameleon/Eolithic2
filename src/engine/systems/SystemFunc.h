/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../../game/ComponentType.h"
#include "../gamestate/GameStateManager.h"

typedef struct SpriteBatch SpriteBatch;

typedef void (*SystemFunc_InitHelperFunc)(Entity owner, void* data);
typedef void (*SystemFunc_UpdateHelperFunc)(Entity owner, void* data);
typedef void (*SystemFunc_DrawHelperFunc)(Entity owner, void* data, SpriteBatch* spriteBatch);
typedef void (*SystemFunc_UpdateLastRenderPositionFunc)(void* data);
typedef void (*SystemFunc_ReceiveBroadcastHelperFunc)(Entity owner, void* data, int32_t broadcastType, int32_t packet1, int32_t packet2, int32_t packet3);

void SystemFunc_InitHelper(ComponentType ctype, ComponentType givenType, SystemFunc_InitHelperFunc initRoutine, Entity owner);
void SystemFunc_UpdateHelper(ComponentType ctype, SystemFunc_UpdateHelperFunc updateRoutine);
void SystemFunc_DrawHelper(ComponentType ctype, SystemFunc_DrawHelperFunc drawRoutine, SpriteBatch* spriteBatch);
void SystemFunc_DrawHudHelper(ComponentType ctype, SystemFunc_DrawHelperFunc drawHudRoutine, SpriteBatch* spriteBatch);
void SystemFunc_DrawDebugHudHelper(ComponentType ctype, SystemFunc_DrawHelperFunc drawDebugHudRoutine, SpriteBatch* spriteBatch);
void SystemFunc_UpdateLastRenderPositionHelper(ComponentType ctype, GameState* gameState, SystemFunc_UpdateLastRenderPositionFunc updateLastRenderPositionsRoutine);
void SystemFunc_ReceiveBroadcastHelper(ComponentType ctype, SystemFunc_ReceiveBroadcastHelperFunc receiveBroadcastRoutine, 
	int32_t broadcastType, int32_t packet1, int32_t packet2, int32_t packet3);