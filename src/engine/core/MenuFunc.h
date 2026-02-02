/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Vector2.h"
#include "../math/Rectangle.h"
#include "../utils/StringPair.h"

enum MenuDirection
{
	MENUFUNC_DIRECTION_UP = 0,
	MENUFUNC_DIRECTION_RIGHT = 1,
	MENUFUNC_DIRECTION_DOWN = 2,
	MENUFUNC_DIRECTION_LEFT = 3,
	MENUFUNC_DIRECTION_UPRIGHT = 4,
	MENUFUNC_DIRECTION_DOWNRIGHT = 5,
	MENUFUNC_DIRECTION_DOWNLEFT = 6,
	MENUFUNC_DIRECTION_UPLEFT = 7
};

//ACTION
enum MenuCatAction
{
	MENUFUNC_ACTION_GO_TO_ANCHOR = -1,
	MENUFUNC_ACTION_LOAD_LEVEL = 0,
	MENUFUNC_ACTION_RESUME_GAME = 2,
	MENUFUNC_ACTION_EXIT_GAME = 3,
	MENUFUNC_ACTION_GO_BACK_ONE_ANCHOR = 4,
	MENUFUNC_ACTION_LOAD_SAVE_SLOT = 5,
	MENUFUNC_ACTION_GO_FORWARD = 6,
	MENUFUNC_ACTION_GO_BACKWARD = 7,
	MENUFUNC_ACTION_RETURN_TO_TITLE = 8
};

//VIDEO
enum MenuCatVideo
{
	MENUFUNC_VIDEO_FULLSCREEN_TOGGLE = 100,
	MENUFUNC_VIDEO_VSYNC_TOGGLE = 101,
	MENUFUNC_VIDEO_SHOW_FPS_TOGGLE = 102,
	MENUFUNC_VIDEO_DRAW_MODE_TOGGLE = 103,
	MENUFUNC_VIDEO_REFRESH_SYNC_TOGGLE = 104,
	MENUFUNC_VIDEO_CHANGE_WINDOW_SIZE_MULTIPLE = 105,
	MENUFUNC_VIDEO_APPLY_WINDOW_SIZE_MULTIPLE = 106,
	MENUFUNC_VIDEO_FIXED_TIME_STEP_TOGGLE = 107,
	MENUFUNC_VIDEO_SHOW_IN_GAME_TIMER_TOGGLE = 108,
	MENUFUNC_VIDEO_INTERNAL_RESOLUTION_MULTIPLE_TOGGLE = 109,
	MENUFUNC_VIDEO_LINEAR_FILTER_TOGGLE = 110,
	MENUFUNC_VIDEO_CHANGE_WINDOW_SIZE = 111,
	MENUFUNC_VIDEO_APPLY_CHANGE_WINDOW_SIZE = 112
};

//SOUND
enum MenuCatSound
{
	MENUFUNC_SOUND_SFX_VOLUME_TOGGLE = 200,
	MENUFUNC_SOUND_MUSIC_VOLUME_TOGGLE = 201,
	MENUFUNC_SOUND_MUTE_ON_FOCUS_LOSS_TOGGLE = 202,
	MENUFUNC_SOUND_STOP_SOUND_TEST_MUSIC = 203,
	MENUFUNC_SOUND_CHANGE_SOUND_TEST_MUSIC = 204,
	MENUFUNC_SOUND_CHANGE_SOUND_TEST_SFX = 205,
	MENUFUNC_SOUND_PLAY_SOUND_TEST_MUSIC = 206,
	MENUFUNC_SOUND_PLAY_SOUND_TEST_SFX = 207,
	MENUFUNC_SOUND_STOP_SOUND_TEST_SFX = 208,
	MENUFUNC_SOUND_MASTER_VOLUME_TOGGLE = 209
};

//INPUT
enum MenuCatInput
{
	MENUFUNC_INPUT_CONTROLLER_TYPE_TOGGLE = 300,
	MENUFUNC_INPUT_RUMBLE_TOGGLE = 301,
	MENUFUNC_INPUT_DEADZONE_TOGGLE = 302,
	MENUFUNC_INPUT_RESET_ALL_BINDINGS = 303,
	MENUFUNC_INPUT_BIND_ACTION = 304,
	MENUFUNC_INPUT_RESET_CERTAIN_BINDINGS = 305
};

//GAME
enum MenuCatGame
{
	MENUFUNC_GAME_BORDER_TOGGLE = 400,
	MENUFUNC_GAME_BORDER_FRAME = 401,
	MENUFUNC_GAME_PAUSE_ON_FOCUS_LOSS_TOGGLE = 402,
	MENUFUNC_GAME_IS_SCREENSHAKE_DISABLED = 403,
	MENUFUNC_GAME_TURBO_MODE_TOGGLE = 404,
	MENUFUNC_GAME_TOGGLE_LANGUAGE = 405,
	MENUFUNC_GAME_TOGGLE_ALT_FONT = 406,
	MENUFUNC_GAME_RESET_ALL_OPTIONS_TO_DEFAULTS = 407
};

//DEBUG
enum MenuCatDebug
{
	MENUFUNC_DEBUG_UNLOCK_NEXT_ACHIEVEMENT = 7701,
	MENUFUNC_DEBUG_SEND_FAKE_SCORE_AND_TIME = 7702,
	MENUFUNC_DEBUG_RESET_ACHIEVEMENTS = 7703,
	MENUFUNC_DEBUG_GOD_MODE = 7704,
	MENUFUNC_DEBUG_PLAY_RECORDINGS = 7705,
	MENUFUNC_DEBUG_TOGGLE_AUTO_SPEED = 7706,
};

void MenuFunc_Init(void);

Rectangle MenuFunc_GetTempWindowSize(void);
void MenuFunc_SetTempWindowSize(Rectangle value);
int32_t MenuFunc_GetTempWindowSizeMultiple(void);
void MenuFunc_SetTempWindowSizeMultiple(int32_t value);
void MenuFunc_GameResetAllOptionsToDefaults(void);
void MenuFunc_DebugGodModeToggle(void);
void MenuFunc_DebugResetAchievements(void);
void MenuFunc_DebugUnlockNextAchievement(void);
//void MenuFunc_ToggleNextUserLanguage(); //UNUSED
//const char* MenuFunc_GetUserLanguageName(); //UNUSED
//bool MenuFunc_WentBackThisFrame(); //UNUSED
void MenuFunc_SoundStopSoundTestMusic(void);
void MenuFunc_SoundChangeSoundTextMusic(int32_t direction);
void MenuFunc_SoundChangeSoundTextSfx(int32_t direction);
void MenuFunc_SoundPlaySoundTestSfx(void);
void MenuFunc_SoundPlaySoundTestMusic(void);
const char* MenuFunc_GetCurrentSoundTestMusic(void);
const char* MenuFunc_GetCurrentSoundTestMusicAsNumber(void);
const char* MenuFunc_GetCurrentSoundTestSfx(void);
const char* MenuFunc_GetCurrentSoundTestSfxAsNumber(void);
//const char* MenuFunc_GetGenericText(int32_t action, const char* data1, const char* data2); //UNUSED
const char* MenuFunc_GetChangeWindowSizeMultipleString(void);
const char* MenuFunc_GetChangeWindowSizeString(void);
const char* MenuFunc_GetInternalResolutionMultipleString(void);
void MenuFunc_SoundStopSoundTestSfx(void);
void MenuFunc_VideoFixedTimeStepToggle(void);
void MenuFunc_VideoApplyWindowSizeMultiple(void);
void MenuFunc_VideoApplyWindowSize(void);
void MenuFunc_VideoChangeWindowSize(int32_t direction, int32_t forcedSliderValue);
void MenuFunc_VideoChangeWindowSizeMultiple(int32_t direction, int32_t forcedSliderValue);
//const char* MenuFunc_StringHelperBorder(int32_t value); //UNUSED
const char* MenuFunc_StringHelperFloat(float value);
const char* MenuFunc_StringHelperInt(int32_t value);
//void MenuFunc_InputDeadzoneToggle(); //UNUSED
int32_t MenuFunc_VolumeToggle(const char* cvar, int32_t direction, int32_t forcedSliderValue);
void MenuFunc_VideoVsyncToggle(void);
void MenuFunc_InputRumbleToggle(void);
void MenuFunc_VideoShowFpsToggle(void);
void MenuFunc_VideoShowInGameTimerToggle(void);
void MenuFunc_VideoInternalResolutionMultipleToggle(int32_t direction, int32_t forcedSliderValue);
int32_t MenuFunc_GetVideoInternalResolutionMultipleToggleMaxValue(void);
int32_t MenuFunc_GetVideoChangeWindowSizeMultipleMaxValue(void);
void MenuFunc_VideoLinearFilterToggle(void);
void MenuFunc_SoundMuteOnFocusToggle(void);
void MenuFunc_GamePauseOnFocusToggle(void);
void MenuFunc_GameTurboModeToggle(void);
void MenuFunc_GameToggleAltFont(void);
void MenuFunc_InputResetBindings(void);
void MenuFunc_ActionResumeGame(void);
void MenuFunc_ActionExitGame(void);
void MenuFunc_ActionLoadLevel(const char* level);
void MenuFunc_GameBorderFrameToggle(void);
void MenuFunc_GameScreenshakeToggle(void);
//void MenuFunc_ActionLoadSaveSlot(const char* save, const char* level); //UNUSED
void MenuFunc_VideoRefreshSyncToggle(void);
void MenuFunc_VideoDrawModeToggle(int32_t direction);
void MenuFunc_InputControllerTypeToggle(void);
void MenuFunc_GameBorderToggle(void);
void MenuFunc_VideoFullscreenToggle(void);
void MenuFunc_PlayMenuSoundSelect(void);
void MenuFunc_PlayMenuSoundDeny(void);
void MenuFunc_PlayMenuSoundUp(void);
void MenuFunc_PlayMenuSoundRight(void);
void MenuFunc_PlayMenuSoundDown(void);
void MenuFunc_PlayMenuSoundLeft(void);
void MenuFunc_PlayMenuSoundHelper(const char* soundToPlay);
void MenuFunc_SaveIfLoadedOptionsMenu(void);
void MenuFunc_SoundTestAddSoundEffect(const char* key, const char* value);
void MenuFunc_SoundTestAddMusic(const char* key, const char* value);