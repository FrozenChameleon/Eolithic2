/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"
#include "ControllerState.h"

void ControllerStates_Init(void);
void ControllerStates_Dispose(void);
void ControllerStates_Read(void);
void ControllerStates_Write(void);
void ControllerStates_Poll(void);
ControllerState* ControllerStates_GetController(int32_t number);
bool ControllerStates_IsAnyButtonPressedOnAnyController(void);
bool ControllerStates_IsAnyButtonTappedOnAnyController(void);
bool ControllerStates_IsButtonPressedOnAnyController(Button button);
int32_t ControllerStates_GetControllerNumberIfAnyButtonReleased(void);
int32_t ControllerStates_GetControllerNumberIfAnyButtonTapped(void);
void ControllerStates_MuteInput(void);
float ControllerStates_GetHighestAnalogDataValue(int32_t loc);
int32_t ControllerStates_GetMasterRecordingReadFrame(void);
int32_t ControllerStates_GetCurrentMasterRecordingVersion(void);