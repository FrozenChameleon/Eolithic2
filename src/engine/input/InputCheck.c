/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "InputCheck.h"

#include "MouseState.h"
#include "ControllerStates.h"
#include "KeyboardState.h"
#include "InputPlayer.h"
#include "../math/Math.h"
#include "Input.h"
#include "Axes.h"
#include "../utils/Logger.h"
#include "../utils/Utils.h"
#include "../utils/MString.h"
#include "../io/DynamicByteBuffer.h"
#include "../io/BufferReader.h"
#include "../utils/Strings.h"

static MString* _mTempString;

float InputCheck_CorrectAnalogForDirection(int32_t direction, float value)
{
	int32_t sig = Math_SignumFloat(value);
	if (((direction == -1) && (sig == 1)) || ((direction == 1) && (sig == -1)))
	{
		return 0;
	}
	else
	{
		return value;
	}
}
bool InputCheck_CheckTypeAxis(InputCheck* data)
{
	bool isTriggers = InputCheck_IsAxisTriggers(data);

	float threshold;
	if (isTriggers)
	{
		threshold = Input_GetAnalogToPressThresholdForTriggers();
	}
	else
	{
		threshold = Input_GetAnalogToPressThresholdForSticks();
	}

	if (threshold == 0)
	{
		Logger_LogWarning("No threshold set for stick/trigger");
		threshold = 50;
	}

	if (data->mAnalogValue >= threshold)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool InputCheck_IsAxisTriggers(InputCheck* data)
{
	return (data->mAxis == AXES_LEFT_TRIGGER) || (data->mAxis == AXES_RIGHT_TRIGGER);
}
bool InputCheck_CheckTypeButton(InputPlayer* playerInput, InputCheck* data)
{
	if (Input_IsSimultaneousInputForController())
	{
		return ControllerStates_IsButtonPressedOnAnyController(data->mButton);
	}

	if (!InputPlayer_IsUsingController(playerInput))
	{
		return false;
	}
	if (ControllerState_IsButtonPressed(InputPlayer_GetController(playerInput), data->mButton))
	{
		return true;
	}

	return false;
}
bool InputCheck_CheckTypeMouseButton(InputPlayer* playerInput, InputCheck* data)
{
	if (Input_IsRecordingOrPlayingMasterRecording())
	{
		return false;
	}

	if (Input_IsSimultaneousInputForKeyboard())
	{
		return MouseState_IsButtonPressed(data->mMouseButton);
	}

	if (playerInput != NULL)
	{
		if (!InputPlayer_IsUsingKeyboard(playerInput))
		{
			return false;
		}
	}

	bool valid = false;
	if (MouseState_IsButtonPressed(data->mMouseButton))
	{
		valid = true;
	}

	if (valid)
	{
		return true;
	}

	return false;
}
bool InputCheck_CheckTypeKey(InputPlayer* playerInput, InputCheck* data)
{
	if (Input_IsRecordingOrPlayingMasterRecording())
	{
		return false;
	}

	if (Input_IsSimultaneousInputForKeyboard())
	{
		return KeyboardState_IsKeyPressed(data->mKey);
	}

	if (playerInput != NULL)
	{
		if (!InputPlayer_IsUsingKeyboard(playerInput))
		{
			return false;
		}
	}

	if (KeyboardState_IsKeyPressed(data->mKey))
	{
		return true;
	}

	return false;
}
const char* InputCheck_GetNameTypeAxis(InputCheck* data)
{
	switch (data->mAxis)
	{
	case AXES_LEFT_STICK_X:
		if (data->mAxisDirection >= 0)
		{
			return "LS_RIGHT";
		}
		else
		{
			return "LS_LEFT";
		}
	case AXES_LEFT_STICK_Y:
		if (data->mAxisDirection >= 0)
		{
			return "LS_UP";
		}
		else
		{
			return "LS_DOWN";
		}
	case AXES_RIGHT_STICK_X:
		if (data->mAxisDirection >= 0)
		{
			return "RS_RIGHT";
		}
		else
		{
			return "RS_LEFT";
		}
	case AXES_RIGHT_STICK_Y:
		if (data->mAxisDirection >= 0)
		{
			return "RS_UP";
		}
		else
		{
			return "RS_DOWN";
		}
	case AXES_LEFT_TRIGGER:
		return "LT";
	case AXES_RIGHT_TRIGGER:
		return "RT";
	}
	return "";
}
const char* InputCheck_GetNameTypeButton(InputCheck* data)
{
	switch (data->mButton)
	{
	case BUTTONS_A:
		return "A";
	case BUTTONS_B:
		return "B";
	case BUTTONS_X:
		return "X";
	case BUTTONS_Y:
		return "Y";
	case BUTTONS_LEFTSTICK:
		return "LS";
	case BUTTONS_LEFTSHOULDER:
		return "LB";
		/*case BUTTONS_LeftTrigger:
			return "LT";*/
	case BUTTONS_RIGHTSTICK:
		return "RS";
	case BUTTONS_RIGHTSHOULDER:
		return "RB";
		/*case BUTTONS_RightTrigger:
			return "RT";*/
	case BUTTONS_START:
		return "START";
	case BUTTONS_BACK:
		return "BACK";
	case BUTTONS_DPADUP:
		return "DPAD_UP";
	case BUTTONS_DPADRIGHT:
		return "DPAD_RIGHT";
	case BUTTONS_DPADDOWN:
		return "DPAD_DOWN";
	case BUTTONS_DPADLEFT:
		return "DPAD_LEFT";
		/*case BUTTONS_LeftThumbstickUp:
			return "LS_UP";
		case BUTTONS_LeftThumbstickRight:
			return "LS_RIGHT";
		case BUTTONS_LeftThumbstickDown:
			return "LS_DOWN";
		case BUTTONS_LeftThumbstickLeft:
			return "LS_LEFT";
		case BUTTONS_RightThumbstickUp:
			return "RS_UP";
		case BUTTONS_RightThumbstickRight:
			return "RS_RIGHT";
		case BUTTONS_RightThumbstickDown:
			return "RS_DOWN";
		case BUTTONS_RightThumbstickLeft:
			return "RS_LEFT";*/
	}
	return "?";
}
const char* InputCheck_GetNameTypeMouseButton(InputCheck* data)
{
	switch (data->mMouseButton)
	{
	case MOUSEBUTTONS_LEFTBUTTON:
		return "LMB";
	case MOUSEBUTTONS_RIGHTBUTTON:
		return "RMB";
	case MOUSEBUTTONS_MIDDLEBUTTON:
		return "MMB";
	case MOUSEBUTTONS_XBUTTON1:
		return "MXB1";
	case MOUSEBUTTONS_XBUTTON2:
		return "MXB2";
	}
	return "";
}
const char* InputCheck_GetGlyphStringTypeKey(InputCheck* data)
{
	if (data->mKey == KEYS_OEMSEMICOLON)
	{
		return "Semicolon";
	}
	if (data->mKey == KEYS_OEMPLUS)
	{
		return "Plus";
	}
	if (data->mKey == KEYS_OEMCOMMA)
	{
		return "Comma";
	}
	if (data->mKey == KEYS_OEMMINUS)
	{
		return "Minus";
	}
	if (data->mKey == KEYS_OEMPERIOD)
	{
		return "Period";
	}
	if (data->mKey == KEYS_OEMQUESTION)
	{
		return "/";
	}
	if (data->mKey == KEYS_OEMBACKSLASH)
	{
		return "\\";
	}
	if (data->mKey == KEYS_OEMTILDE)
	{
		return "`";
	}
	if (data->mKey == KEYS_OEMOPENBRACKETS)
	{
		return "[";
	}
	if (data->mKey == KEYS_OEMCLOSEBRACKETS)
	{
		return "]";
	}
	return Keys_GetKeyName((Key)data->mKey);
}
bool InputCheck_IsDummy(InputCheck* data)
{
	return data->mType == INPUTCHECK_TYPE_DUMMY;
}
bool InputCheck_Check(InputPlayer* playerInput, InputCheck* data)
{
	switch (data->mType)
	{
	case INPUTCHECK_TYPE_KEY:
		return InputCheck_CheckTypeKey(playerInput, data);
	case INPUTCHECK_TYPE_MOUSEBUTTON:
		return InputCheck_CheckTypeMouseButton(playerInput, data);
	case INPUTCHECK_TYPE_BUTTON:
		return InputCheck_CheckTypeButton(playerInput, data);
	case INPUTCHECK_TYPE_AXIS:
		return InputCheck_CheckTypeAxis(data);
	}
	return false;
}
void InputCheck_UpdateAnalogValue(InputPlayer* playerInput, InputCheck* data)
{
	data->mAnalogValue = 0;

	if (data->mType != INPUTCHECK_TYPE_AXIS)
	{
		return;
	}

	float currentVal = 0;
	if (Input_IsSimultaneousInputForController())
	{
		currentVal = ControllerStates_GetHighestAnalogDataValue(data->mAxis);
		data->mAnalogValue = Math_fabsf(InputCheck_CorrectAnalogForDirection(data->mAxisDirection, currentVal));
		return;
	}

	if (!InputPlayer_IsUsingController(playerInput))
	{
		return;
	}

	currentVal = ControllerState_GetAnalogData(InputPlayer_GetController(playerInput), data->mAxis);
	data->mAnalogValue = Math_fabsf(InputCheck_CorrectAnalogForDirection(data->mAxisDirection, currentVal));
}
bool InputCheck_IsAnalog(InputCheck* data)
{
	if (data->mType == INPUTCHECK_TYPE_AXIS)
	{
		return true;
	}
	return false;
}
const char* InputCheck_GetName(InputCheck* data)
{
	switch (data->mType)
	{
	case INPUTCHECK_TYPE_KEY:
	{
		MString_AssignString(&_mTempString, Strings_Get("GEN_BIND_KEY"));
		MString_AddAssignString(&_mTempString, ": ");
		MString_AddAssignString(&_mTempString, Keys_GetKeyName((Key)data->mKey));
		return MString_Text(_mTempString);
	}
	case INPUTCHECK_TYPE_MOUSEBUTTON:
	{
		MString_AssignString(&_mTempString, Strings_Get("GEN_BIND_MOUSE"));
		MString_AddAssignString(&_mTempString, ": ");
		MString_AddAssignString(&_mTempString, InputCheck_GetNameTypeMouseButton(data));
		return MString_Text(_mTempString);
	}
	case INPUTCHECK_TYPE_BUTTON:
	{
		MString_AssignString(&_mTempString, "Button: ");
		MString_AddAssignString(&_mTempString, InputCheck_GetNameTypeButton(data));
		return MString_Text(_mTempString);
	}
	case INPUTCHECK_TYPE_AXIS:
		return InputCheck_GetNameTypeAxis(data);
	}
	return Strings_Get("GEN_BIND_NOT_SET");
}
void InputCheck_Write(InputCheck* data, DynamicByteBuffer* writer)
{
	DynamicByteBuffer_WriteI32(writer, data->mType);
	DynamicByteBuffer_WriteI32(writer, data->mKey);
	DynamicByteBuffer_WriteI32(writer, data->mMouseButton);
	DynamicByteBuffer_WriteI32(writer, data->mButton);
	DynamicByteBuffer_WriteI32(writer, data->mAxis);
	DynamicByteBuffer_WriteI32(writer, data->mAxisDirection);
}
void InputCheck_Read(InputCheck* data, BufferReader* reader)
{
	data->mType = (InputCheckType)BufferReader_ReadI32(reader);
	data->mKey = (Key)BufferReader_ReadI32(reader);
	data->mMouseButton = (MouseButton)BufferReader_ReadI32(reader);
	data->mButton = (Button)BufferReader_ReadI32(reader);
	data->mAxis = (Axis)BufferReader_ReadI32(reader);
	data->mAxisDirection = BufferReader_ReadI32(reader);
}
bool InputCheck_IsGlyphImage(InputCheck* data)
{
	if ((data->mType == INPUTCHECK_TYPE_AXIS) || (data->mType == INPUTCHECK_TYPE_BUTTON))
	{
		return true;
	}
	return false;
}
bool InputCheck_IsController(InputCheck* data)
{
	if ((data->mType == INPUTCHECK_TYPE_AXIS) || (data->mType == INPUTCHECK_TYPE_BUTTON))
	{
		return true;
	}
	return false;
}
const char* InputCheck_GetGlyphString(InputCheck* data)
{
	switch (data->mType)
	{
	case INPUTCHECK_TYPE_KEY:
		return InputCheck_GetGlyphStringTypeKey(data);
	case INPUTCHECK_TYPE_MOUSEBUTTON:
		return InputCheck_GetNameTypeMouseButton(data);
	case INPUTCHECK_TYPE_BUTTON:
		return InputCheck_GetGlyphStringForController(InputCheck_GetNameTypeButton(data));
	case INPUTCHECK_TYPE_AXIS:
		return InputCheck_GetGlyphStringForController(InputCheck_GetNameTypeAxis(data));
	}
	return "";
}
const char* InputCheck_GetGlyphStringForController(const char* value)
{
	MString_Combine4(&_mTempString, "GLYPH_", Utils_GetGlyphType(), "_", value);
	return MString_Text(_mTempString);
}
InputCheck InputCheck_CreateCheckKey(Key key)
{
	InputCheck tempCheck = { INPUTCHECK_TYPE_DUMMY };
	tempCheck.mType = INPUTCHECK_TYPE_KEY;
	tempCheck.mKey = key;
	return tempCheck;
}
InputCheck InputCheck_CreateCheckMouseButton(MouseButton mouseButton)
{
	InputCheck tempCheck = { INPUTCHECK_TYPE_DUMMY };
	tempCheck.mType = INPUTCHECK_TYPE_MOUSEBUTTON;
	tempCheck.mMouseButton = mouseButton;
	return tempCheck;
}
InputCheck InputCheck_CreateCheckButton(Button button)
{
	InputCheck tempCheck = { INPUTCHECK_TYPE_DUMMY };
	tempCheck.mType = INPUTCHECK_TYPE_BUTTON;
	tempCheck.mButton = button;
	return tempCheck;
}
InputCheck InputCheck_CreateCheckAxis(Axis axis, int32_t direction)
{
	InputCheck tempCheck = { INPUTCHECK_TYPE_DUMMY };
	tempCheck.mType = INPUTCHECK_TYPE_AXIS;
	tempCheck.mAxis = axis;
	tempCheck.mAxisDirection = direction;
	return tempCheck;
}