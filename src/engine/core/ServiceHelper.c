/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "ServiceHelper.h"

#include "../utils/Macros.h"
#include "../core/Game.h"
#include "../service/Service.h"
#include "../input/Input.h"
#include "../input/InputPlayer.h"
#include "../utils/Cvars.h"
#include "../input/ControllerStates.h"
#include "../input/KeyboardState.h"
#include "../input/MouseState.h"
#include "../input/RecordingTool.h"
#include "../gamesave/GameSaveManager.h"
#include "../input/InputBindings.h"

enum { PLAYER_LOST_CONTROLLER_CONNECTION_LENGTH = 4 };

static bool _mPlayerHasLostControllerConnection[PLAYER_LOST_CONTROLLER_CONNECTION_LENGTH];
static bool _mHasSignedInAtSomePoint;
static bool _mHasLoadedEverything;

static void UpdateLostControllerConnectionStatus(void)
{
	if (RecordingTool_IsDisplayingSessionReadout() || Input_IsRecordingOrPlayingMasterRecording())
	{
		return;
	}

	if (!Service_PlatformShowsControllerDisconnectScreen())
	{
		return;
	}

#ifdef DEBUG_DEF_DISABLE_CONTROLLER_SAFETY_CHECKS
	return;
#else
	int32_t len = INPUT_MAXIMUM_PLAYER_COUNT;
	for (int32_t i = 0; i < len; i += 1)
	{
		if (InputPlayer_MyControllerLostConnection(Input_GetPlayer(i)))
		{
			_mPlayerHasLostControllerConnection[i] = true;
		}
	}
#endif
}
static void TrySignIn(void)
{
	if (Service_HasSignedIn())
	{
		return;
	}

	Service_SignIn(true, false);
}
static bool LoadEverythingHelper(void)
{
	bool loadedSaves = false;
	bool loadedConfig = false;
	bool loadedBindings = false;

	if (GameSaveManager_HasInit())
	{
		if (!GameSaveManager_HasLoaded())
		{
			GameSaveManager_Load();
		}
		else
		{
			loadedSaves = true;
		}
	}

	if (!Service_PlatformUsesLocalStorageForSaveData()) //Cvars are loaded immediately if local storage
	{
		if (!Cvars_HasLoadedSaveDataCvars())
		{
			Cvars_LoadSaveCvarsFromBlob();
		}
		else
		{
			loadedConfig = true;
		}
	}
	else
	{
		loadedConfig = true;
	}

	if (Input_HasInit())
	{
		if (!InputBindings_HaveAllPlayersLoadedBindings())
		{
			InputBindings_LoadAllPlayerBindings();
		}
		else
		{
			loadedBindings = true;
		}
	}

	if (loadedSaves && loadedConfig && loadedBindings)
	{
		return true;
	}
	else
	{
		return false;
	}
}
static void LoadEverything(void)
{
	if (!Service_HasSignedIn())
	{
		return;
	}

	_mHasSignedInAtSomePoint = true;
	if (!_mHasLoadedEverything)
	{
		_mHasLoadedEverything = LoadEverythingHelper();
	}
}

void ServiceHelper_Update(double delta)
{
	TrySignIn();

	LoadEverything();

	UpdateLostControllerConnectionStatus();

	Service_Update(delta);
}
bool ServiceHelper_HasPlayerHasLostControllerConnection(void)
{
	int32_t playerThatLostConnection = ServiceHelper_GetPlayerThatLostControllerConnection();
	if (playerThatLostConnection != -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}
int32_t ServiceHelper_GetPlayerThatLostControllerConnection(void)
{
	for (int32_t i = 0; i < PLAYER_LOST_CONTROLLER_CONNECTION_LENGTH; i += 1)
	{
		if (_mPlayerHasLostControllerConnection[i])
		{
			return i;
		}
	}
	return -1;
}
void ServiceHelper_HandlePlayerLostControllerConnection(void)
{
	int32_t playerThatLostConnection = ServiceHelper_GetPlayerThatLostControllerConnection();
	if (playerThatLostConnection == -1)
	{
		return;
	}

	int32_t controllerTapped = ControllerStates_GetControllerNumberIfAnyButtonTapped();
	if ((controllerTapped != -1) || KeyboardState_IsAnyKeyTapped() || MouseState_IsAnyButtonTapped())
	{
		_mPlayerHasLostControllerConnection[playerThatLostConnection] = false;
	}
}
bool ServiceHelper_HasLoadedEverything(void)
{
	return _mHasLoadedEverything;
}
