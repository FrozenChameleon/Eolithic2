/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "MovieGlobals.h"
#include "MovieImage.h"
#include "../utils/Timer.h"
#include "../globals/Globals.h"

typedef struct MovieOperationShake
{
	MOVIE_OPERATION_HEADER
	int32_t mMinimum;
	int32_t mMaximum;
	bool mContinual;
	char mKey[EE_FILENAME_MAX];
	Timer mTimerWait;
	MovieImage* mImage;
} MovieOperationShake;

void MovieOperationShake_Init(MovieOperationShake* shake, const char* key, MovieImage* image, int32_t min, int32_t max, int32_t time);
void MovieOperationShake_SpeedUp(MovieOperationShake* shake);
void MovieOperationShake_Update(MovieOperationShake* shake);
bool MovieOperationShake_DoesNotBlock(MovieOperationShake* shake);
void MovieOperationShake_Broadcast(MovieOperationShake* shake, const char* key, const char* value);