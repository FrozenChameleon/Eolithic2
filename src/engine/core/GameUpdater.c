/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GameUpdater.h"

#include "../utils/Macros.h"
#include "../core/ServiceHelper.h"
#include "../service/Service.h"
#include "../core/Game.h"
#include "../globals/Globals.h"
#include "../utils/Utils.h"
#include "../input/Input.h"
#include "../math/Math.h"
#include "../utils/FPSTool.h"
#include "../utils/Cvars.h"
#include "../gamestate/GameStateManager.h"
#include "../audio/Music.h"
#include "../audio/SoundEffect.h"
#include "../input/Keys.h"
#include "../utils/Logger.h"
#include "../input/RecordingTool.h"
#include "../input/ActionList.h"
#include "../input/InputAction.h"
#include "../core/Func.h"
#include "../leveldata/LevelData.h"
#include "../input/ControllerStates.h"
#include "../io/DynamicByteBuffer.h"
#include "../render/Renderer.h"
#include "../gamestate/GameState.h"
#ifdef EDITOR_MODE
#include "../editor/Editor.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlgpu3.h"
#endif

static uint64_t _mSkippedFrames;
static uint64_t _mGlobalTicks;
static double _mDeltaAccumulator;
static double _mLastDelta;
static bool _mIsDebugAutoSpeedOn;
static bool _mSuppressFullscreenToggle;
static bool _mPausedMusic;
static bool _mWasNotInFocus;
static bool _mWasShowingControllerLostConnectionMessage;
static FPSTool _mFpsToolUpdate;
static char _mBufferForFPSString[EE_SAFE_BUFFER_LEN_FOR_INT];

static void SetDebugAutoSpeed(bool value)
{
	if (_mIsDebugAutoSpeedOn == value)
	{
		return;
	}

	Cvars_SetAsBool(CVARS_USER_IS_VSYNC, !_mIsDebugAutoSpeedOn);
	Renderer_UpdateVsync();
	Renderer_ApplyChanges();
	_mIsDebugAutoSpeedOn = value;
}
static void HandleAltEnterCheck(void)
{
	if (Input_IsAltPressed())
	{
		if (!_mSuppressFullscreenToggle)
		{
			if (Input_IsEnterTapped())
			{
				Utils_ToggleFullscreenButton();
				Cvars_SaveUserConfig();
				_mSuppressFullscreenToggle = true;
			}
		}
	}
	else
	{
		_mSuppressFullscreenToggle = false;
	}
}
#ifdef EDITOR_MODE
static int32_t _mDebugTimeHeld;
static void DebugLogGameSpeed(void)
{
	MString* tempString = NULL;
	MString_AssignString(&tempString, "Game Speed: ");
	MString_AddAssignDouble(&tempString, Globals_GetDebugGameSpeedAsMul());
	Logger_LogInformation(MString_Text(tempString));
	MString_Dispose(&tempString);
}
static void DebugLogBoolHelper(const char* info, bool value)
{
	MString* tempString = NULL;
	MString_AssignString(&tempString, info);
	MString_AddAssignBool(&tempString, value);
	Logger_LogInformation(MString_Text(tempString));
	MString_Dispose(&tempString);
}
static void Cheats(void)
{
	if (Input_IsCtrlPressed())
	{
		if (Input_IsKeyTapped(KEYS_NUMPAD0))
		{
			GameUpdater_CycleDebugShowInfo();
		}
		if (Input_IsKeyTapped(KEYS_NUMPAD1))
		{
			SetDebugAutoSpeed(false);
			GLOBALS_DEBUG_GAME_LOGIC_SPEED = GLOBALS_DEFAULT_DEBUG_GAME_LOGIC_SPEED;
			DebugLogBoolHelper("Auto Speed: ", _mIsDebugAutoSpeedOn);
		}
		if (Input_IsKeyTapped(KEYS_NUMPAD2))
		{
			GameUpdater_ToggleDebugAutoSpeed();
			DebugLogBoolHelper("Auto Speed: ", _mIsDebugAutoSpeedOn);
		}
		if (Input_IsKeyTapped(KEYS_NUMPAD3))
		{
			//UNUSED
			Logger_LogInformation("Reloaded Audio");
		}
		if (Input_IsKeyTapped(KEYS_NUMPAD4))
		{
			GameUpdater_DebugReloadGraphics();
			Logger_LogInformation("Reloaded All Assets And Animations");
		}
	}
	else if (Input_IsShiftPressed())
	{
	}
	else if (Input_IsAltPressed())
	{
		if (Input_IsKeyTapped(KEYS_NUMPAD0))
		{
			Globals_SetIsRenderDisabled(!Globals_IsRenderDisabled());
			DebugLogBoolHelper("Render Disabled: ", Globals_IsRenderDisabled());
		}
	}
	else
	{
		if (Input_IsKeyTapped(KEYS_NUMPAD0))
		{
			Globals_ToggleGodMode();
		}
		int32_t timeToStart = 60;
		int32_t timeInterval = 5;
		bool timeGo = false;
		if ((_mDebugTimeHeld == 0) || ((_mDebugTimeHeld >= timeToStart) && (_mDebugTimeHeld % timeInterval == 0)))
		{
			timeGo = true;
		}
		if (Input_IsKeyPressed(KEYS_NUMPAD1) && Input_IsKeyPressed(KEYS_NUMPAD2))
		{
			if (GLOBALS_DEBUG_GAME_LOGIC_SPEED != 100)
			{
				SetDebugAutoSpeed(false);
				GLOBALS_DEBUG_GAME_LOGIC_SPEED = 100;
				DebugLogGameSpeed();
			}
		}
		else if (Input_IsKeyPressed(KEYS_NUMPAD1))
		{
			_mDebugTimeHeld += 1;
			if (timeGo)
			{
				SetDebugAutoSpeed(false);
				GLOBALS_DEBUG_GAME_LOGIC_SPEED -= 10;
				if (GLOBALS_DEBUG_GAME_LOGIC_SPEED < 10)
				{
					GLOBALS_DEBUG_GAME_LOGIC_SPEED = 10;
				}
				DebugLogGameSpeed();
			}
		}
		else if (Input_IsKeyPressed(KEYS_NUMPAD2))
		{
			_mDebugTimeHeld += 1;
			if (timeGo)
			{
				SetDebugAutoSpeed(false);
				GLOBALS_DEBUG_GAME_LOGIC_SPEED += 10;
				DebugLogGameSpeed();
			}
		}
		else
		{
			_mDebugTimeHeld = 0;
		}
		if (Input_IsKeyTapped(KEYS_NUMPAD3))
		{
			GLOBALS_DEBUG_IS_PAUSED = !GLOBALS_DEBUG_IS_PAUSED;
			DebugLogBoolHelper("Is Paused: ", GLOBALS_DEBUG_IS_PAUSED);
			return;
		}

		if (Input_IsKeyTapped(KEYS_NUMPAD5))
		{
			Cvars_FlipAsBool(CVARS_EDITOR_SHOW_PROPS);
			DebugLogBoolHelper("Show Props: ", Cvars_GetAsBool(CVARS_EDITOR_SHOW_PROPS));
		}
		if (Input_IsKeyTapped(KEYS_NUMPAD6))
		{
			Cvars_FlipAsBool(CVARS_EDITOR_SHOW_THINGS);
			DebugLogBoolHelper("Show Things: ", Cvars_GetAsBool(CVARS_EDITOR_SHOW_THINGS));
		}
		if (Input_IsKeyTapped(KEYS_NUMPAD7))
		{
			Cvars_FlipAsBool(CVARS_EDITOR_SHOW_TILES);
			DebugLogBoolHelper("Show Tiles: ", Cvars_GetAsBool(CVARS_EDITOR_SHOW_TILES));
		}
		if (Input_IsKeyTapped(KEYS_NUMPAD8))
		{
			if (Globals_IsEditorActive())
			{
				Cvars_FlipAsBool(CVARS_EDITOR_SHOW_COLLISION);
				DebugLogBoolHelper("Show Editor Col: ", Cvars_GetAsBool(CVARS_EDITOR_SHOW_COLLISION));
			}
			else
			{
				GLOBALS_DEBUG_SHOW_INGAME_COLLISION = !GLOBALS_DEBUG_SHOW_INGAME_COLLISION;
				DebugLogBoolHelper("Show InGame Col: ", GLOBALS_DEBUG_SHOW_INGAME_COLLISION);
			}
		}
		if (Input_IsKeyTapped(KEYS_NUMPAD9))
		{
			GLOBALS_DEBUG_DISABLE_HUD = !GLOBALS_DEBUG_DISABLE_HUD;
			DebugLogBoolHelper("Disable Hud: ", GLOBALS_DEBUG_DISABLE_HUD);
		}
		if (Input_IsKeyTapped(KEYS_F8))
		{
			SoundEffect_StopAllSounds();
			GameUpdater_DebugReloadMap();
		}
		if (Input_IsKeyTapped(KEYS_F9))
		{
			GameUpdater_DebugSaveMap();
		}
		if (Input_IsKeyTapped(KEYS_F10))
		{
			SoundEffect_StopAllSounds();
			GameUpdater_FastReset();
		}
		if (Input_IsKeyTapped(KEYS_F11))
		{
			SoundEffect_StopAllSounds();
			GameUpdater_FastResetPlusMove();
		}
		if (Input_IsKeyTapped(KEYS_F12))
		{
			GameUpdater_ToggleEditor();
		}
		if (Input_IsKeyTapped(KEYS_OEMCOMMA))
		{
			//Utils_ToggleNextUserLanguage(true); //UNUSED
		}
	}
}
#endif
static void Tick(void)
{
	Utils_FreeArena(UTILS_ALLOCATION_ARENA_JUST_THIS_FRAME);

	Renderer_StartImGuiFrame();

	GameStateManager_UpdateLastRenderPosition();

	if (ServiceHelper_HasPlayerHasLostControllerConnection())
	{
		if (!_mWasShowingControllerLostConnectionMessage)
		{
			Do_PauseAllSounds(0);
			Do_PauseMusic(0);
		}
		Input_Update(false);
		ServiceHelper_HandlePlayerLostControllerConnection();
		_mWasShowingControllerLostConnectionMessage = true;
	}
	else
	{
		if (_mWasShowingControllerLostConnectionMessage)
		{
			Do_ResumeAllSounds(0);
			Do_ResumeMusic(0);
		}
		GameStateManager_Tick();
		_mGlobalTicks += 1;
		_mWasShowingControllerLostConnectionMessage = false;
	}

	HandleAltEnterCheck();

#ifdef EDITOR_MODE
	Cheats();
	if (Globals_IsEditorActive())
	{
		Editor_Update(1.0f / 60.0f);
		Renderer_SetupRenderState();
	}
#endif

	Renderer_SetupImGuiRenderState();
}
#ifdef EDITOR_MODE
static bool HandleDebugPauseAndStep(void)
{
	/*
	int32_t readFrame = OeControllerStates_GetMasterRecordingReadFrame(); //For specific frame stops
	if (readFrame == 137600)
	{
		GLOBALS_DEBUG_IS_PAUSED = true;
	}
	*/

	if (!GLOBALS_DEBUG_IS_PAUSED)
	{
		return false;
	}

	Input_Update(true);

	if (Input_IsKeyTapped(KEYS_NUMPAD4))
	{
		Tick();
		Renderer_SetupRenderState();
		Logger_LogInformation("Step");
		return true;
	}

	if (Input_IsKeyTapped(KEYS_OEMCOMMA))
	{
		//Utils_ToggleNextUserLanguage(true); //UNUSED
		return true;
	}

	if (Input_IsKeyTapped(KEYS_OEMPERIOD))
	{
		//Utils_ToggleNextUserLanguage(true); //UNUSED
		Tick();
		Renderer_SetupRenderState();
		Logger_LogInformation("Step");
		return true;
	}

	if (Input_GetPlayerOneAction(ACTIONLIST_GAME_LS)->mIsTapped)
	{
		GameState_CreateDebugSaveState(GameStateManager_GetGameState());
	}

	if (Input_GetPlayerOneAction(ACTIONLIST_GAME_RS)->mIsTapped)
	{
		GameState_UseDebugSaveState(GameStateManager_GetGameState());
	}

	if (Input_GetPlayerOneAction(ACTIONLIST_GAME_LB)->mIsTapped)
	{
		GameState_Rewind(GameStateManager_GetGameState());
	}

	Renderer_SetupRenderState();

	Cheats();

	return true;
}
#endif
static bool IsPaused(void)
{
#ifdef DEBUG_DEF_MAKE_GAME_ALWAYS_ACTIVE
	return false;
#else
	if (Service_PlatformDoesNotDoNormalPausing())
	{
		return false;
	}

	bool isGameActive = Game_IsActive();
	bool pauseOnFocusLoss = Cvars_GetAsBool(CVARS_USER_IS_PAUSE_ON_FOCUS_LOSS);
	bool muteOnFocusLoss = Cvars_GetAsBool(CVARS_USER_IS_MUTE_ON_FOCUS_LOSS);
	if (pauseOnFocusLoss && Music_DoNotAllowUpdatesWhilePaused())
	{
		muteOnFocusLoss = true;
	}

	if (isGameActive)
	{
		if (_mWasNotInFocus)
		{
			Do_ResumeAllSounds(0);

			if (_mPausedMusic)
			{
				Do_ResumeMusic(0);
			}

			SoundEffect_SetSfxMuted(false);
			Music_SetMusicMuted(false);

			_mWasNotInFocus = false;
			_mPausedMusic = false;
		}
	}
	else
	{
		if (!_mWasNotInFocus)
		{
			if (pauseOnFocusLoss)
			{
				Do_PauseAllSounds(0);

				Input_StopAllRumble();

				if (muteOnFocusLoss)
				{
					_mPausedMusic = true;
					Do_PauseMusic(0);
				}
			}

			if (muteOnFocusLoss)
			{
				SoundEffect_SetSfxMuted(true);
				Music_SetMusicMuted(true);
			}

			_mWasNotInFocus = true;
		}
		if (!muteOnFocusLoss && pauseOnFocusLoss)
		{
			Music_Tick();
		}
	}

	if (!isGameActive && pauseOnFocusLoss)
	{
		return true;
	}

	return false;
#endif
}
static void UpdateLoop(double deltaTime)
{
#ifdef EDITOR_MODE
	if (HandleDebugPauseAndStep())
	{
		return;
	}
#endif

#ifdef DEBUG_DEF_RUN_THE_GAME_FAST
	_mIsDebugAutoSpeedOn = true;
#endif

	double stepLength = Utils_GetNormalStepLength();
	bool interpolated = GameUpdater_IsInterpolated();

	int32_t fixedTimeStepTicks = 1;
	if (!_mIsDebugAutoSpeedOn)
	{
		if ((RecordingTool_IsReading() && RecordingTool_IsGoFastFlagSet()) || (Input_IsPlayingMasterRecording() && INPUT_FLAG_IS_PLAYING_MASTER_RECORDING_FAST))
		{
			fixedTimeStepTicks = Cvars_GetAsInt(CVARS_ENGINE_RECORDING_PLAYBACK_FAST_SPEED);
			if (fixedTimeStepTicks < 1)
			{
				fixedTimeStepTicks = 1;
			}
			deltaTime *= fixedTimeStepTicks;
		}
	}
	else
	{
		if (Service_PlatformForcesRelyOnVsync() || Cvars_GetAsBool(CVARS_ENGINE_RELY_ON_VSYNC)) //Make rely on vsync run at fast speed when debug auto speed is on
		{
			fixedTimeStepTicks = Cvars_GetAsInt(CVARS_ENGINE_RECORDING_PLAYBACK_FAST_SPEED);
		}
	}

#ifdef EDITOR_MODE
	if (!Globals_IsEditorActive() && Globals_IsDebugGameSpeedSet())
	{
		deltaTime *= Globals_GetDebugGameSpeedAsMul();
	}
#endif

#ifdef DEBUG_DEF_FORCE_GAME_SPEED_MUL
	fixedTimeStepTicks = DEBUG_DEF_FORCE_GAME_SPEED_MUL;
	delta += (DEBUG_DEF_FORCE_GAME_SPEED_MUL * stepLength);
#endif

	bool ticked = false;
	if (!interpolated)
	{
		_mDeltaAccumulator = 0;
		for (int32_t i = 0; i < fixedTimeStepTicks; i += 1)
		{
			Tick();
			ticked = true;
		}
	}
	else
	{
		_mDeltaAccumulator += deltaTime;
		while (_mDeltaAccumulator >= stepLength)
		{
			if (ticked)
			{
				_mSkippedFrames += 1;
			}
			Tick();
			_mDeltaAccumulator -= stepLength;
			ticked = true;
		}
	}

	if (ticked)
	{
		Renderer_SetupRenderState();
	}
}

const char* GameUpdater_GetFpsString(void)
{
	int32_t number;
	if (_mIsDebugAutoSpeedOn)
	{
		float speed = GameUpdater_GetAutoSpeedUpdateSpeed();
		number = (int32_t)speed;
	}
	else
	{
		number = Renderer_GetFPS();
	}
	number = Math_MinInt(number, 9999);
	Utils_IntToString(number, _mBufferForFPSString, EE_SAFE_BUFFER_LEN_FOR_INT);
	return _mBufferForFPSString;
}
float GameUpdater_GetAutoSpeedUpdateSpeed(void)
{
	return GameUpdater_GetFPS() / 60.0f;
}
int32_t GameUpdater_GetFPS(void)
{
	return _mFpsToolUpdate.mCurrentFps;
}
bool GameUpdater_IsInterpolated(void)
{
	if (_mIsDebugAutoSpeedOn)
	{
		return false;
	}

	if (Service_PlatformForcesRelyOnVsync() || Cvars_GetAsBool(CVARS_ENGINE_RELY_ON_VSYNC))
	{
		return false;
	}

#ifdef EDITOR_MODE
	if (Globals_IsDebugGameSpeedSet())
	{
		return true;
	}
#endif

	if (Cvars_GetAsBool(CVARS_USER_IS_FIXED_TIMESTEP_ENABLED))
	{
		return false;
	}

	return true;
}
void GameUpdater_Update(double deltaTime)
{
	_mLastDelta = deltaTime;

	FPSTool_Update(&_mFpsToolUpdate, deltaTime);

	if (!Game_IsActive())
	{
		if (Input_HasInit() && Service_PlatformMutesInputWhenGameIsNotActive())
		{
			Input_MuteAllInput();
		}
	}

	if (!IsPaused())
	{
		UpdateLoop(deltaTime);
	}
}
void GameUpdater_DebugReloadMap(void)
{
	//Get_LevelDataResource()->Reload(); //UNUSED FOR NOW
	
	{
		MString* tempString = NULL;
		MString_Combine5(&tempString, "Map Reloaded (Full) [", Resource_GetFilenameWithoutExtension(Get_LevelDataResource()), "][", Get_LevelFileName(), "]");
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}

	GLOBALS_DEBUG_QUICK_PLAYER_POSITION = Vector2_Zero;

	GameStateManager_SetupReloadMap();
}
void GameUpdater_DebugSaveMap(void)
{
	//UNUSED
}
void GameUpdater_FastReset(void)
{
	GLOBALS_DEBUG_QUICK_PLAYER_POSITION = Vector2_Zero;
	GameStateManager_DebugForceReloadMapNow();
	GameStateManager_SetGameState(GAMESTATEMANAGER_GAME_STATE_NORMAL);
	Globals_SetIsEditorActive(false);
	{
		MString* tempString = NULL;
		MString_Combine5(&tempString, "Map Reloaded (Fast) [", MString_Text(Resource_GetPath(Get_LevelDataResource())), "][", Get_LevelData()->mLevelName, "]");
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}
}
void GameUpdater_FastResetPlusMove(void)
{
	//UNUSED
}
void GameUpdater_ToggleEditor(void)
{
	Globals_SetIsEditorActive(!Globals_IsEditorActive());
	Logger_LogInformation("Editor Toggle");
}
void GameUpdater_ToggleDebugAutoSpeed(void)
{
	SetDebugAutoSpeed(!_mIsDebugAutoSpeedOn);
}
void GameUpdater_DebugReloadGraphics(void)
{
	//UNUSED
}
void GameUpdater_CycleDebugShowInfo(void)
{
	GLOBALS_DEBUG_SHOW_INFO += 1;
	if (GLOBALS_DEBUG_SHOW_INFO > GAMESTATEMANAGER_DEBUG_INFO_SHOW_QUICK_STATS)
	{
		GLOBALS_DEBUG_SHOW_INFO = GAMESTATEMANAGER_DEBUG_INFO_SHOW_NOTHING;
	}

	{
		MString* tempString = NULL;
		MString_AssignString(&tempString, "Debug Info: ");
		MString_AddAssignBool(&tempString, GLOBALS_DEBUG_SHOW_INFO);
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}
}
uint64_t GameUpdater_GetGlobalTicks(void)
{
	return _mGlobalTicks;
}
bool GameUpdater_IsDebugAutoSpeedOn(void)
{
	return _mIsDebugAutoSpeedOn;
}
double GameUpdater_GetLastDelta(void)
{
	return _mLastDelta;
}
double GameUpdater_GetDeltaAccumulator(void)
{
	return _mDeltaAccumulator;
}
uint64_t GameUpdater_GetSkippedFrames(void)
{
	return _mSkippedFrames;
}
