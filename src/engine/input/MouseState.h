/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"
#include "../math/Point.h"
#include "../math/Vector2.h"
#include "MouseButtons.h"

typedef struct MouseData
{
	Vector2 _mMousePosition;
	int32_t _mScrollWheelValue;
	bool _mIsButtonDown[MOUSEBUTTONS_AMOUNT_OF_MOUSE_BUTTONS];
} MouseData;


void MouseData_CopyFrom(MouseData* md, const MouseData* otherData);
void MouseData_Clear(MouseData* md);
void MouseData_ClearOnlyButtonData(MouseData* md);
void MouseData_Poll(MouseData* md);
int32_t MouseData_GetScrollWheelValue(const MouseData* md);
float MouseData_GetMouseX(const MouseData* md);
float MouseData_GetMouseY(const MouseData* md);
bool MouseData_IsButtonPressed(const MouseData* md, int32_t button);

void MouseState_Init(void);
void MouseState_MuteInput(void);
bool MouseState_IsButtonPressed(int32_t button);
bool MouseState_IsButtonTapped(int32_t button);
bool MouseState_IsButtonReleased(int32_t button);
bool MouseState_IsAnyButtonReleased(void);
bool MouseState_IsAnyButtonTapped(void);
void MouseState_ClearEverything(void);
void MouseState_ClearOnlyButtonData(void);
void MouseState_Poll(void);
int32_t MouseState_GetScrollWheelValue(void);
float MouseState_GetMouseX(void);
float MouseState_GetMouseY(void);
float MouseState_GetDifferenceMouseX(void);
float MouseState_GetDifferenceMouseY(void);
bool MouseState_JustScrolledUp(void);
bool MouseState_JustScrolledDown(void);
void MouseState_INTERNAL_SetScrollWheelValue(int32_t value);