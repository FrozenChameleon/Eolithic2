/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "ActionList.h"
#include "InputAction.h"

typedef struct ControllerState ControllerState;

typedef struct InputPlayer
{
	int32_t _mPlayerNumber;
	int32_t _mIsBlockedForPlayerSelect;
	int32_t _mInputDeviceNumber;
	int32_t _mCounterRumble;
	int32_t _mRumblePriority;
	float _mRumbleLeftMotor;
	float _mRumbleRightMotor;
	InputAction _mActions[ACTIONLIST_LENGTH];
} InputPlayer;

typedef enum InputPlayerDevice
{
	INPUTPLAYER_CONTROLLER_ONE_DEVICE_NUMBER = 0,
	INPUTPLAYER_CONTROLLER_TWO_DEVICE_NUMBER = 1,
	INPUTPLAYER_CONTROLLER_THREE_DEVICE_NUMBER = 2,
	INPUTPLAYER_CONTROLLER_FOUR_DEVICE_NUMBER = 3,
	INPUTPLAYER_KEYBOARD_DEVICE_NUMBER = 9
} InputPlayerDevice;

typedef enum InputPlayerBlockMenu
{
	INPUTPLAYER_BLOCK_MENU_INPUT_OFF = 0,
	INPUTPLAYER_BLOCK_MENU_INPUT_ALLOW_BACK = 1,
	INPUTPLAYER_BLOCK_MENU_INPUT_ALL = 2
} InputPlayerBlockMenu;

void InputPlayer_SetVibration(InputPlayer* ip, float leftMotor, float rightMotor);
void InputPlayer_StopVibrating(InputPlayer* ip);
InputAction* InputPlayer_GetCurrentActions(InputPlayer* ip);
void InputPlayer_Init(InputPlayer* ip, int32_t number);
void InputPlayer_SetInputDevice(InputPlayer* ip, int32_t newDeviceNumber);
InputAction* InputPlayer_GetActionsForBindingsSync(InputPlayer* ip);
int32_t InputPlayer_GetActionsLength(InputPlayer* ip);
void InputPlayer_DetectInputDevice(InputPlayer* ip, bool isOnRelease);
int32_t InputPlayer_GetDeviceNumber(InputPlayer* ip);
bool InputPlayer_IsInputDeviceSet(InputPlayer* ip);
void InputPlayer_RemoveInputDevice(InputPlayer* ip);
bool InputPlayer_IsUsingController(InputPlayer* ip);
bool InputPlayer_IsUsingKeyboard(InputPlayer* ip);
void InputPlayer_ClearInput(InputPlayer* ip);
bool InputPlayer_IsPlayerPressingAnything(InputPlayer* ip);
void InputPlayer_SetBlockMenuInput(InputPlayer* ip, int32_t value);
void InputPlayer_Update(InputPlayer* ip);
InputAction* InputPlayer_GetAction(InputPlayer* ip, const char* name);
ControllerState* InputPlayer_GetController(InputPlayer* ip);
void InputPlayer_Vibrate(InputPlayer* ip, int32_t priority, int32_t frames, float leftMotor, float rightMotor);
bool InputPlayer_MyControllerLostConnection(InputPlayer* ip);
#ifdef EDITOR_MODE
void InputPlayer_DebugSetDevice(InputPlayer* ip, int32_t deviceNumber);
#endif