/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "MovieGlobals.h"
#include "../math/Vector2.h"
#include "../utils/Timer.h"

typedef struct SpriteBatch SpriteBatch;

typedef struct MovieOperationFadeText
{
	MOVIE_OPERATION_HEADER
	Vector2 mPosition;
	int32_t mRamp;
	bool mIsRampingDown;
	bool mIsRamping;
	char mStringToShow[EE_FILENAME_MAX];
	char mFont[EE_FILENAME_MAX];
	Timer mTimerHold;
	Timer mTimerRamp;
	bool mIsCentered;
} MovieOperationFadeText;

void MovieOperationFadeText_Init(MovieOperationFadeText* fade, const char* str, const char* font, Vector2 position, int32_t rampSpeed, int32_t holdTime, bool isTextCentered);
void MovieOperationFadeText_Update(MovieOperationFadeText* fade);
void MovieOperationFadeText_DrawHud(MovieOperationFadeText* fade, SpriteBatch* spriteBatch);
void MovieOperationFadeText_SpeedUp(MovieOperationFadeText* fade);
