/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "PlayerIndex.h"
#include "Axes.h"
#include "Buttons.h"
#include "stdint.h"
#include "stdbool.h"
#include "../math/Vector2.h"

typedef struct ControllerData
{
	PlayerIndex _mIndex;
	bool _mIsConnected;
	float _mAnalogData[AXES_AMOUNT_OF_AXIS];
	bool _mIsButtonPressed[BUTTONS_AMOUNT_OF_BUTTONS];
} ControllerData;

void ControllerData_PollControllerStatus(ControllerData* cd, PlayerIndex index);
void ControllerData_PollInput(ControllerData* cd, bool* isButtonMutedArray, bool* isAnalogMutedArray);
bool ControllerData_IsConnected(const ControllerData* cd);
float ControllerData_GetAnalogData(const ControllerData* cd, int32_t loc);
float ControllerData_GetLeftStickX(const ControllerData* cd);
float ControllerData_GetLeftStickY(const ControllerData* cd);
Vector2 ControllerData_GetLeftStick(const ControllerData* cd);
float ControllerData_GetRightStickX(const ControllerData* cd);
float ControllerData_GetRightStickY(const ControllerData* cd);
Vector2 ControllerData_GetRightStick(const ControllerData* cd);
float ControllerData_GetTriggerRight(const ControllerData* cd);
float ControllerData_GetTriggerLeft(const ControllerData* cd);
void ControllerData_Clear(ControllerData* cd);
void ControllerData_CopyFrom(ControllerData* cd, const ControllerData* otherData);
bool ControllerData_IsButtonPressedAtLoc(const ControllerData* cd, int32_t loc);
bool ControllerData_IsButtonPressed(const ControllerData* cd, Button button);

typedef struct ControllerState
{
	bool _mIsButtonMuted[BUTTONS_AMOUNT_OF_BUTTONS];
	bool _mIsAnalogMuted[AXES_AMOUNT_OF_AXIS];
	PlayerIndex _mIndex;
	ControllerData _mThisFrame;
	ControllerData _mLastFrame;
} ControllerState;

enum
{
	CONTROLLER_STATE_AMOUNT_OF_ANALOG_DATA = 6,
};

void ControllerState_Init(ControllerState* cs, PlayerIndex index);
void ControllerState_MuteInput(ControllerState* cs);
Vector2 ControllerState_GetLeftStick(const ControllerState* cs);
Vector2 ControllerState_GetRightStick(const ControllerState* cs);
float ControllerState_GetTriggerRight(const ControllerState* cs);
float ControllerState_GetTriggerLeft(const ControllerState* cs);
float ControllerState_GetAnalogData(const ControllerState* cs, int32_t loc);
void ControllerState_SetVibration(ControllerState* cs, float leftMotor, float rightMotor);
bool ControllerState_IsButtonPressed(const ControllerState* cs, Button button);
bool ControllerState_IsButtonTapped(const ControllerState* cs, Button button);
bool ControllerState_IsButtonReleased(const ControllerState* cs, Button button);
int32_t ControllerState_IsAnalogTapped(const ControllerState* cs, int32_t loc, float threshold);
int32_t ControllerState_IsAnalogReleased(const ControllerState* cs, int32_t loc, float threshold);
bool ControllerState_IsAnyButtonReleased(const ControllerState* cs);
bool ControllerState_IsAnyButtonTapped(const ControllerState* cs);
bool ControllerState_IsAnyButtonPressed(const ControllerState* cs);
void ControllerState_Poll(ControllerState* cs);
bool ControllerState_ControllerConnectionWasLost(const ControllerState* cs);
int32_t ControllerState_AddControllerInstance(int32_t dev);
int32_t ControllerState_RemoveControllerInstance(int32_t dev);
void ControllerState_BeforePollingUpdate(void);
bool ControllerState_PlatformUsesControllerPrompt(void);
bool ControllerState_GetControllerPrompt(int32_t minPlayer, int32_t maxPlayer, bool retainConnection, bool singlePlayer, bool spinlockUntilSuccess);
bool ControllerState_PlatformIsForcingCustomInputActions(int32_t player);
//static std::vector<OeInputAction>& ControllerState_GetForcedCustomInputActions(int32_t player);
bool ControllerState_IsUsingControllerForbiddenToFullRebinding(void);
bool ControllerState_IsUsingControllerPrompt(void);
const char* ControllerState_PlatformGetForcedSpecificGlyphAsString(void);