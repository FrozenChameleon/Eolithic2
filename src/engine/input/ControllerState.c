/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "ControllerState.h"

#include "Buttons.h"
#include "Axes.h"
#include "../utils/Utils.h"
#include "Buttons.h"
#include "../math/Math.h"
#include "../utils/Cvars.h"
#include "../core/Game.h"
#include "../service/Service.h"

bool ControllerData_IsConnected(const ControllerData* cd)
{
	return cd->_mIsConnected;
}
float ControllerData_GetAnalogData(const ControllerData* cd, int32_t loc)
{
	return cd->_mAnalogData[loc];
}
float ControllerData_GetLeftStickX(const ControllerData* cd)
{
	return cd->_mAnalogData[AXES_ANALOG_LEFT_STICK_X];
}
float ControllerData_GetLeftStickY(const ControllerData* cd)
{
	return cd->_mAnalogData[AXES_ANALOG_LEFT_STICK_Y];
}
Vector2 ControllerData_GetLeftStick(const ControllerData* cd)
{
	Vector2 vec;
	vec.X = ControllerData_GetLeftStickX(cd);
	vec.Y = ControllerData_GetLeftStickY(cd);
	return vec;
}
float ControllerData_GetRightStickX(const ControllerData* cd)
{
	return cd->_mAnalogData[AXES_ANALOG_RIGHT_STICK_X];
}
float ControllerData_GetRightStickY(const ControllerData* cd)
{
	return cd->_mAnalogData[AXES_ANALOG_RIGHT_STICK_Y];
}
Vector2 ControllerData_GetRightStick(const ControllerData* cd)
{
	Vector2 vec;
	vec.X = ControllerData_GetRightStickX(cd);
	vec.Y = ControllerData_GetRightStickY(cd);
	return vec;
}
float ControllerData_GetTriggerRight(const ControllerData* cd)
{
	return cd->_mAnalogData[AXES_ANALOG_RIGHT_TRIGGER];
}
float ControllerData_GetTriggerLeft(const ControllerData* cd)
{
	return cd->_mAnalogData[AXES_ANALOG_LEFT_TRIGGER];
}
void ControllerData_Clear(ControllerData* cd)
{
	cd->_mIsConnected = false;
	Utils_ResetArrayAsBool(cd->_mIsButtonPressed, BUTTONS_AMOUNT_OF_BUTTONS, false);
	Utils_ResetArrayAsFloat(cd->_mAnalogData, AXES_AMOUNT_OF_AXIS, 0);
}
void ControllerData_CopyFrom(ControllerData* cd, const ControllerData* otherData)
{
	cd->_mIsConnected = otherData->_mIsConnected;
	for (int32_t i = 0; i < BUTTONS_AMOUNT_OF_BUTTONS; i += 1)
	{
		cd->_mIsButtonPressed[i] = otherData->_mIsButtonPressed[i];
	}
	for (int32_t i = 0; i < AXES_AMOUNT_OF_AXIS; i += 1)
	{
		cd->_mAnalogData[i] = otherData->_mAnalogData[i];
	}
}
bool ControllerData_IsButtonPressedAtLoc(const ControllerData* cd, int32_t loc)
{
	return cd->_mIsButtonPressed[loc];
}
bool ControllerData_IsButtonPressed(const ControllerData* cd, Button button)
{
	for (int32_t i = 0; i < BUTTONS_AMOUNT_OF_BUTTONS; i += 1)
	{
		if (button == Buttons_GetButton(i))
		{
			return ControllerData_IsButtonPressedAtLoc(cd, i);
		}
	}
	return false;
}

static const float ANALOG_MUTE_DEADZONE = 0.5f;

void ControllerState_Init(ControllerState* cs, PlayerIndex index)
{
	ControllerState_MuteInput(cs);

	cs->_mIndex = index;
}
void ControllerState_MuteInput(ControllerState* cs)
{
	Utils_ResetArrayAsBool(cs->_mIsButtonMuted, BUTTONS_AMOUNT_OF_BUTTONS, true);
	Utils_ResetArrayAsBool(cs->_mIsAnalogMuted, AXES_AMOUNT_OF_AXIS, true);
}
Vector2 ControllerState_GetLeftStick(const ControllerState* cs)
{
	return ControllerData_GetLeftStick(&cs->_mThisFrame);
}
Vector2 ControllerState_GetRightStick(const ControllerState* cs)
{
	return ControllerData_GetRightStick(&cs->_mThisFrame);
}
float ControllerState_GetTriggerRight(const ControllerState* cs)
{
	return ControllerData_GetTriggerRight(&cs->_mThisFrame);
}
float ControllerState_GetTriggerLeft(const ControllerState* cs)
{
	return ControllerData_GetTriggerLeft(&cs->_mThisFrame);
}
float ControllerState_GetAnalogData(const ControllerState* cs, int32_t loc)
{
	return ControllerData_GetAnalogData(&cs->_mThisFrame, loc);
}
bool ControllerState_IsButtonPressed(const ControllerState* cs, Button button)
{
	return ControllerData_IsButtonPressed(&cs->_mThisFrame, button);
}
bool ControllerState_IsButtonTapped(const ControllerState* cs, Button button)
{
	return (!ControllerData_IsButtonPressed(&cs->_mLastFrame, button) && ControllerData_IsButtonPressed(&cs->_mThisFrame, button));
}
bool ControllerState_IsButtonReleased(const ControllerState* cs, Button button)
{
	return (ControllerData_IsButtonPressed(&cs->_mLastFrame, button) && !ControllerData_IsButtonPressed(&cs->_mThisFrame, button));
}
int32_t ControllerState_IsAnalogTapped(const ControllerState* cs, int32_t loc, float threshold)
{
	float absLastValue = Math_fabsf(ControllerData_GetAnalogData(&cs->_mLastFrame, loc));
	float thisValue = ControllerData_GetAnalogData(&cs->_mThisFrame, loc);
	float absThisValue = Math_fabsf(thisValue);
	if ((absLastValue < threshold) && (absThisValue >= threshold))
	{
		return Math_SignumFloat(thisValue);
	}
	else
	{
		return 0;
	}
}
int32_t ControllerState_IsAnalogReleased(const ControllerState* cs, int32_t loc, float threshold)
{
	float lastValue = ControllerData_GetAnalogData(&cs->_mLastFrame, loc);
	float absLastValue = Math_fabsf(lastValue);
	float absThisValue = Math_fabsf(ControllerData_GetAnalogData(&cs->_mThisFrame, loc));
	if ((absLastValue >= threshold) && (absThisValue < threshold))
	{
		return Math_SignumFloat(lastValue);
	}
	else
	{
		return 0;
	}
}
bool ControllerState_IsAnyButtonReleased(const ControllerState* cs)
{
	int32_t len = BUTTONS_AMOUNT_OF_BUTTONS;
	for (int32_t i = 0; i < len; i += 1)
	{
		if (ControllerData_IsButtonPressedAtLoc(&cs->_mLastFrame, i) && !ControllerData_IsButtonPressedAtLoc(&cs->_mThisFrame, i))
		{
			return true;
		}
	}
	return false;
}
bool ControllerState_IsAnyButtonTapped(const ControllerState* cs)
{
	int32_t len = BUTTONS_AMOUNT_OF_BUTTONS;
	for (int32_t i = 0; i < len; i += 1)
	{
		if (!ControllerData_IsButtonPressedAtLoc(&cs->_mLastFrame, i) && ControllerData_IsButtonPressedAtLoc(&cs->_mThisFrame, i))
		{
			return true;
		}
	}
	return false;
}
bool ControllerState_IsAnyButtonPressed(const ControllerState* cs)
{
	int32_t len = BUTTONS_AMOUNT_OF_BUTTONS;
	for (int32_t i = 0; i < len; i += 1)
	{
		if (ControllerData_IsButtonPressedAtLoc(&cs->_mThisFrame, i))
		{
			return true;
		}
	}
	return false;
}
void ControllerState_Poll(ControllerState* cs)
{
	ControllerData_CopyFrom(&cs->_mLastFrame, &cs->_mThisFrame);

	ControllerData_Clear(&cs->_mThisFrame);

	ControllerData_PollControllerStatus(&cs->_mThisFrame, cs->_mIndex);

	if (Cvars_GetAsBool(CVARS_ENGINE_DISABLE_CONTROLLER) ||
		(!Game_IsActive() && Service_PlatformStopsPollingInputWhenGameIsNotActive()))
	{
		return;
	}

	ControllerData_PollInput(&cs->_mThisFrame, cs->_mIsButtonMuted, cs->_mIsAnalogMuted);
}
bool ControllerState_ControllerConnectionWasLost(const ControllerState* cs)
{
	return (ControllerData_IsConnected(&cs->_mLastFrame) && !ControllerData_IsConnected(&cs->_mThisFrame));
}