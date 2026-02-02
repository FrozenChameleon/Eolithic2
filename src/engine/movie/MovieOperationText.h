/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "MovieGlobals.h"
#include "PlaySound.h"
#include "Pause.h"
#include "../render/Color.h"
#include "../utils/Timer.h"
#include "../math/Vector2.h"

#define TEXT_PAUSES_LEN 16
#define PLAY_SOUNDS_LEN 8

typedef struct SpriteBatch SpriteBatch;

extern char mTextTypeSound[EE_FILENAME_MAX];

typedef struct MovieOperationText
{
	MOVIE_OPERATION_HEADER
	Pause mPauses[TEXT_PAUSES_LEN];
	PlaySound mPlaySounds[PLAY_SOUNDS_LEN];
	Timer mTimerAddText;
	Timer mTimerWait;
	Vector2 mPosition;
	int32_t mIndex;
	int32_t mRate;
	char mTotalString[EE_FILENAME_MAX];
	char mCurrentString[EE_FILENAME_MAX];
	bool mStringShown;
	char mFont[EE_FILENAME_MAX];
	bool mIsPaused;
	Color mColor;
	bool mIsSpeedIncreased;
	int32_t mSpeedIncreaseCounter;
	bool mIsCentered;
} MovieOperationText;

int32_t MovieOperationText_GetSpeedToIncrease(MovieOperationText* text);
void MovieOperationText_EndUpdate(MovieOperationText* text);
void MovieOperationText_NormalUpdate(MovieOperationText* text);
void MovieOperationText_TextCommandsCheck(MovieOperationText* text, int32_t point);

void MovieOperationText_Init(MovieOperationText* text, bool isMappedText, const char* str, const char* font, Vector2 position, 
	int32_t speed, int32_t rate, int32_t wait, const char* color, bool isTextCentered);

void MovieOperationText_SpeedUp(MovieOperationText* text);
void MovieOperationText_Update(MovieOperationText* text);
void MovieOperationText_DrawHud(MovieOperationText* text, SpriteBatch* spriteBatch);
void MovieOperationText_SetTextTypeSound(MovieOperationText* text, const char* sound);
