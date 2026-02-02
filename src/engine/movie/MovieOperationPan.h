/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "MovieGlobals.h"
#include "../math/Vector2.h"
#include "../utils/Timer.h"
#include "MovieImage.h"

typedef struct MovieOperationPan
{
	MOVIE_OPERATION_HEADER
	Vector2 mSpeed;
	Timer mTimerWait;
	MovieImage* mImage;
} MovieOperationPan;

void MovieOperationPan_Init(MovieOperationPan* pan, MovieImage* image, Vector2 speed, int32_t time);
void MovieOperationPan_SpeedUp(MovieOperationPan* pan);
void MovieOperationPan_Update(MovieOperationPan* pan);