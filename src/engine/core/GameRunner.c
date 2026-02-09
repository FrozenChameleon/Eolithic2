/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GameRunner.h"

#include "../utils/Macros.h"
#include "../utils/Exception.h"
#include "../core/Game.h"
#include "../core/GameHelper.h"
#include "../service/Service.h"
#include "../utils/Utils.h"
#include "../globals/Globals.h"
#include "../input/RecordingTool.h"
#include "../input/Input.h"
#include "../utils/Logger.h"
#include "../utils/Cvars.h"

#define SAFETY_ARGUMENT_MAX_LENGTH 255

static bool DEBUG_SERVICE_DISABLED;
static const char ARGS_FILE_SEPARATOR = ' ';

/*static void HandleDebugFlag(void) //UNUSED, NO DEBUG FILE MODE YET
{
#if EDITOR_MODE
	Globals_SetDebugFileMode(true);
#else
	Globals_SetDebugFileMode(false);
#endif

	if (Globals_IsDebugFileMode())
	{
		Logger_LogInformation("Game is in debug file mode");
	}
}*/
/*static void FillArgListFromArgFile(std_string path, std_vector<std_string>& argList)
{
	if (!OeFile_FileExists(path))
	{
		return;
	}

	OeLogger_LogInformation("Loading arg file: " + path);
	std_string temp;

	//WILLNOTDO 06262023 2023
	//using (StreamReader reader = OeFile.CreateStreamReader(path))
	//{
	//	temp = reader.ReadToEnd();
	//}

	if (temp == "")
	{
		return;
	}

	std_vector<std_string> args = std_vector<std_string>();
	OeUtils_StringSplit(ARGS_FILE_SEPARATOR, temp, args);
	for (int i = 0; i < args.size(); i++)
	{
		argList.push_back(args[i]);
	}
}*/

static void HandleCvarsFromArgList(int argc, char* args[])
{
	for (int32_t i = 0; i < argc; i += 1)
	{
		const char* argument = args[i];

		if ((i + 1) >= argc) //Need two arguments present to do this
		{
			continue;
		}

		size_t argumentLen = Utils_strnlen(argument, SAFETY_ARGUMENT_MAX_LENGTH);
		if (argumentLen < 2)
		{
			continue;
		}

		if (argument[0] != '+')
		{
			continue;
		}

		{
			MString* cvarKey = NULL;
			MString* cvarValue = NULL;

			MString_AssignSubString(&cvarKey, argument, 1, (int32_t)(argumentLen - 1));
			MString_AssignString(&cvarValue, args[i + 1]);

			{
				MString* tempString = NULL;
				MString_Combine3(&tempString, argument, " ", MString_Text(cvarValue));
				Logger_LogInformation(MString_Text(tempString));
				MString_Dispose(&tempString);
			}

			Cvars_SetAsString(MString_Text(cvarKey), MString_Text(cvarValue));

			MString_Dispose(&cvarKey);
			MString_Dispose(&cvarValue);
		}

		i += 1;
	}
}
static void HandleSpecialArgsFromArgList(int argc, char* args[])
{
	for (int32_t i = 0; i < argc; i += 1)
	{
		const char* argument = args[i];
		bool showArg = false;
		if (Utils_StringEqualTo(argument, "-disable_audio"))
		{
			Globals_DisableAudioPermanently();
			showArg = true;
		}
		if (Utils_StringEqualTo(argument, "-disable_achievements"))
		{
			Globals_DisableAchievements();
			showArg = true;
		}
		if (Utils_StringEqualTo(argument, "-disable_leaderboards"))
		{
			Globals_DisableLeaderboards();
			showArg = true;
		}
		if (Utils_StringEqualTo(argument, "-disable_saving"))
		{
			Globals_DisableSavingUserData();
			showArg = true;
		}
		if (Utils_StringEqualTo(argument, "-disable_loading"))
		{
			Globals_DisableLoadingUserData();
			showArg = true;
		}
		if (Utils_StringEqualTo(argument, "-disable_slal"))
		{
			Globals_DisableSavingAndLoadingAndAchievementsAndLeaderboards();
			showArg = true;
		}
		if (Utils_StringEqualTo(argument, "-debug"))
		{
			Globals_SetDebugFileMode(true);
			showArg = true;
		}
		if (Utils_StringEqualTo(argument, "-nodebug"))
		{
			Globals_SetDebugFileMode(false);
			showArg = true;
		}
		if (Utils_StringEqualTo(argument, "-unsafe"))
		{
			Globals_SetAsExceptionUnsafe();
			showArg = true;
		}
		if (Utils_StringEqualTo(argument, "-noservice"))
		{
			showArg = true;
			DEBUG_SERVICE_DISABLED = true;
		}
		if (Utils_StringEqualTo(argument, "-create_recording"))
		{
			showArg = true;
			//UNUSED OeInput_SetMasterRecordingState(Input_MASTER_RECORDING_STATE_RECORDING);
		}
		if (Utils_StringEqualTo(argument, "-play_recording"))
		{
			showArg = true;
			//UNUSED OeInput_SetMasterRecordingState(Input_MASTER_RECORDING_STATE_PLAYBACK);
		}
		if (Utils_StringEqualTo(argument, "-play_recording_fast"))
		{
			showArg = true;
			//UNUSED Input_SetMasterRecordingState(Input_MASTER_RECORDING_STATE_PLAYBACK);
			//UNUSED Input_FLAG_IS_PLAYING_MASTER_RECORDING_FAST = true;
		}
		if (Utils_StringEqualTo(argument, "-soak_test"))
		{
			showArg = true;
			RecordingTool_EnableFromArgumentsPlayback(RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_SOAK_NORMAL);
		}
		if (Utils_StringEqualTo(argument, "-soak_test_fast"))
		{
			showArg = true;
			RecordingTool_EnableFromArgumentsPlayback(RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_SOAK_FAST);
		}
		if (Utils_StringEqualTo(argument, "-play_recording_sessions"))
		{
			showArg = true;
			RecordingTool_EnableFromArgumentsPlayback(RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_SESSION_NORMAL);
		}
		if (Utils_StringEqualTo(argument, "-play_recording_sessions_fast"))
		{
			showArg = true;
			RecordingTool_EnableFromArgumentsPlayback(RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_SESSION_FAST);
		}
		if (Utils_StringEqualTo(argument, "-auto_mode"))
		{
			showArg = true;
			Globals_TurnOnAutoMode(false);
		}
		if (Utils_StringEqualTo(argument, "-auto_mode_fixed"))
		{
			showArg = true;
			Globals_TurnOnAutoMode(true);
		}
		if (showArg)
		{
			Logger_LogInformation(argument);
		}
	}
}
#if defined(DEBUG_DEF_BUILT_WITH_DEBUG_DEFS)
static void HandleDebugDefs(void)
{
#if defined(DEBUG_DEF_AUTO_MODE)
	Globals_TurnOnAutoMode(false);
#elif defined(DEBUG_DEF_AUTO_MODE_FIXED)
	Globals_TurnOnAutoMode(true);
#elif defined(DEBUG_DEF_PLAY_RECORDING)
	//UNUSED Input_SetMasterRecordingState(OeInput_MASTER_RECORDING_STATE_PLAYBACK);
#elif defined(DEBUG_DEF_PLAY_RECORDING_FAST)
	//UNUSED Input_SetMasterRecordingState(OeInput_MASTER_RECORDING_STATE_PLAYBACK);
	//UNUSED Input_FLAG_IS_PLAYING_MASTER_RECORDING_FAST = true;
#elif defined(DEBUG_DEF_SOAK_TEST)
	RecordingTool_EnableFromArgumentsPlayback(RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_SOAK_NORMAL);
#elif defined(DEBUG_DEF_SOAK_TEST_FAST)
	RecordingTool_EnableFromArgumentsPlayback(RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_SOAK_FAST);
#elif defined(DEBUG_DEF_PLAY_RECORDING_SESSIONS)
	RecordingTool_EnableFromArgumentsPlayback(RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_SESSION_NORMAL);
#elif defined(DEBUG_DEF_PLAY_RECORDING_SESSIONS_FAST)
	RecordingTool_EnableFromArgumentsPlayback(RECORDINGTOOL_FROM_ARGUMENTS_PLAYBACK_SESSION_FAST);
#endif

#if defined(DEBUG_DEF_DISABLE_AUDIO)
	Globals_DisableAudioPermanently();
#endif

#if defined(DEBUG_DEF_DISABLE_SAVING)
	Globals_DisableSavingUserData();
#endif

#if defined(DEBUG_DEF_DISABLE_LOADING)
	Globals_DisableLoadingUserData();
#endif

#if defined(DEBUG_DEF_DISABLE_ACHIEVEMENTS)
	Globals_DisableAchievements();
#endif

#if defined(DEBUG_DEF_DISABLE_LEADERBOARDS)
	Globals_DisableLeaderboards();
#endif
}
static void HandleCvarsFromDebugDefs(void)
{
#if defined(DEBUG_DEF_FORCE_CVAR_SHOW_IN_GAME_TIMER_ON)
	Cvars_SetAsBool(CVARS_USER_IS_SHOWING_IN_GAME_TIMER, true);
#endif
}
#endif

int GameRunner_Run(int argc, char* args[])
{
	if (Game_FirstInit() < 0)
	{
		return Exception_Run("Unable to init platform!", true);
	}

	HandleSpecialArgsFromArgList(argc, args);

#ifdef EDITOR_MODE
	HandleDebugDefs();
#endif

	//HandleDebugFlag(); //UNUSED FOR NOW
	Cvars_LoadInitialCvars();
	HandleCvarsFromArgList(argc, args);
#ifdef EDITOR_MODE
	HandleCvarsFromDebugDefs();
#endif

	GLOBALS_DEBUG_ENGINE_FORCE_LOAD_DATS = Cvars_GetAsBool(CVARS_ENGINE_FORCE_LOAD_DATS); //UNUSED FOR NOW

	Logger_SetLevel(Cvars_GetAsInt(CVARS_ENGINE_LOGGER_LEVEL));

	Service_Init();

	if (Game_SecondInit() < 0)
	{
		return Exception_Run("Unable to init!", true);
	}

	Game_Run();

	return 0;
}
