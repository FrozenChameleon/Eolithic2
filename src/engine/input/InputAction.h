/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Timer.h"
#include "InputChecks.h"
#include "../math/Rectangle.h"
#include "../math/Vector2.h"
#include "../render/Color.h"

typedef struct BufferWriter BufferWriter;
typedef struct BufferReader BufferReader;
typedef struct SpriteBatch SpriteBatch;
typedef struct DynamicByteBuffer DynamicByteBuffer;

enum
{
	INPUTACTION_DOUBLE_TAP_LENGTH = 12
};

typedef struct InputAction
{
	int32_t mTimeHeld;
	float mAnalogValue;
	bool mWasAnalog;
	bool mIsPressed;
	bool mIsPressedLastFrame;
	bool mIsPressedOrReleased;
	bool mIsTapped;
	bool mIsDoubleTapped;
	bool mIsReleased;
	bool mIsCheckDoubleTap;
	char mName[EE_FILENAME_MAX];
	Timer mTimerDoubleTap;
	InputChecks mChecks;
} InputAction;

extern InputAction INPUTACTION_DUMMY_ACTION;

void InputAction_ClearPolledInput(InputAction* action);
void InputAction_Init(const char* name, InputAction* action);
void InputAction_Write(InputAction* action, DynamicByteBuffer* writer);
void InputAction_Read(InputAction* action, BufferReader* reader);
void InputAction_Update(InputAction* action, InputPlayer* input);
void InputAction_DrawCurrentGlyph(InputAction* action, SpriteBatch* spriteBatch, int32_t depth, const char* font,
	Color color, bool centerX, bool centerY, int32_t alignmentX, int32_t alignmentY, float x, float y, Vector2 scale, bool forceControllerGlyph);
void InputAction_DrawCurrentGlyph2(InputAction* action, SpriteBatch* spriteBatch, int32_t depth, const char* font,
	Color colorForText, Color colorForSheet, bool centerX, bool centerY, int32_t alignmentX, int32_t alignmentY, float x, float y,
	Vector2 scale, bool forceControllerGlyph);
Rectangle InputAction_GetCurrentGlyphRectangle(InputAction* action, const char* font, bool forceControllerGlyph);
Rectangle InputAction_GetGlyphRectangle(InputAction* action, int32_t index, const char* font);
void InputAction_DrawGlyph(InputAction* action, int32_t index, SpriteBatch* spriteBatch, int32_t depth, const char* font,
	Color color, bool centerX, bool centerY, int32_t alignmentX, int32_t alignmentY, float x, float y, Vector2 scale);
const char* InputAction_ToString(InputAction* action);