/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Stopwatch.h"

#include "SDL3/SDL.h"

#define DIVIDER_FOR_SECONDS 1000.0

uint64_t Stopwatch_GetTicks(void)
{
	return SDL_GetTicks();
}
double Stopwatch_GetElapsedSeconds(uint64_t startingTicks, uint64_t endingTicks)
{
	return ((endingTicks - startingTicks) / DIVIDER_FOR_SECONDS);
}
uint64_t Stopwatch_Start(Stopwatch* data)
{
	data->mBegin = Stopwatch_GetTicks();
	return data->mBegin;
}
double Stopwatch_GetElapsedSecondsFromData(const Stopwatch* data)
{
	return Stopwatch_GetElapsedSeconds(data->mBegin, Stopwatch_GetTicks());
}
double Stopwatch_GetElapsedSecondsAgainstCurrentTicks(uint64_t startingTicks)
{
	return Stopwatch_GetElapsedSeconds(startingTicks, Stopwatch_GetTicks());
}