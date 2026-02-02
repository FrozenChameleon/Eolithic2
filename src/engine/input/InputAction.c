/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "InputAction.h"

#include "Input.h"
#include "../io/BufferReader.h"
#include "../render/SpriteBatch.h"
#include "../core/Game.h"
#include "../service/Service.h"
#include "InputPlayer.h"
#include "../render/Sheet.h"
#include "../render/DrawTool.h"
#include "../utils/Utils.h"

InputAction INPUTACTION_DUMMY_ACTION;

void InputAction_ClearPolledInput(InputAction* action)
{
	action->mTimeHeld = 0;
	action->mAnalogValue = 0;
	action->mWasAnalog = false;
	action->mIsPressed = false;
	action->mIsPressedLastFrame = false;
	action->mIsPressedOrReleased = false;
	action->mIsTapped = false;
	action->mIsDoubleTapped = false;
	action->mIsReleased = false;
	action->mIsCheckDoubleTap = false;
	Timer_Reset(&action->mTimerDoubleTap);
}
void InputAction_Init(const char* name, InputAction* action)
{
	Utils_memset(action, 0, sizeof(InputAction));
	Utils_strlcpy(action->mName, name, EE_FILENAME_MAX);
	action->mTimerDoubleTap = Timer_Create(INPUTACTION_DOUBLE_TAP_LENGTH);
}
void InputAction_Write(InputAction* action, DynamicByteBuffer* writer)
{
	DynamicByteBuffer_WriteString(writer, action->mName, EE_FILENAME_MAX);

	DynamicByteBuffer_WriteI32(writer, INPUTCHECKS_LENGTH);
	for (int32_t i = 0; i < INPUTCHECKS_LENGTH; i += 1)
	{
		InputCheck_Write(InputChecks_Get(&action->mChecks, i), writer);
	}
}
void InputAction_Read(InputAction* action, BufferReader* reader)
{
	BufferReader_ReadString(reader, action->mName, EE_FILENAME_MAX);

	int32_t count = BufferReader_ReadI32(reader);
	for (int32_t i = 0; i < count; i += 1)
	{
		InputCheck_Read(InputChecks_Get(&action->mChecks, i), reader);
	}
}
void InputAction_Update(InputAction* action, InputPlayer* input)
{
	action->mIsPressedLastFrame = action->mIsPressed;
	action->mIsPressed = false;
	action->mIsTapped = false;
	action->mIsReleased = false;
	action->mAnalogValue = 0;
	action->mWasAnalog = false;

	for (int32_t i = 0; i < INPUTCHECKS_LENGTH; i += 1)
	{
		InputCheck* check = InputChecks_Get(&action->mChecks, i);

		InputCheck_UpdateAnalogValue(input, check);

		if (INPUT_FLAG_IS_ON_MENU)
		{
			if (check->mType == INPUTCHECK_TYPE_MOUSEBUTTON)
			{
				continue;
			}
		}

		if (InputCheck_Check(input, check))
		{
			action->mIsPressed = true;
			if (InputCheck_IsAnalog(check))
			{
				action->mWasAnalog = true;
			}
			else
			{
				action->mWasAnalog = false;
				action->mAnalogValue = 1;
				break;
			}
		}
	}

	for (int32_t i = 0; i < INPUTCHECKS_LENGTH; i += 1)
	{
		InputCheck* check = InputChecks_Get(&action->mChecks, i);
		if (InputCheck_IsAnalog(check))
		{
			if (check->mAnalogValue > action->mAnalogValue) //Save highest analog value
			{
				action->mAnalogValue = check->mAnalogValue;
			}
		}
	}

	if (action->mIsPressed && !action->mIsPressedLastFrame)
	{
		action->mIsTapped = true;
	}

	if (!action->mIsPressed && action->mIsPressedLastFrame)
	{
		action->mIsReleased = true;
	}

	if (action->mIsPressed) // Get amount held in press.
	{
		action->mTimeHeld += 1;
	}
	else
	{
		action->mTimeHeld = 0;
	}

	if (action->mIsDoubleTapped)
	{
		action->mIsDoubleTapped = false;
		action->mIsCheckDoubleTap = false;
		Timer_Reset(&action->mTimerDoubleTap);
	}
	else if (action->mIsCheckDoubleTap) // Check for double tap.
	{
		if (action->mIsTapped)
		{
			action->mIsDoubleTapped = true;
		}
		else if (Timer_Update(&action->mTimerDoubleTap))
		{
			action->mIsDoubleTapped = false;
			action->mIsCheckDoubleTap = false;
		}
	}

	if (action->mIsTapped)
	{
		action->mIsCheckDoubleTap = true;
	}

	action->mIsPressedOrReleased = action->mIsPressed || action->mIsReleased;
}
void InputAction_DrawCurrentGlyph(InputAction* action, SpriteBatch* spriteBatch, int32_t depth, const char* font,
	Color color, bool centerX, bool centerY, int32_t alignmentX, int32_t alignmentY, float x, float y, Vector2 scale, bool forceControllerGlyph)
{
	InputAction_DrawCurrentGlyph2(action, spriteBatch, depth, font, color, color, centerX, centerY, alignmentX, alignmentY, x, y, scale, forceControllerGlyph);
}
void InputAction_DrawCurrentGlyph2(InputAction* action, SpriteBatch* spriteBatch, int32_t depth, const char* font,
	Color colorForText, Color colorForSheet, bool centerX, bool centerY, int32_t alignmentX, int32_t alignmentY, float x, float y,
	Vector2 scale, bool forceControllerGlyph)
{
	bool isController = InputPlayer_IsUsingController(Input_GetPlayerOne());
	if (forceControllerGlyph || Service_PlatformForcesControllerGlyphs())
	{
		isController = true;
	}
	for (int32_t i = 0; i < INPUTCHECKS_LENGTH; i += 1)
	{
		InputCheck* check = InputChecks_Get(&action->mChecks, i);
		if (!InputCheck_IsDummy(check))
		{
			bool isCheckController = InputCheck_IsController(check);
			if ((isController && isCheckController) || (!isController && !isCheckController))
			{
				Color color = colorForText;
				if (isController)
				{
					color = colorForSheet;
				}
				InputAction_DrawGlyph(action, i, spriteBatch, depth, font, color, centerX, centerY, alignmentX, alignmentY, x, y, scale);
				return;
			}
		}
	}
}
Rectangle InputAction_GetCurrentGlyphRectangle(InputAction* action, const char* font, bool forceControllerGlyph)
{
	bool isController = InputPlayer_IsUsingController(Input_GetPlayerOne());
	if (forceControllerGlyph || Service_PlatformForcesControllerGlyphs())
	{
		isController = true;
	}
	for (int32_t i = 0; i < INPUTCHECKS_LENGTH; i += 1)
	{
		InputCheck* check = InputChecks_Get(&action->mChecks, i);
		if (!InputCheck_IsDummy(check))
		{
			bool isCheckController = InputCheck_IsController(check);
			if ((isController && isCheckController) || (!isController && !isCheckController))
			{
				return InputAction_GetGlyphRectangle(action, i, font);
			}
		}
	}
	return Rectangle_Create(0, 0, 0, 0);
}
Rectangle InputAction_GetGlyphRectangle(InputAction* action, int32_t index, const char* font)
{
	InputCheck* check = InputChecks_Get(&action->mChecks, index);
	const char* glyphString = InputCheck_GetGlyphString(check);
	if (InputCheck_IsGlyphImage(check))
	{
		Sheet* sheet = Sheet_GetSheet(glyphString);
		Rectangle rect = sheet->mRectangle;
		return Rectangle_Create(0, 0, rect.Width, rect.Height);
	}
	else
	{
		MString* tempString = NULL;
		MString_AssignString(&tempString, "[");
		MString_AddAssignString(&tempString, glyphString);
		MString_AddAssignString(&tempString, "]");
		Rectangle rect = DrawTool_GetBounds(MString_Text(tempString), font);
		MString_Dispose(&tempString);
		return rect;
	}
}
void InputAction_DrawGlyph(InputAction* action, int32_t index, SpriteBatch* spriteBatch, int32_t depth, const char* font, Color color, bool centerX, bool centerY, int32_t alignmentX, int32_t alignmentY, float x, float y, Vector2 scale)
{
	InputCheck* check = InputChecks_Get(&action->mChecks, index);
	const char* glyphString = InputCheck_GetGlyphString(check);
	if (InputCheck_IsGlyphImage(check))
	{
		Sheet_Draw3(Sheet_GetSheet(glyphString), spriteBatch, color, depth, centerX, centerY, NULL, Vector2_Create(x, y), scale, 0);
	}
	else
	{
		MString* tempString = NULL;
		MString_AssignString(&tempString, "[");
		MString_AddAssignString(&tempString, glyphString);
		MString_AddAssignString(&tempString, "]");
		SpriteBatch_DrawString3(spriteBatch, font, MString_Text(tempString), color, depth, Vector2_Create(x, y), alignmentX, alignmentY, false);
		MString_Dispose(&tempString);
	}
}
const char* InputAction_ToString(InputAction* action)
{
	return action->mName;
}

