/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "Macros.h"

typedef struct Timer
{
	int32_t mCurrent;
	int32_t mLimit;
} Timer;

extern const Timer Timer_Zero;

Timer Timer_Create(int32_t limit);
void Timer_Init(Timer* timer, int32_t limit);
float Timer_GetPercentage(const Timer* timer);
float Timer_GetInversePercentage(const Timer* timer);
int32_t Timer_GetRemainingTime(const Timer* timer);
bool Timer_UpdateWithTuningLimit(Timer* timer, Entity entity, const char* tuning);
bool Timer_UpdateWithLimit(Timer* timer, int32_t limit);
bool Timer_Update(Timer* timer);
void Timer_Reset(Timer* timer);
bool Timer_IsAtPercentage(const Timer* timer, float percent);
void Timer_SetToTwoFourthsOfLimit(Timer* timer);
void Timer_SetToThreeFourthsOfLimit(Timer* timer);
void Timer_SetToOneFourthsOfLimit(Timer* timer);
void Timer_SetToLimit(Timer* timer);
void Timer_SetAsPercentOfLimit(Timer* timer, float percent);
void Timer_InverseCurrent(Timer* timer);

