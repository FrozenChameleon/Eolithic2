/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Globals.h"

#include "../utils/Macros.h"
#include "../utils/Logger.h"
#include "../service/Service.h"
#include "../utils/Utils.h"
#include "../input/Input.h"
#include "../input/ControllerStates.h"
#include "../input/RecordingTool.h"
#include "time.h"

static bool _mIsAutoMode;
static bool _mIsAutoModeFixed;
static Random32 _mSharedRandom;
static bool _mIsAudioDisabled;
static bool _mIsLoadingUserDataDisabled;
static bool _mAreAchievementsDisabled;
static bool _mIsSavingUserDataDisabled;
static bool _mIsGameLoaded;
static bool _mNeedToTestFrameRate;
static bool _mIsExceptionUnsafe;
static bool _mDebugFileModeWasSet;
static bool _mIsDebugFileMode;
static bool _mIsRenderDisabled;
static bool _mDebugIsEditorMode;

Vector2 GLOBALS_DEBUG_QUICK_PLAYER_POSITION;
int32_t GLOBALS_DEBUG_SHOW_INFO;
int32_t GLOBALS_DEBUG_GAME_LOGIC_SPEED = GLOBALS_DEFAULT_DEBUG_GAME_LOGIC_SPEED;
bool GLOBALS_DEBUG_SHOW_INGAME_COLLISION;
bool GLOBALS_DEBUG_DISABLE_HUD;
bool GLOBALS_DEBUG_IS_GOD_MODE;
bool GLOBALS_DEBUG_IS_PAUSED;
bool GLOBALS_DEBUG_EDITOR_JUST_RELOADED_GRAPHICS;
bool GLOBALS_DEBUG_JUST_LOADED_MAP_NOTIFY_EDITOR;
bool GLOBALS_DEBUG_IS_META_MAP_EDIT_TILE_MODE_AT_MAP_LOAD;
bool GLOBALS_DEBUG_ENGINE_FORCE_LOAD_DATS;

bool Globals_DebugIsEditorMode(void)
{
#ifdef EDITOR_MODE
	return _mDebugIsEditorMode;
#else
	return false;
#endif
}
void Globals_SetDebugIsEditorMode(bool value)
{
#ifdef EDITOR_MODE
	_mDebugIsEditorMode = value;
#else
	//DO NOTHING
#endif
}
bool Globals_DebugIsRenderDisabled(void)
{
	return _mIsRenderDisabled;
}
void Globals_DebugSetRenderDisabled(bool value)
{
	_mIsRenderDisabled = value;
}
bool Globals_IsAutoMode(void)
{
	return _mIsAutoMode;
}
bool Globals_IsAutoModeFixed(void)
{
	return _mIsAutoModeFixed;
}
void Globals_TurnOnAutoMode(bool isFixed)
{
	Globals_DisableSavingAndLoadingAndAchievementsAndLeaderboards();
	_mIsAutoMode = true;
	_mIsAutoModeFixed = isFixed;
}
Random32* Globals_GetSharedRandom(void)
{
	return &_mSharedRandom;
}
bool Globals_IsAudioDisabledPermanently(void)
{
	return _mIsAudioDisabled;
}
void Globals_DisableAudioPermanently(void)
{
	if (_mIsAudioDisabled)
	{
		return;
	}

	Logger_LogError("Disabling all audio");
	_mIsAudioDisabled = true;
}
void Globals_DisableAudioPermanentlyBecauseOfNoAudioHardware(void)
{
	if (_mIsAudioDisabled)
	{
		return;
	}

	Logger_LogError("No audio hardware, disabling all audio");
	_mIsAudioDisabled = true;
}
void Globals_SetDebugFileMode(bool value)
{
	if (_mDebugFileModeWasSet)
	{
		return;
	}

	_mDebugFileModeWasSet = true;
	_mIsDebugFileMode = value;
}
bool Globals_IsDebugFileMode(void)
{
	return _mIsDebugFileMode;
}
void Globals_SetAsExceptionUnsafe(void)
{
	_mIsExceptionUnsafe = true;
}
bool Globals_IsExceptionUnsafe(void)
{
	return _mIsExceptionUnsafe;
}
void Globals_SetAsNeedToTestFrameRate(void)
{
	_mNeedToTestFrameRate = true;
}
bool Globals_NeedToTestFrameRate(void)
{
	return _mNeedToTestFrameRate;
}
void Globals_SetAsGameLoaded(void)
{
	_mIsGameLoaded = true;
}
bool Globals_IsGameLoaded(void)
{
	return _mIsGameLoaded;
}
void Globals_DisableSavingUserData(void)
{
#ifdef DEBUG_DEF_DISABLE_RECORDING_SAFETY_SAVING
	return;
#else
	_mIsSavingUserDataDisabled = true;
#endif
}
bool Globals_IsSavingUserDataDisabled(void)
{
#ifdef DEBUG_DEF_DISABLE_SAVING
	return true;
#else
	return _mIsSavingUserDataDisabled;
#endif
}
void Globals_DisableAchievements(void)
{
#ifdef DEBUG_DEF_DISABLE_RECORDING_SAFETY_ACHIEVEMENTS
	return;
#else
	_mAreAchievementsDisabled = true;
#endif
}
bool Globals_AreAchievementsDisabled(void)
{
	return _mAreAchievementsDisabled;
}
void Globals_DisableLoadingUserData(void)
{
#ifdef DEBUG_DEF_DISABLE_RECORDING_SAFETY_LOADING
	return;
#else
	_mIsLoadingUserDataDisabled = true;
#endif
}
bool Globals_IsLoadingUserDataDisabled(void)
{
#ifdef DEBUG_DEF_DISABLE_LOADING
	return true;
#else
	return _mIsLoadingUserDataDisabled;
#endif
}
void Globals_DisableLeaderboards(void)
{
#ifdef DEBUG_DEF_DISABLE_RECORDING_SAFETY_LEADERBOARDS
	return;
#else
	Service_DisableLeaderboards();
#endif
}
bool Globals_AreLeaderboardDisabled(void)
{
	return Service_AreLeaderboardDisabled();
}
double Globals_GetDebugGameSpeedAsMul(void)
{
	return (double)GLOBALS_DEBUG_GAME_LOGIC_SPEED / GLOBALS_DEFAULT_DEBUG_GAME_LOGIC_SPEED;
}
bool Globals_IsDebugGameSpeedSet(void)
{
	if (GLOBALS_DEBUG_GAME_LOGIC_SPEED == GLOBALS_DEFAULT_DEBUG_GAME_LOGIC_SPEED)
	{
		return false;
	}
	if (GLOBALS_DEBUG_GAME_LOGIC_SPEED <= 0)
	{
		return false;
	}
	return true;
}
void Globals_DisableSavingAndLoadingAndAchievementsAndLeaderboards(void)
{
	Globals_DisableSavingUserData();
	Globals_DisableLoadingUserData();
	Globals_DisableAchievements();
	Globals_DisableLeaderboards();
}
int32_t Globals_GetNewestRecordingVersion(void)
{
	return GLOBALS_RECORDING_VERSION_2_CS_THING_ID_FIX;
}
int32_t Globals_GetCurrentRecordingVersion(void)
{
	if (Input_IsPlayingMasterRecording())
	{
		return ControllerStates_GetCurrentMasterRecordingVersion();
	}
	else if (RecordingTool_IsReading())
	{
		return RecordingTool_GetCurrentRecordingVersion();
	}
	else
	{
		return Globals_GetNewestRecordingVersion();
	}
}
int64_t Globals_GetTime(void)
{
	time_t currentTime;
	currentTime = time(NULL);
	return (int64_t)currentTime;
}
uint32_t Globals_GetTimeForRandomSeed(void)
{
	int64_t source = Globals_GetTime();
	uint64_t dest;
	Utils_memcpy(&dest, &source, sizeof(uint64_t));
	uint32_t returnValue = (uint32_t)dest;
	return returnValue;
}
void Globals_ToggleGodMode(void)
{
	GLOBALS_DEBUG_IS_GOD_MODE = !GLOBALS_DEBUG_IS_GOD_MODE;
	{
		MString* tempString = NULL;
		MString_AssignString(&tempString, "God Mode: ");
		MString_AddAssignBool(&tempString, GLOBALS_DEBUG_IS_GOD_MODE);
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}
}