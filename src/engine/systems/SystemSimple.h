/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "System.h"

typedef struct Body Body;

typedef bool (*SystemSimple_BeforeCollideWithBodyFunc)(Body* myBody, Body* otherBody, int32_t myDirectionX, int32_t myDirectionY, int32_t otherDirectionX, int32_t otherDirectionY, bool isVertical);
typedef void (*SystemSimple_CollideWithBodyFunc)(Body* myBody, Body* otherBody, int32_t myDirectionX, int32_t myDirectionY, int32_t otherDirectionX, int32_t otherDirectionY, bool isVertical);
typedef bool (*SystemSimple_CollideWithCollisionFunc)(Body* myBody, int32_t x, int32_t y, int32_t width, int32_t height, int32_t myDirectionX, int32_t myDirectionY, int32_t collisionBit, bool isVertical);
typedef void (*SystemSimple_BeforeUpdateFunc)(void);
typedef void (*SystemSimple_AfterUpdateFunc)(void);
typedef void (*SystemSimple_DrawFunc)(Entity owner, void* data, SpriteBatch* spriteBatch);
typedef void (*SystemSimple_JustCompletedFunc)(Entity owner, void* data);
typedef void (*SystemSimple_UpdateLastRenderPositionFunc)(void* data);
typedef void (*SystemSimple_ReceiveBroadcastFunc)(Entity owner, void* data, int32_t broadcastType, int32_t packet1, int32_t packet2, int32_t packet3);
typedef void (*SystemSimple_UpdateFunc)(Entity entity, void* data);
typedef void (*SystemSimple_InitFunc)(Entity owner, void* data);
typedef void (*SystemSimple_SetupSystemFunc)(void);

typedef struct SystemSimple
{
	ComponentType _mType;
	bool _mHasBeenInit;
	SystemSimple_BeforeCollideWithBodyFunc _mBeforeCollideWithBody;
	SystemSimple_CollideWithBodyFunc _mCollideWithBody;
	SystemSimple_CollideWithCollisionFunc _mCollideWithCollision;
	SystemSimple_BeforeUpdateFunc _mBeforeUpdateRoutine;
	SystemSimple_AfterUpdateFunc _mAfterUpdateRoutine;
	SystemSimple_DrawFunc _mDrawRoutine;
	SystemSimple_DrawFunc _mDrawHudRoutine;
	SystemSimple_DrawFunc _mDrawDebugHudRoutine;
	SystemSimple_JustCompletedFunc _mJustCompletedRoutine;
	SystemSimple_UpdateLastRenderPositionFunc _mUpdateLastRenderPositionRoutine;
	SystemSimple_ReceiveBroadcastFunc _mReceiveBroadcastRoutine;
	SystemSimple_UpdateFunc _mUpdateRoutine;
	SystemSimple_InitFunc _mInitRoutine;
	SystemSimple_SetupSystemFunc _mSetupSystem;
} SystemSimple;

SystemSimple* SystemSimple_Create(ComponentType ctype);
System* SystemSimple_CreateSystem(SystemSimple* ss);