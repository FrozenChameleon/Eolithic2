/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "../input/InputAction.h"
#include "../resources/ResourceManager.h"
#include "../utils/StringPair.h"
#include "../systems/System.h"
#include "../collision/Body.h"
#include "../collision/CollisionCheckData.h"
#include "../components/Camera.h"
#include "../gamestate/GameStateData.h"

typedef struct IntIntPair
{
	int32_t key;
	int32_t value;
} IntIntPair;

typedef struct CharIntPair
{
	char* key;
	int32_t value;
} CharIntPair;

typedef struct SpriteBatch SpriteBatch;
typedef struct LevelData LevelData;
typedef struct BufferWriter BufferWriter;
typedef struct BufferReader BufferReader;
typedef struct ThingInstance ThingInstance;
typedef struct IStringArray IStringArray;
typedef struct DynamicByteBuffer DynamicByteBuffer;

//supposed to be protected
Resource* GameHelper_GetDefaultLevelDataResource(void);
LevelData* GameHelper_GetDefaultLevelData(void);
void GameHelper_UpdateLastRenderPositionNormally(void);
Camera* GameHelper_GetDefaultCameraForRender(void);
float GameHelper_GetDefaultPostGameBrightness(void);
void GameHelper_UpdateGlobalSystemsNormally(void);
void GameHelper_DrawGlobalSystemsNormally(SpriteBatch* spriteBatch);
void GameHelper_DrawHudGlobalSystemsNormally(SpriteBatch* spriteBatch);
void GameHelper_DrawDebugHudGlobalSystemsNormally(SpriteBatch* spriteBatch);
void GameHelper_UpdateStateSystemsNormally(void);
void GameHelper_DrawStateSystemsNormally(SpriteBatch* spriteBatch);
void GameHelper_DrawHudStateSystemsNormally(SpriteBatch* spriteBatch);
void GameHelper_DrawDebugHudStateSystemsNormally(SpriteBatch* spriteBatch);
void GameHelper_CreateDefaultGlobalSystems(void);
void GameHelper_CreateDefaultBindings(IStringArray* strings, InputAction* input);
void GameHelper_AddStringsHelper(void);

//supposed to be public
enum
{
	GAMEHELPER_COMPONENT_SCAN_RANGE = 1000
};

extern const float GAMEHELPER_DEFAULT_GRAVITY_DECELERATION;
extern const float GAMEHELPER_DEFAULT_GRAVITY_DECELERATION_MAX;
extern const int32_t GAMEHELPER_DEFAULT_MOVING_PLATFORM_LEEWAY;

extern int32_t GAMEHELPER_PLATFORM_UP;
extern int32_t GAMEHELPER_PLATFORM_DOWN;
extern int32_t GAMEHELPER_PLATFORM_RIGHT;
extern int32_t GAMEHELPER_PLATFORM_LEFT;

#ifdef EDITOR_MODE
void GameHelper_OnDebugFastResetPlusMove(void);
#endif
void GameHelper_Initialize(void);
void GameHelper_OnLoadingStart(void);
void GameHelper_OnLoadingAfterResources(void);
void GameHelper_WriteSessionData(BufferWriter* writer);
void GameHelper_ReadSessionData(BufferReader* reader);
Resource* GameHelper_GetLevelDataResource(void);
LevelData* GameHelper_GetLevelData(void);
void GameHelper_UpdateLastRenderPosition(void);
Camera* GameHelper_GetCameraForRender(void);
void GameHelper_BuildAchievementList(void);
void GameHelper_OnNormalGameStateChange(void);
const char* GameHelper_DebugGiveNextLevel(void);
float GameHelper_GetMovingPlatformLeeway(void);
float GameHelper_GetPostGameBrightness(void);
void GameHelper_UpdateGlobalSystems(void);
void GameHelper_DrawGlobalSystems(SpriteBatch* spriteBatch);
void GameHelper_DrawHudGlobalSystems(SpriteBatch* spriteBatch);
void GameHelper_DrawDebugHudGlobalSystems(SpriteBatch* spriteBatch);
bool GameHelper_HandleRewindBeforeUpdatingStateSystems(void);
void GameHelper_UpdateStateSystems(void);
void GameHelper_DrawStateSystems(SpriteBatch* spriteBatch);
void GameHelper_DrawHudStateSystems(SpriteBatch* spriteBatch);
void GameHelper_DrawDebugHudStateSystems(SpriteBatch* spriteBatch);
void GameHelper_CreateGameStates(void);
void GameHelper_InitGameStateData(const char* name, GameStateData* initThis);
void* GameHelper_CreateGameSaveData(void);
void GameHelper_InitGameState(const char* levelToLoad);
void GameHelper_CreateStateSystems(void);
void GameHelper_CreateGlobalSystems(void);
void GameHelper_InitPoolsForGame(void);
float GameHelper_GetGravityDecelerationMax(void);
float GameHelper_GetGravityDeceleration(void);
ComponentType GameHelper_BuildControllerComponent(int32_t number, Entity entity);
void GameHelper_SetupPlatformTypes(void);
bool GameHelper_IsCollisionSolidForLineOfSight(int32_t collision);
bool GameHelper_IsCollisionSolidForPathFinding(int32_t collision, int32_t currentX, int32_t currentY, int32_t newX, int32_t newY);
void GameHelper_DefaultBindings(IStringArray* strings, InputAction* input);
void GameHelper_SetupMusicAndSfxList(StringPair* music, StringPair* sfx);
const char* GameHelper_ReturnInputDisplayMask(const char* displayName);
void GameHelper_AddStrings(void);
void GameHelper_AddDefaultThingSettings(StringPair* pairs);
IStringArray* GameHelper_GetAllTheRecordings(void);
void GameHelper_BakedCollisionCheck(float x, float y, int32_t collisionToCheck, Body* bodyRef, bool vertical, CollisionCheckData* data);
void GameHelper_WriteGameSaveData(void* gsd, DynamicByteBuffer* writer);
void GameHelper_ReadGameSaveData(void* gsd, BufferReader* reader);