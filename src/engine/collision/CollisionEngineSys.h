/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../systems/SystemSimple.h"
#include "../components/CollisionImprintData.h"
#include "CollisionEngine.h"
#include "Body.h"

typedef struct MetaMapChunk MetaMapChunk;
typedef struct LevelData LevelData;

void CollisionEngineSys_ResolveBody(CollisionEngine* data, bool isVertical, Body* body);
void CollisionEngineSys_CollisionHelper(Body* body, int32_t myDirectionX, int32_t myDirectionY, int32_t collisionBit);
void CollisionEngineSys_ResolveWithOtherBodies(CollisionEngine* data, Body* bodyOne, bool isVertical);
bool CollisionEngineSys_ResolvePush(CollisionEngine* data, Body* pushingBody, Body* pushedBody, bool isVertical);
void CollisionEngineSys_HandleNullDividePlusPushing(CollisionEngine* data, Body* pushingBody, Body* pushedBody, bool isVertical);
void CollisionEngineSys_ImprintToCollisionGridHelper(CollisionEngine* data, CollisionImprintData* imprint);
bool CollisionEngineSys_IsPointSafe(CollisionEngine* data, int32_t x, int32_t y);
bool CollisionEngineSys_CheckPoint(CollisionEngine* data, float checkX, float checkY, int32_t directionX, int32_t directionY, Body* body, bool isVertical);

void CollisionEngineSys_DisableDynamicGravity(void);

void CollisionEngineSys_CopyCollisionGridFromPristine(CollisionEngine* data);
void CollisionEngineSys_UpdateRoutine(Entity owner, CollisionEngine* data);
bool CollisionEngineSys_PushBody(CollisionEngine* data, bool isBakedCollision, bool isVertical, Body* body, const Rectangle* otherPhysicsRectangle);
bool CollisionEngineSys_IsCollisionValid(Body* bodyOne, Body* bodyTwo, bool isVertical);
void CollisionEngineSys_UpdateParticleBody(CollisionEngine* data, Body* body);
void CollisionEngineSys_RemoveAllCollisionImprints(void);
void CollisionEngineSys_ImprintToCollisionGrid(CollisionEngine* data, float pixelX, float pixelY, int32_t width, int32_t height, int32_t type);
int32_t CollisionEngineSys_GetCollisionBit(CollisionEngine* data, float x, float y);
int32_t CollisionEngineSys_GetCollisionBitSafe(CollisionEngine* data, float x, float y, int32_t returnValue);
int32_t CollisionEngineSys_GetCollisionBitSafeGrid(CollisionEngine* data, int32_t x, int32_t y, int32_t returnValue);
int32_t CollisionEngineSys_GetCollisionBitGrid(CollisionEngine* data, int32_t x, int32_t y);
void CollisionEngineSys_SetupCollisionGrid(CollisionEngine* data, LevelData* level);
bool CollisionEngineSys_DoPlatformCollision(Body* platformBody, Body* thingBody);
Point CollisionEngineSys_GetCollisionGridPosition(float x, float y);
bool CollisionEngineSys_CheckSurroundingCollision(CollisionEngine* data, int32_t bodyX, int32_t bodyY, int32_t xDirection, int32_t yDirection, 
	const int32_t* collisionToCheck, int32_t collisionToCheckLen);
bool CollisionEngineSys_IsRectIntersectsCollision(CollisionEngine* data, int32_t x, int32_t y, int32_t width, int32_t height);
bool CollisionEngineSys_CheckFeetCollision(CollisionEngine* data, Body* body, bool checkLeft, bool checkRight, int32_t offset, int32_t collisionBit);
bool CollisionEngineSys_HasLineOfSight(CollisionEngine* data, float x1, float y1, float x2, float y2, bool respectOneWays);
bool CollisionEngineSys_HasLineOfSight2(CollisionEngine* data, bool draw, SpriteBatch* spriteBatch, float x1, float y1, float x2, float y2, bool respectOneWays);
int32_t CollisionEngineSys_GetPlatformDown(void);
int32_t CollisionEngineSys_GetPlatformLeft(void);
int32_t CollisionEngineSys_GetPlatformRight(void);
int32_t CollisionEngineSys_GetPlatformUp(void);
float CollisionEngineSys_GetCurrentDeceleration(float maxDecel, int32_t framesInAir);
void CollisionEngineSys_HandleDynamicGravityY(Body* body);
void CollisionEngineSys_HandleBodiesHousekeepingHelper(CollisionEngine* data, Body* body);
void CollisionEngineSys_Step(CollisionEngine* data, bool isVertical);
void CollisionEngineSys_ResolveWithBakedCollision(CollisionEngine* data, Body* body, bool vertical);
void CollisionEngineSys_ApplyBodyVelocity(CollisionEngine* data, Body* body, bool isVertical);
void CollisionEngineSys_DrawRoutine(Entity owner, CollisionEngine* data, SpriteBatch* spriteBatch);
void CollisionEngineSys_DrawTiles(GameState* scene);
void CollisionEngineSys_DebugGenerateDebugRectangles(CollisionEngine* data);

System* CollisionEngineSys_CreateSystem(void);