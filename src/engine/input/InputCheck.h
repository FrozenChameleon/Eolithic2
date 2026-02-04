/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "Keys.h"
#include "Buttons.h"
#include "Axes.h"
#include "MouseButtons.h"

typedef struct InputPlayer InputPlayer;
typedef struct BufferReader BufferReader;
typedef struct DynamicByteBuffer DynamicByteBuffer;

typedef enum InputCheckType
{
	INPUTCHECK_TYPE_DUMMY = 0,
	INPUTCHECK_TYPE_AXIS = 1,
	INPUTCHECK_TYPE_BUTTON = 2,
	INPUTCHECK_TYPE_KEY = 3,
	INPUTCHECK_TYPE_MOUSEBUTTON = 4
} InputCheckType;

typedef struct InputCheck
{
	InputCheckType mType;
	MouseButton mMouseButton;
	Axis mAxis;
	int32_t mAxisDirection;
	float mAnalogValue;
	Key mKey;
	Button mButton;
} InputCheck;

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
InputCheck InputCheck_CreateCheckKey(Key key);
InputCheck InputCheck_CreateCheckMouseButton(MouseButton mouseButton);
InputCheck InputCheck_CreateCheckButton(Button button);
InputCheck InputCheck_CreateCheckAxis(Axis axis, int32_t direction);