/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"

typedef struct InputPlayer InputPlayer;
typedef struct BufferReader BufferReader;
typedef struct DynamicByteBuffer DynamicByteBuffer;

typedef struct InputCheck
{
	int32_t mType;
	int32_t mMouseButton;
	int32_t mAxis;
	int32_t mAxisDirection;
	float mAnalogValue;
	int32_t mKey;
	int32_t mButton;
} InputCheck;

enum InputCheckType
{
	INPUTCHECK_TYPE_DUMMY = 0,
	INPUTCHECK_TYPE_AXIS = 1,
	INPUTCHECK_TYPE_BUTTON = 2,
	INPUTCHECK_TYPE_KEY = 3,
	INPUTCHECK_TYPE_MOUSEBUTTON = 4
};

enum
{
	INPUTCHECK_AXIS_AMOUNT = 6
};

enum InputCheckAxisInfo
{
	INPUTCHECK_AXIS_LEFT_STICK_X = 0,
	INPUTCHECK_AXIS_LEFT_STICK_Y = 1,
	INPUTCHECK_AXIS_RIGHT_STICK_X = 2,
	INPUTCHECK_AXIS_RIGHT_STICK_Y = 3,
	INPUTCHECK_AXIS_LEFT_TRIGGER = 4,
	INPUTCHECK_AXIS_RIGHT_TRIGGER = 5
};

float InputCheck_CorrectAnalogForDirection(int32_t direction, float value);
bool InputCheck_CheckTypeAxis(InputCheck* data);
bool InputCheck_IsAxisTriggers(InputCheck* data);
bool InputCheck_CheckTypeButton(InputPlayer* playerInput, InputCheck* data);
bool InputCheck_CheckTypeMouseButton(InputPlayer* playerInput, InputCheck* data);
bool InputCheck_CheckTypeKey(InputPlayer* playerInput, InputCheck* data);
const char* InputCheck_GetNameTypeAxis(InputCheck* data);
const char* InputCheck_GetNameTypeButton(InputCheck* data);
const char* InputCheck_GetNameTypeMouseButton(InputCheck* data);
const char* InputCheck_GetGlyphStringTypeKey(InputCheck* data);
bool InputCheck_IsDummy(InputCheck* data);
bool InputCheck_Check(InputPlayer* playerInput, InputCheck* data);
void InputCheck_UpdateAnalogValue(InputPlayer* playerInput, InputCheck* data);
bool InputCheck_IsAnalog(InputCheck* data);
const char* InputCheck_GetName(InputCheck* data);
void InputCheck_Write(InputCheck* data, DynamicByteBuffer* writer);
void InputCheck_Read(InputCheck* data, BufferReader* reader);
bool InputCheck_IsGlyphImage(InputCheck* data);
bool InputCheck_IsController(InputCheck* data);
const char* InputCheck_GetGlyphString(InputCheck* data);
const char* InputCheck_GetGlyphStringForController(const char* value);
InputCheck InputCheck_CreateCheckKey(int32_t key);
InputCheck InputCheck_CreateCheckMouseButton(int32_t mouseButton);
InputCheck InputCheck_CreateCheckButton(int32_t button);
InputCheck InputCheck_CreateCheckAxis(int32_t axis, int32_t direction);