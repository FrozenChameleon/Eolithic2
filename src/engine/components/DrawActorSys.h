/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../leveldata/ThingSettings.h"
#include "../systems/SystemSimple.h"
#include "DrawActor.h"
#include "../leveldata/ImageData.h"
#include "../render/DrawStateInfo.h"
#include "../render/DrawRenderInfo.h"
#include "../gamestate/ComponentPack.h"
#include "limits.h"

typedef struct Animation Animation;
typedef struct ImageDataInstance ImageDataInstance;

enum
{
	DRAWACTORSYS_NOTHING = INT_MIN
};

void DrawActorSys_Setup(Entity owner, DrawActor* data, ThingGraphicsData* sh_graphics_data, const char* defaultDrawState, const char* defaultDrawPhase);
void DrawActorSys_InitRoutine(Entity owner, DrawActor* data);
void DrawActorSys_UpdateLastRenderPositionRoutine(DrawActor* data);
void DrawActorSys_BeforeUpdateRoutine(void);
void DrawActorSys_UpdateRoutine(Entity owner, DrawActor* data);
DrawStateInfo* DrawActorSys_GetStateInfo(Entity owner, int32_t state);
void DrawActorSys_SetDepthOverride(Entity owner, int32_t state, int32_t value);
void DrawActorSys_SetImageDataDepth(Entity owner, int32_t state, int32_t phase, int32_t depth);
void DrawActorSys_SetImageState(Entity owner, DrawActor* data, int32_t state, int32_t phase);
void DrawActorSys_SetImageState2(Entity owner, DrawActor* data, int32_t state, int32_t newPhase, bool carryAnimationState);
void DrawActorSys_SetStateRotation(Entity owner, int32_t state, float rotation);
int32_t DrawActorSys_GetCurrentPhase(Entity owner, int32_t state);
void DrawActorSys_SetNudge(Entity owner, int32_t state, float x, float y);
ImageDataInstance* DrawActorSys_GetRender(Entity owner, int32_t state, int32_t phase, int32_t index);
int32_t DrawActorSys_GetAnimationPosition(Entity owner, int32_t state, int32_t phase);
Animation* DrawActorSys_GetCurrentAnimation(Entity owner, int32_t state);
Animation* DrawActorSys_GetAnimation(Entity owner, int32_t state, int32_t phase);
ImageDataInstance* DrawActorSys_GetCurrentImageDataRender(Entity owner, int32_t state, int32_t phase);
void DrawActorSys_ResetCurrentAnimation(Entity owner, int32_t state);
void DrawActorSys_SetCurrentAnimationPosition(Entity owner, int32_t state, int32_t position);
void DrawActorSys_SetAnimationPosition(Entity owner, int32_t state, int32_t phase, int32_t position);
void DrawActorSys_SetShaderProgram(Entity owner, int32_t state, ShaderProgram* shaderProgram);
void DrawActorSys_UpdateRenders(Entity owner, DrawActor* data, ComponentPack* stateInfos, ComponentPack* renders);
void DrawActorSys_DrawRoutine(Entity owner, DrawActor* data, SpriteBatch* spriteBatch);
void DrawActorSys_DrawInterpolated(Entity owner, DrawActor* data, SpriteBatch* spriteBatch, Color color, Vector2 currentPosition, Vector2 lastPosition,
	float rotation, Vector2 scale, int32_t givenDepth);
System* DrawActorSys_CreateSystem(void);