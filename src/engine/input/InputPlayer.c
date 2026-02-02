/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "InputPlayer.h"

#include "../utils/Macros.h"
#include "InputBindings.h"
#include "ActionList.h"
#include "Input.h"
#include "../utils/Logger.h"
#include "../core/GameUpdater.h"
#include "../utils/Cvars.h"
#include "ControllerState.h"
#include "KeyboardState.h"
#include "MouseState.h"
#include "ControllerStates.h"
#include "../service/Service.h"
#include "../gamestate/GameStateManager.h"
#include "../gamestate/GameState.h"
#include "../utils/Utils.h"
#include "InputAction.h"
#include "../utils/MString.h"

InputAction* InputPlayer_GetCurrentActions(InputPlayer* ip)
{
	/*if (ControllerState_PlatformIsForcingCustomInputActions(ip->_mPlayerNumber)) //UNUSED FOR NOW
	{
		return ControllerState_GetForcedCustomInputActions(ip->_mPlayerNumber);
	}*/
	return ip->_mActions;
}

void InputPlayer_SetInputDevice(InputPlayer* ip, int32_t newDeviceNumber)
{
	if (newDeviceNumber == ip->_mInputDeviceNumber)
	{
		return;
	}

	if ((ip->_mInputDeviceNumber == INPUTPLAYER_CONTROLLER_ONE_DEVICE_NUMBER) || (ip->_mInputDeviceNumber == INPUTPLAYER_CONTROLLER_TWO_DEVICE_NUMBER)
		|| (ip->_mInputDeviceNumber == INPUTPLAYER_CONTROLLER_THREE_DEVICE_NUMBER) || (ip->_mInputDeviceNumber == INPUTPLAYER_CONTROLLER_FOUR_DEVICE_NUMBER))
	{
		InputPlayer_StopVibrating(ip);
	}

	if (ip->_mInputDeviceNumber == -1)
	{
		MString* tempString = NULL;
		MString_AssignString(&tempString, "Input device set for Player #");
		MString_AddAssignInt(&tempString, ip->_mPlayerNumber + 1);
		MString_AddAssignString(&tempString, ", Device #");
		MString_AddAssignInt(&tempString, newDeviceNumber);
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}

	ip->_mInputDeviceNumber = newDeviceNumber;
}
void InputPlayer_SetVibration(InputPlayer* ip, float leftMotor, float rightMotor)
{
	if (GameUpdater_IsDebugAutoSpeedOn() || Cvars_GetAsBool(CVARS_USER_IS_RUMBLE_DISABLED) || !InputPlayer_IsUsingController(ip))
	{
		return;
	}

	ControllerState* controller = InputPlayer_GetController(ip);
	ControllerState_SetVibration(controller, leftMotor, rightMotor);
}
void InputPlayer_StopVibrating(InputPlayer* ip)
{
	if (!InputPlayer_IsUsingController(ip))
	{
		return;
	}

	ControllerState* controller = InputPlayer_GetController(ip);
	ControllerState_SetVibration(controller, 0, 0);
}

void InputPlayer_Init(InputPlayer* ip, int32_t number)
{
	Utils_memset(ip, 0, sizeof(InputPlayer));

	ip->_mPlayerNumber = number;
	//ip->_mIsBlockedForPlayerSelect = 0;
	ip->_mInputDeviceNumber = -1;
	//ip->_mCounterRumble = 0;
	ip->_mRumblePriority = -1;
	//ip->_mRumbleLeftMotor = 0;
	//ip->_mRumbleRightMotor = 0;
	//ip->_mActions = std_vector<InputAction>(OeActionList_GetArray().size());
	for (int32_t i = 0; i < ACTIONLIST_LENGTH; i += 1)
	{
		InputAction* currentAction = &ip->_mActions[i];
		InputAction_Init(ActionList_GetAction(i), currentAction);
	}
}

InputAction* InputPlayer_GetActionsForBindingsSync(InputPlayer* ip)
{
	return ip->_mActions;
}
int32_t InputPlayer_GetActionsLength(InputPlayer* ip)
{
	return ACTIONLIST_LENGTH;
}
void InputPlayer_DetectInputDevice(InputPlayer* ip, bool isOnRelease)
{
	bool isKeyboardUsed;
	if (isOnRelease)
	{
		isKeyboardUsed = KeyboardState_IsAnyKeyReleased() || MouseState_IsAnyButtonReleased();
	}
	else
	{
		isKeyboardUsed = KeyboardState_IsAnyKeyTapped() || MouseState_IsAnyButtonTapped();
	}
	if (isKeyboardUsed)
	{
		if (!Input_IsAnyPlayerUsingThisDeviceNumber(INPUTPLAYER_KEYBOARD_DEVICE_NUMBER))
		{
			InputPlayer_SetInputDevice(ip, INPUTPLAYER_KEYBOARD_DEVICE_NUMBER);
			return;
		}
	}

	int32_t controllerNumber;
	if (isOnRelease)
	{
		controllerNumber = ControllerStates_GetControllerNumberIfAnyButtonReleased();
	}
	else
	{
		controllerNumber = ControllerStates_GetControllerNumberIfAnyButtonTapped();
	}
	if (controllerNumber != -1)
	{
		if (!Input_IsAnyPlayerUsingThisDeviceNumber(controllerNumber))
		{
			InputPlayer_SetInputDevice(ip, controllerNumber);
			return;
		}
	}
}
int32_t InputPlayer_GetDeviceNumber(InputPlayer* ip)
{
	return ip->_mInputDeviceNumber;
}
bool InputPlayer_IsInputDeviceSet(InputPlayer* ip)
{
	return ip->_mInputDeviceNumber != -1;
}
void InputPlayer_RemoveInputDevice(InputPlayer* ip)
{
	if (ip->_mInputDeviceNumber == -1)
	{
		return;
	}

	{
		MString* tempString = NULL;
		MString_AssignString(&tempString, "Input device removed for Player #");
		MString_AddAssignInt(&tempString, ip->_mPlayerNumber + 1);
		MString_AddAssignString(&tempString, ", Device #");
		MString_AddAssignInt(&tempString, ip->_mInputDeviceNumber);
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}

	InputPlayer_SetInputDevice(ip, -1);
}
bool InputPlayer_IsUsingController(InputPlayer* ip)
{
	if ((ip->_mInputDeviceNumber == -1) || (ip->_mInputDeviceNumber == INPUTPLAYER_KEYBOARD_DEVICE_NUMBER))
	{
		return false;
	}
	else
	{
		return true;
	}
}
bool InputPlayer_IsUsingKeyboard(InputPlayer* ip)
{
	return (ip->_mInputDeviceNumber == INPUTPLAYER_KEYBOARD_DEVICE_NUMBER);
}
void InputPlayer_ClearInput(InputPlayer* ip)
{
	InputAction* currentActions = InputPlayer_GetCurrentActions(ip);
	for (int32_t i = 0; i < ACTIONLIST_LENGTH; i += 1)
	{
		InputAction_ClearPolledInput(&currentActions[i]);
	}
}
bool InputPlayer_IsPlayerPressingAnything(InputPlayer* ip)
{
	InputAction* currentActions = InputPlayer_GetCurrentActions(ip);
	for (int32_t i = 0; i < ACTIONLIST_LENGTH; i += 1)
	{
		InputAction* action = &currentActions[i];
		for (int32_t j = 0; j < INPUTCHECKS_LENGTH; j += 1)
		{
			if (action->mIsPressed)
			{
				return true;
			}
		}
	}
	return false;
}
void InputPlayer_SetBlockMenuInput(InputPlayer* ip, int32_t value)
{
	ip->_mIsBlockedForPlayerSelect = value;
}
void InputPlayer_Update(InputPlayer* ip)
{
	InputAction* currentActions = InputPlayer_GetCurrentActions(ip);
	for (int32_t i = 0; i < ACTIONLIST_LENGTH; i += 1)
	{
		InputAction_Update(&currentActions[i], ip);
	}

	if (ip->_mCounterRumble > 0)
	{
		ip->_mCounterRumble -= 1;

		if (ip->_mCounterRumble <= 0)
		{
			InputPlayer_StopVibrating(ip);
			ip->_mCounterRumble = 0;
			ip->_mRumblePriority = -1;
		}
		else
		{
			InputPlayer_SetVibration(ip, ip->_mRumbleLeftMotor, ip->_mRumbleRightMotor);
		}
	}

	if (ip->_mIsBlockedForPlayerSelect != INPUTPLAYER_BLOCK_MENU_INPUT_OFF)
	{
		InputAction_ClearPolledInput(InputPlayer_GetAction(ip, ACTIONLIST_GAME_MENU_UP));
		InputAction_ClearPolledInput(InputPlayer_GetAction(ip, ACTIONLIST_GAME_MENU_RIGHT));
		InputAction_ClearPolledInput(InputPlayer_GetAction(ip, ACTIONLIST_GAME_MENU_DOWN));
		InputAction_ClearPolledInput(InputPlayer_GetAction(ip, ACTIONLIST_GAME_MENU_LEFT));
		InputAction_ClearPolledInput(InputPlayer_GetAction(ip, ACTIONLIST_GAME_MENU_SELECT));
		if (ip->_mIsBlockedForPlayerSelect == INPUTPLAYER_BLOCK_MENU_INPUT_ALL)
		{
			InputAction_ClearPolledInput(InputPlayer_GetAction(ip, ACTIONLIST_GAME_MENU_BACK));
		}
	}
}
InputAction* InputPlayer_GetAction(InputPlayer* ip, const char* name)
{
	return InputBindings_GetActionFromArray(InputPlayer_GetCurrentActions(ip), name);
}
ControllerState* InputPlayer_GetController(InputPlayer* ip)
{
	return Input_GetController(ip->_mInputDeviceNumber);
}


void InputPlayer_Vibrate(InputPlayer* ip, int32_t priority, int32_t frames, float leftMotor, float rightMotor)
{
#ifdef DEBUG_DEF_DISABLE_RUMBLE
	return;
#else
	if (GameUpdater_IsDebugAutoSpeedOn() || Cvars_GetAsBool(CVARS_USER_IS_RUMBLE_DISABLED) || !InputPlayer_IsUsingController(ip)
		|| (priority < ip->_mRumblePriority) || GameState_IsRewinding(GameStateManager_GetGameState()))
	{
		return;
	}

	InputPlayer_SetVibration(ip, leftMotor, rightMotor);
	ip->_mRumblePriority = priority;
	ip->_mCounterRumble = frames;
	ip->_mRumbleLeftMotor = leftMotor;
	ip->_mRumbleRightMotor = rightMotor;
#endif
}

bool InputPlayer_MyControllerLostConnection(InputPlayer* ip)
{
	if (!InputPlayer_IsUsingController(ip))
	{
		return false;
	}

	ControllerState* controller = InputPlayer_GetController(ip);
	if (ControllerState_ControllerConnectionWasLost(controller))
	{
		return true;
	}

	return false;
}
#ifdef EDITOR_MODE
void InputPlayer_DebugSetDevice(InputPlayer* ip, int32_t deviceNumber)
{
	ip->_mInputDeviceNumber = deviceNumber;
}
#endif