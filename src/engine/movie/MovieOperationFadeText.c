/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "MovieOperationFadeText.h"

#include "../globals/Align.h"
#include "../render/Color.h"
#include "../render/SpriteBatch.h"
#include "../utils/Utils.h"

void MovieOperationFadeText_Init(MovieOperationFadeText* fade, const char* str, const char* font, Vector2 position, int32_t rampSpeed, int32_t holdTime, bool isTextCentered)
{
	Utils_memset(fade, 0, sizeof(MovieOperationFadeText));

	fade->mType = MOVIE_OPERATION_TYPE_FADE_TEXT;
	fade->mIsCentered = isTextCentered;
	Utils_strlcpy(fade->mStringToShow, str, EE_FILENAME_MAX);
	{
		MString* tempString = NULL;
		Utils_StringReplaceStr(&tempString, fade->mStringToShow, "\\n", "\n");
		Utils_strlcpy(fade->mStringToShow, MString_Text(tempString), EE_FILENAME_MAX);
		MString_Dispose(&tempString);
	}
	Utils_strlcpy(fade->mFont, font, EE_FILENAME_MAX);
	fade->mPosition = position;
	fade->mTimerRamp = Timer_Create(rampSpeed);
	fade->mTimerHold = Timer_Create(holdTime);
	fade->mIsRamping = true;
}

void MovieOperationFadeText_Update(MovieOperationFadeText* fade)
{
	if (fade->mIsRamping)
	{
		if (Timer_Update(&fade->mTimerRamp))
		{
			if (fade->mIsRampingDown)
			{
				fade->mRamp -= 1;
			}
			else
			{
				fade->mRamp += 1;
			}

			if ((fade->mRamp == 3) && !fade->mIsRampingDown)
			{
				fade->mIsRamping = false;
			}
			else if ((fade->mRamp < 0) && fade->mIsRampingDown)
			{
				fade->mIsComplete = true;
			}
		}
	}
	else
	{
		if (Timer_Update(&fade->mTimerHold))
		{
			fade->mIsRampingDown = true;
			fade->mIsRamping = true;
			fade->mRamp = 2;
		}
	}
}
void MovieOperationFadeText_DrawHud(MovieOperationFadeText* fade, SpriteBatch* spriteBatch)
{
	if (fade->mIsComplete)
	{
		return;
	}

	if (fade->mRamp < 1)
	{
		return;
	}

	Color color = COLOR_WHITE;

	switch (fade->mRamp)
	{
	case 1:
		color = COLOR_MUTE_DGRAY;
		break;
	case 2:
		color = COLOR_MUTE_LGRAY;
		break;
	case 3:
		color = COLOR_WHITE;
		break;
	}

	if (fade->mIsCentered)
	{
		SpriteBatch_DrawString2(spriteBatch, fade->mFont, fade->mStringToShow, color, 100, fade->mPosition, ALIGN_CENTER, ALIGN_CENTER);
	}
	else
	{
		SpriteBatch_DrawString(spriteBatch, fade->mFont, fade->mStringToShow, color, 100, fade->mPosition);
	}
}
void MovieOperationFadeText_SpeedUp(MovieOperationFadeText* fade)
{
	if (!fade->mIsRampingDown && fade->mIsRamping)
	{
		fade->mIsRamping = false;
		fade->mRamp = 3;
	}
	else
	{
		fade->mIsComplete = true;
	}
}
