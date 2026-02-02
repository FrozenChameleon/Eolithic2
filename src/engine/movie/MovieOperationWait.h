/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "MovieGlobals.h"
#include "../utils/Timer.h"

typedef struct MovieOperationWait
{
	MOVIE_OPERATION_HEADER
	Timer mTimerWait;
} MovieOperationWait;

void MovieOperationWait_Init(MovieOperationWait* wait, int32_t timeLimit);

void MovieOperationWait_SpeedUp(MovieOperationWait* wait);
void MovieOperationWait_Update(MovieOperationWait* wait);
bool MovieOperationWait_ForcesWait(MovieOperationWait* wait);