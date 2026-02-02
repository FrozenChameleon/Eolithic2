/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "../../game/ComponentType.h"

typedef struct SpriteBatch SpriteBatch;
typedef struct GameState GameState;
typedef struct System System;

typedef void (*System_InitStringSettingsHereFunc)(void* givenData);
typedef void (*System_InitFunc)(void* givenData, ComponentType givenType, Entity owner);
typedef void (*System_UpdateFunc)(void* givenData);
typedef void (*System_DrawFunc)(void* givenData, SpriteBatch* spriteBatch);
typedef void (*System_UpdateLastRenderPositionFunc)(void* givenData, GameState* gameState);
typedef void (*System_ReceiveBroadcastFunc)(void* givenData, int32_t broadcastType, int32_t packet1, int32_t packet2, int32_t packet3);

typedef struct System
{
	void* _mData;
	System_InitStringSettingsHereFunc _mInitStringSettingsHere;
	System_InitFunc _mInit;
	System_UpdateFunc _mUpdate;
	System_DrawFunc _mDraw;
	System_DrawFunc _mDrawHud;
	System_DrawFunc _mDrawDebugHud;
	System_UpdateLastRenderPositionFunc _mUpdateLastRenderPosition;
	System_ReceiveBroadcastFunc _mReceiveBroadcast;
} System;

System* System_Create(void);