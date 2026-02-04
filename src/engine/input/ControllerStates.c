/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "ControllerStates.h"

#include "Input.h"
#include "../math/Math.h"
#include "../globals/Globals.h"

//static int32_t _mMasterRecordingCurrentVersion = OeGlobals::GetNewestRecordingVersion(); //UNUSED
static int32_t _mMasterRecordingCurrentVersion;
static int32_t _mMasterRecordingReadFrame;

//static BinaryWriter _mWriter; //UNUSED
static bool _mIsReading;
//static std::shared_ptr<BinaryReader> _mReader; //UNUSED
static ControllerState _mControllerStates[INPUT_MAXIMUM_PLAYER_COUNT];
static bool _mHasInit;

void ControllerStates_Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	for (int i = 0; i < INPUT_MAXIMUM_PLAYER_COUNT; i += 1)
	{
		ControllerState_Init(&_mControllerStates[i], (PlayerIndex)i);
	}

	_mHasInit = true;
}
void ControllerStates_Dispose(void)
{
	//UNUSED
}
void ControllerStates_Read(void)
{
	//UNUSED
}
void ControllerStates_Write(void)
{
	//UNUSED
}
void ControllerStates_Poll(void)
{
	for (int32_t i = 0; i < INPUT_MAXIMUM_PLAYER_COUNT; i += 1)
	{
		ControllerState_Poll(&_mControllerStates[i]);
	}
}
ControllerState* ControllerStates_GetController(int32_t number)
{
	return &_mControllerStates[number];
}
bool ControllerStates_IsAnyButtonPressedOnAnyController(void)
{
	for (int32_t i = 0; i < INPUT_MAXIMUM_PLAYER_COUNT; i += 1)
	{
		if (ControllerState_IsAnyButtonPressed(&_mControllerStates[i]))
		{
			return true;
		}
	}
	return false;
}
bool ControllerStates_IsAnyButtonTappedOnAnyController(void)
{
	for (int32_t i = 0; i < INPUT_MAXIMUM_PLAYER_COUNT; i += 1)
	{
		if (ControllerState_IsAnyButtonTapped(&_mControllerStates[i]))
		{
			return true;
		}
	}
	return false;
}
bool ControllerStates_IsButtonPressedOnAnyController(Button button)
{
	for (int32_t i = 0; i < INPUT_MAXIMUM_PLAYER_COUNT; i += 1)
	{
		if (ControllerState_IsButtonPressed(&_mControllerStates[i], button))
		{
			return true;
		}
	}
	return false;
}
int32_t ControllerStates_GetControllerNumberIfAnyButtonReleased(void)
{
	for (int32_t i = 0; i < INPUT_MAXIMUM_PLAYER_COUNT; i += 1)
	{
		if (ControllerState_IsAnyButtonReleased(&_mControllerStates[i]))
		{
			return i;
		}
	}
	return -1;
}
int32_t ControllerStates_GetControllerNumberIfAnyButtonTapped(void)
{
	for (int32_t i = 0; i < INPUT_MAXIMUM_PLAYER_COUNT; i += 1)
	{
		if (ControllerState_IsAnyButtonTapped(&_mControllerStates[i]))
		{
			return i;
		}
	}
	return -1;
}
void ControllerStates_MuteInput(void)
{
	for (int32_t i = 0; i < INPUT_MAXIMUM_PLAYER_COUNT; i += 1)
	{
		ControllerState_MuteInput(&_mControllerStates[i]);
	}
}
float ControllerStates_GetHighestAnalogDataValue(int32_t loc)
{
	int32_t index = -1;
	float highest = -999;
	for (int32_t i = 0; i < INPUT_MAXIMUM_PLAYER_COUNT; i += 1)
	{
		float value = Math_fabsf(ControllerState_GetAnalogData(&_mControllerStates[i], loc));
		if (value > highest)
		{
			highest = value;
			index = i;
		}
	}
	return ControllerState_GetAnalogData(&_mControllerStates[index], loc);
}
int32_t ControllerStates_GetMasterRecordingReadFrame(void)
{
	return _mMasterRecordingReadFrame;
}
int32_t ControllerStates_GetCurrentMasterRecordingVersion(void)
{
	if (!Input_IsPlayingMasterRecording())
	{
		return Globals_GetNewestRecordingVersion();
	}
	else
	{
		return _mMasterRecordingCurrentVersion;
	}
}