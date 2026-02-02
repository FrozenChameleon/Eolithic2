/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"
#include "../math/Vector2.h"
#include "../math/Random32.h"

enum RecordingVersion
{
	GLOBALS_RECORDING_VERSION_0_ORIGINAL = 0,
	GLOBALS_RECORDING_VERSION_1_SUCCESS_NUMBER_FIX = 1,
	GLOBALS_RECORDING_VERSION_2_CS_THING_ID_FIX = 2
};

#define GLOBALS_DEFAULT_DEBUG_GAME_LOGIC_SPEED 100

extern Vector2 GLOBALS_DEBUG_QUICK_PLAYER_POSITION;
extern int32_t GLOBALS_DEBUG_SHOW_INFO;
extern int32_t GLOBALS_DEBUG_GAME_LOGIC_SPEED;
extern bool GLOBALS_DEBUG_SHOW_INGAME_COLLISION;
extern bool GLOBALS_DEBUG_DISABLE_HUD;
extern bool GLOBALS_DEBUG_IS_GOD_MODE;
extern bool GLOBALS_DEBUG_IS_PAUSED;
extern bool GLOBALS_DEBUG_EDITOR_JUST_RELOADED_GRAPHICS;
extern bool GLOBALS_DEBUG_JUST_LOADED_MAP_NOTIFY_EDITOR;
extern bool GLOBALS_DEBUG_IS_EDITOR_MODE;
extern bool GLOBALS_DEBUG_IS_META_MAP_EDIT_TILE_MODE_AT_MAP_LOAD;
extern bool GLOBALS_DEBUG_ENGINE_FORCE_LOAD_DATS;

bool Globals_DebugIsRenderDisabled(void);
void Globals_DebugSetRenderDisabled(bool value);
bool Globals_IsAutoMode(void);
bool Globals_IsAutoModeFixed(void);
void Globals_TurnOnAutoMode(bool isFixed);
Random32* Globals_GetSharedRandom(void);
bool Globals_IsAudioDisabledPermanently(void);
void Globals_DisableAudioPermanently(void);
void Globals_DisableAudioPermanentlyBecauseOfNoAudioHardware(void);
void Globals_SetDebugFileMode(bool value);
bool Globals_IsDebugFileMode(void);
void Globals_SetAsExceptionUnsafe(void);
bool Globals_IsExceptionUnsafe(void);
void Globals_SetAsNeedToTestFrameRate(void);
bool Globals_NeedToTestFrameRate(void);
void Globals_SetAsGameLoaded(void);
bool Globals_IsGameLoaded(void);
void Globals_DisableSavingUserData(void);
bool Globals_IsSavingUserDataDisabled(void);
void Globals_DisableAchievements(void);
bool Globals_AreAchievementsDisabled(void);
void Globals_DisableLoadingUserData(void);
bool Globals_IsLoadingUserDataDisabled(void);
void Globals_DisableLeaderboards(void);
bool Globals_AreLeaderboardDisabled(void);
double Globals_GetDebugGameSpeedAsMul(void);
bool Globals_IsDebugGameSpeedSet(void);
void Globals_DisableSavingAndLoadingAndAchievementsAndLeaderboards(void);
int32_t Globals_GetNewestRecordingVersion(void);
int32_t Globals_GetCurrentRecordingVersion(void);
int64_t Globals_GetTime(void);
uint32_t Globals_GetTimeForRandomSeed(void);
void Globals_ToggleGodMode(void);