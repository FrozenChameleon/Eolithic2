/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"

typedef struct Stopwatch
{
	uint64_t mBegin;
} Stopwatch;

uint64_t Stopwatch_GetTicks(void);
double Stopwatch_GetElapsedSeconds(uint64_t startingTicks, uint64_t endingTicks);
uint64_t Stopwatch_Start(Stopwatch* data);
double Stopwatch_GetElapsedSecondsFromData(const Stopwatch* data);
double Stopwatch_GetElapsedSecondsAgainstCurrentTicks(uint64_t startingTicks);

