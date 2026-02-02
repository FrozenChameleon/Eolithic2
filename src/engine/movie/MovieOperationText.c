/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "MovieOperationText.h"

#include "../utils/Cvars.h"
#include "../utils/Strings.h"
#include "../render/Color.h"
#include "../utils/Utils.h"
#include "../render/SpriteBatch.h"
#include "../core/Func.h"
#include "../globals/Align.h"
#include "../utils/Logger.h"

char mTextTypeSound[EE_FILENAME_MAX];

int32_t MovieOperationText_GetSpeedToIncrease(MovieOperationText* text)
{
	return Cvars_GetAsInt(CVARS_ENGINE_CUTSCENE_INTL_TEXT_SPEED_INCREASE);
}
void MovieOperationText_EndUpdate(MovieOperationText* text)
{
	if (Timer_Update(&text->mTimerWait))
	{
		text->mIsComplete = true;
	}
}
void MovieOperationText_NormalUpdate(MovieOperationText* text)
{
	if (!text->mIsPaused)
	{
		if (Timer_Update(&text->mTimerAddText))
		{
			if (!Utils_StringEqualTo(mTextTypeSound, EE_STR_EMPTY))
			{
				Do_PlaySound(mTextTypeSound);
			}
			for (int32_t i = 0; i < text->mRate; i += 1)
			{
				if (!text->mIsPaused)
				{
					char newChar = text->mTotalString[text->mIndex];

					MString* tempString = NULL;
					MString_AssignString(&tempString, text->mCurrentString);
					MString_AddAssignChar(&tempString, newChar);
					Utils_strlcpy(text->mCurrentString, MString_Text(tempString), EE_FILENAME_MAX);
					MString_Dispose(&tempString);

					text->mIndex += 1;

					if (text->mIndex == Utils_strlen(text->mTotalString))
					{
						text->mStringShown = true;
						return;
					}

					MovieOperationText_TextCommandsCheck(text, text->mIndex);
				}
			}
		}
	}
	else
	{
		for (int32_t i = 0; i < TEXT_PAUSES_LEN; i += 1)
		{
			Pause* pause = &text->mPauses[i];
			if (!pause->mIsLoaded)
			{
				continue;
			}

			if (Pause_IsAtPoint(pause, text->mIndex))
			{
				Pause_Update(pause);
				if (pause->mIsComplete)
				{
					text->mIsPaused = false;
				}
			}
		}
	}
}
void MovieOperationText_TextCommandsCheck(MovieOperationText* text, int32_t point)
{
	for (int32_t j = 0; j < PLAY_SOUNDS_LEN; j += 1)
	{
		PlaySound* playSound = &text->mPlaySounds[j];
		if (!playSound->mIsLoaded)
		{
			continue;
		}

		PlaySound_PlayTheSoundIfAtPoint(playSound, point);
	}
	for (int32_t j = 0; j < TEXT_PAUSES_LEN; j += 1)
	{
		Pause* pause = &text->mPauses[j];
		if (!pause->mIsLoaded)
		{
			continue;
		}

		if (Pause_IsAtPoint(pause, point))
		{
			text->mIsPaused = true;
		}
	}
}

void MovieOperationText_Init(MovieOperationText* text, bool isMappedText, const char* str, const char* font, Vector2 position,
	int32_t speed, int32_t rate, int32_t wait, const char* color, bool isTextCentered)
{
	Utils_memset(text, 0, sizeof(MovieOperationText));

	text->mType = MOVIE_OPERATION_TYPE_TEXT;

	text->mIsCentered = isTextCentered;

	const char* strToUse = str;
	if (isMappedText)
	{
		strToUse = Strings_Get(str);
	}

	Utils_strlcpy(text->mFont, font, EE_FILENAME_MAX);
	Utils_strlcpy(text->mTotalString, strToUse, EE_FILENAME_MAX);
	{
		MString* tempString = NULL;
		Utils_StringReplaceStr(&tempString, text->mTotalString, "\\n", "\n");
		Utils_strlcpy(text->mTotalString, MString_Text(tempString), EE_FILENAME_MAX);
		MString_Dispose(&tempString);
	}

	bool isReady = false;
	while (!isReady)
	{
		if (Utils_StringContains(text->mTotalString, "{") && Utils_StringContains(text->mTotalString, "}"))
		{
			int32_t begin = Utils_StringIndexOf('{', text->mTotalString, EE_FILENAME_MAX, false);
			int32_t end = Utils_StringIndexOf('}', text->mTotalString, EE_FILENAME_MAX, false);

			int32_t off = begin + 1;
			MString* total = NULL;
			MString_AssignSubString(&total, text->mTotalString, off, end - off);

			if (Utils_StringStartsWith(MString_Text(total), "&"))
			{
				int32_t totalLen = MString_Length(total);
				MString* sound = NULL;
				MString_AssignSubString(&sound, MString_Text(total), 1, totalLen - 1);
				bool wasFound = false;
				for (int32_t i = 0; i < PLAY_SOUNDS_LEN; i += 1)
				{
					PlaySound* ps = &text->mPlaySounds[i];
					if (!ps->mIsLoaded)
					{
						ps->mPoint = begin;
						Utils_strlcpy(ps->mSound, MString_Text(sound), EE_FILENAME_MAX);
						ps->mIsLoaded = true;
						wasFound = true;
						break;
					}
				}
				if (!wasFound)
				{
					Logger_LogInformation("Unable to find index for movie play sound!");
				}
				MString_Dispose(&sound);
			}
			else if (Utils_StringStartsWith(MString_Text(total), "^"))
			{
				int32_t totalLen = MString_Length(total);
				MString* number = NULL;
				MString_AssignSubString(&number, MString_Text(total), 1, totalLen - 1);
				int32_t pauseTime = Utils_ParseInt(MString_Text(number));
				bool wasFound = false;
				for (int32_t i = 0; i < TEXT_PAUSES_LEN; i += 1)
				{
					Pause* p = &text->mPauses[i];
					if (!p->mIsLoaded)
					{
						p->mPoint = begin;
						p->mTimer = Timer_Create(pauseTime);
						p->mIsLoaded = true;
						wasFound = true;
						break;
					}
				}
				if (!wasFound)
				{
					Logger_LogInformation("Unable to find index for movie text pause!");
				}
				MString_Dispose(&number);
			}

			MString* strFirstNew = NULL;
			MString_AssignSubString(&strFirstNew, text->mTotalString,0, begin);

			MString* strSecondNew = NULL;
			MString_AssignSubString(&strSecondNew, text->mTotalString, end + 1, (int32_t)(Utils_strlen(text->mTotalString) - (end + 1)));

			MString* finalResult = NULL;
			MString_AssignMString(&finalResult, strFirstNew);
			MString_AddAssignMString(&finalResult, strSecondNew);
			Utils_strlcpy(text->mTotalString, MString_Text(finalResult), EE_FILENAME_MAX);

			MString_Dispose(&total);
			MString_Dispose(&strFirstNew);
			MString_Dispose(&strSecondNew);
			MString_Dispose(&finalResult);
		}
		else
		{
			isReady = true;
		}
	}

	if (isMappedText && !Utils_IsCurrentLanguageEnglish())
	{
		/*text->mTotalString = Utils_GetStringWithNewLines(mTotalString, font, OeCvars_GetAsInt(OeCvars_ENGINE_CUTSCENE_INTL_TEXT_WORD_WRAP_WIDTH));
		if (GetSpeedToIncrease() > 0)
		{
			text->mIsSpeedIncreased = true;
		}*/ //UNUSED
	}

	Utils_strlcpy(text->mCurrentString, EE_STR_EMPTY, EE_FILENAME_MAX);
	text->mTimerWait = Timer_Create(wait);
	text->mTimerAddText = Timer_Create(speed);
	text->mRate = rate;
	text->mPosition = position;

	if (Utils_StringEqualTo(color, "WHITE"))
	{
		text->mColor = COLOR_WHITE;
	}
	else if (Utils_StringEqualTo(color, "MUTE_YELLOW"))
	{
		text->mColor = COLOR_MUTE_YELLOW;
	}
	else if (Utils_StringEqualTo(color, "MUTE_RED"))
	{
		text->mColor = COLOR_MUTE_RED;
	}
	else if (Utils_StringEqualTo(color, "MUTE_LGRAY"))
	{
		text->mColor = COLOR_MUTE_LGRAY;
	}
	else if (Utils_StringEqualTo(color, "MUTE_DGRAY"))
	{
		text->mColor = COLOR_MUTE_DGRAY;
	}

	MovieOperationText_TextCommandsCheck(text, 0);
}

void MovieOperationText_SpeedUp(MovieOperationText* text)
{
	if (!text->mStringShown)
	{
		Utils_strlcpy(text->mCurrentString, text->mTotalString, EE_FILENAME_MAX);
		text->mStringShown = true;
	}
	else
	{
		text->mIsComplete = true;
	}
}
void MovieOperationText_Update(MovieOperationText* text)
{
	text->mSpeedIncreaseCounter += 1;

	int32_t loops = 1;
	if (text->mIsSpeedIncreased)
	{
		if ((text->mSpeedIncreaseCounter % MovieOperationText_GetSpeedToIncrease(text)) == 0)
		{
			loops = 2;
		}
	}

	for (int32_t i = 0; i < loops; i += 1)
	{
		if (text->mStringShown)
		{
			MovieOperationText_EndUpdate(text);
		}
		else
		{
			MovieOperationText_NormalUpdate(text);
		}
	}
}
void MovieOperationText_DrawHud(MovieOperationText* text, SpriteBatch* spriteBatch)
{
	if (text->mIsComplete)
	{
		return;
	}

	if (!Utils_StringEqualTo(text->mFont, EE_STR_EMPTY) && !Utils_StringEqualTo(text->mCurrentString, EE_STR_EMPTY))
	{
		if (text->mIsCentered)
		{
			SpriteBatch_DrawString2(spriteBatch, text->mFont, text->mCurrentString, text->mColor, 100, text->mPosition, ALIGN_CENTER, ALIGN_CENTER);
		}
		else
		{
			SpriteBatch_DrawString(spriteBatch, text->mFont, text->mCurrentString, text->mColor, 100, text->mPosition);
		}
	}
}
void MovieOperationText_SetTextTypeSound(MovieOperationText* text, const char* sound)
{
	Utils_strlcpy(mTextTypeSound, sound, EE_FILENAME_MAX);
}
