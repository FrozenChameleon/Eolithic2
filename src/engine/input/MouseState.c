/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "MouseState.h"

#include "SDL3/SDL.h"
#include "../utils/Utils.h"
#include "../service/Service.h"
#include "../utils/Cvars.h"
#include "../core/Game.h"

static int32_t _mGlobalScrollWheelValue;
static bool _mIsButtonMuted[MOUSEBUTTONS_AMOUNT_OF_MOUSE_BUTTONS];
static MouseData _mThisFrame;
static MouseData _mLastFrame;
static bool _mHasInit;

void MouseData_CopyFrom(MouseData* md, const MouseData* otherData)
{
	md->_mMousePosition = otherData->_mMousePosition;
	md->_mScrollWheelValue = otherData->_mScrollWheelValue;
	for (int32_t i = 0; i < MOUSEBUTTONS_AMOUNT_OF_MOUSE_BUTTONS; i += 1)
	{
		md->_mIsButtonDown[i] = otherData->_mIsButtonDown[i];
	}
}
void MouseData_Clear(MouseData* md)
{
	md->_mMousePosition = Vector2_Zero;
	md->_mScrollWheelValue = 0;
	MouseData_ClearOnlyButtonData(md);
}
void MouseData_ClearOnlyButtonData(MouseData* md)
{
	Utils_ResetArrayAsBool(md->_mIsButtonDown, MOUSEBUTTONS_AMOUNT_OF_MOUSE_BUTTONS, false);
}
void MouseData_Poll(MouseData* md)
{
	Uint32 mouseButtonState = SDL_GetMouseState(&md->_mMousePosition.X, &md->_mMousePosition.Y);

	md->_mScrollWheelValue = _mGlobalScrollWheelValue;

	md->_mIsButtonDown[MOUSEBUTTONS_MOUSE_LEFTBUTTON] = (mouseButtonState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT));
	md->_mIsButtonDown[MOUSEBUTTONS_MOUSE_RIGHTBUTTON] = (mouseButtonState & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT));
	md->_mIsButtonDown[MOUSEBUTTONS_MOUSE_MIDDLEBUTTON] = (mouseButtonState & SDL_BUTTON_MASK(SDL_BUTTON_MIDDLE));
	md->_mIsButtonDown[MOUSEBUTTONS_MOUSE_XBUTTON1] = (mouseButtonState & SDL_BUTTON_MASK(SDL_BUTTON_X1));
	md->_mIsButtonDown[MOUSEBUTTONS_MOUSE_XBUTTON2] = (mouseButtonState & SDL_BUTTON_MASK(SDL_BUTTON_X2));

	for (int32_t i = 0; i < MOUSEBUTTONS_AMOUNT_OF_MOUSE_BUTTONS; i += 1)
	{
		if (_mIsButtonMuted[i])
		{
			if (md->_mIsButtonDown[i])
			{
				md->_mIsButtonDown[i] = false;
			}
			else
			{
				_mIsButtonMuted[i] = false;
			}
		}
	}
}
int32_t MouseData_GetScrollWheelValue(const MouseData* md)
{
	return md->_mScrollWheelValue;
}
float MouseData_GetMouseX(const MouseData* md)
{
	return md->_mMousePosition.X;
}
float MouseData_GetMouseY(const MouseData* md)
{
	return md->_mMousePosition.Y;
}
bool MouseData_IsButtonPressed(const MouseData* md, int32_t button)
{
	return md->_mIsButtonDown[button];
}

void MouseState_Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	MouseState_MuteInput();

	_mHasInit = true;
}
void MouseState_MuteInput(void)
{
	Utils_ResetArrayAsBool(_mIsButtonMuted, MOUSEBUTTONS_AMOUNT_OF_MOUSE_BUTTONS, true);
}
bool MouseState_IsButtonPressed(int32_t button)
{
	return MouseData_IsButtonPressed(&_mThisFrame, button);
}
bool MouseState_IsButtonTapped(int32_t button)
{
	return (!MouseData_IsButtonPressed(&_mLastFrame, button) && MouseData_IsButtonPressed(&_mThisFrame, button));
}
bool MouseState_IsButtonReleased(int32_t button)
{
	return (MouseData_IsButtonPressed(&_mLastFrame, button) && !MouseData_IsButtonPressed(&_mThisFrame, button));
}
bool MouseState_IsAnyButtonReleased(void)
{
	int32_t len = MOUSEBUTTONS_AMOUNT_OF_MOUSE_BUTTONS;
	for (int32_t i = 0; i < len; i += 1)
	{
		if (MouseData_IsButtonPressed(&_mLastFrame, i) && !MouseData_IsButtonPressed(&_mThisFrame, i))
		{
			return true;
		}
	}
	return false;
}
bool MouseState_IsAnyButtonTapped(void)
{
	int32_t len = MOUSEBUTTONS_AMOUNT_OF_MOUSE_BUTTONS;
	for (int32_t i = 0; i < len; i += 1)
	{
		if (!MouseData_IsButtonPressed(&_mLastFrame, i) && MouseData_IsButtonPressed(&_mThisFrame, i))
		{
			return true;
		}
	}
	return false;
}
void MouseState_ClearEverything(void)
{
	MouseData_Clear(&_mThisFrame);
	MouseData_Clear(&_mLastFrame);
}
void MouseState_ClearOnlyButtonData(void)
{
	MouseData_ClearOnlyButtonData(&_mThisFrame);
	MouseData_ClearOnlyButtonData(&_mLastFrame);
}
void MouseState_Poll(void)
{
	MouseData_CopyFrom(&_mLastFrame, &_mThisFrame);

	MouseData_Clear(&_mThisFrame);

	if (Cvars_GetAsBool(CVARS_ENGINE_DISABLE_MOUSE) || Service_PlatformDisablesKeyboardUse() ||
		(!Game_IsActive() && Service_PlatformStopsPollingInputWhenGameIsNotActive()))
	{
		return;
	}

	MouseData_Poll(&_mThisFrame);
}
int32_t MouseState_GetScrollWheelValue(void)
{
	return MouseData_GetScrollWheelValue(&_mThisFrame);
}
float MouseState_GetMouseX(void)
{
	return MouseData_GetMouseX(&_mThisFrame);
}
float MouseState_GetMouseY(void)
{
	return MouseData_GetMouseY(&_mThisFrame);
}
float MouseState_GetDifferenceMouseX(void)
{
	return MouseState_GetMouseX() - MouseData_GetMouseX(&_mLastFrame);
}
float MouseState_GetDifferenceMouseY(void)
{
	return MouseState_GetMouseY() - MouseData_GetMouseY(&_mLastFrame);
}
bool MouseState_JustScrolledUp(void)
{
	return MouseData_GetScrollWheelValue(&_mLastFrame) < MouseState_GetScrollWheelValue();
}
bool MouseState_JustScrolledDown(void)
{
	return MouseData_GetScrollWheelValue(&_mLastFrame) > MouseState_GetScrollWheelValue();
}
void MouseState_INTERNAL_SetScrollWheelValue(int32_t value)
{
	_mGlobalScrollWheelValue = value;
}