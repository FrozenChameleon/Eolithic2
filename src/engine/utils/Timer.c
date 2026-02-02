/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Timer.h"

#include "Utils.h"

const Timer Timer_Zero = { 0, 0 };

Timer Timer_Create(int32_t limit)
{
	Timer timer;
	timer.mCurrent = 0;
	timer.mLimit = limit;
	return timer;
}
void Timer_Init(Timer* timer, int32_t limit)
{
	Utils_memset(timer, 0, sizeof(Timer));
	timer->mLimit = limit;
}
float Timer_GetPercentage(const Timer* timer)
{
	float temp1 = (float)timer->mCurrent;
	float temp2 = (float)timer->mLimit;
	return temp1 / temp2;
}
float Timer_GetInversePercentage(const Timer* timer)
{
	return 1.0f - Timer_GetPercentage(timer);
}
int32_t Timer_GetRemainingTime(const Timer* timer)
{
	return timer->mLimit - timer->mCurrent;
}
bool Timer_UpdateWithTuningLimit(Timer* timer, Entity entity, const char* tuning)
{
	return false;
	//WILLNOTDO MCDX 2023return Update(timer, OeFunc.Get_TuningAsInt(entity, tuning));
}
bool Timer_UpdateWithLimit(Timer* timer, int32_t limit)
{
	timer->mLimit = limit;
	return Timer_Update(timer);
}
bool Timer_Update(Timer* timer)
{
	timer->mCurrent += 1;
	if (timer->mCurrent >= timer->mLimit)
	{
		Timer_Reset(timer);
		return true;
	}
	return false;
}
void Timer_Reset(Timer* timer)
{
	timer->mCurrent = 0;
}
bool Timer_IsAtPercentage(const Timer* timer, float percent)
{
	return false;
	//int32_t goal = OeMath::Round(timer->mLimit * percent); //UNUSED
	//if (timer->mCurrent == goal)
	//{
	//	return true;
	//}
	//return false;
}
void Timer_SetToTwoFourthsOfLimit(Timer* timer)
{
	Timer_SetAsPercentOfLimit(timer, 0.5f);
}
void Timer_SetToThreeFourthsOfLimit(Timer* timer)
{
	Timer_SetAsPercentOfLimit(timer, 0.75f);
}
void Timer_SetToOneFourthsOfLimit(Timer* timer)
{
	Timer_SetAsPercentOfLimit(timer, 0.25f);
}
void Timer_SetToLimit(Timer* timer)
{
	timer->mCurrent = timer->mLimit;
}
void Timer_SetAsPercentOfLimit(Timer* timer, float percent)
{
	timer->mCurrent = (int32_t)(timer->mLimit * percent);
}
void Timer_InverseCurrent(Timer* timer)
{
	timer->mCurrent = timer->mLimit - timer->mCurrent;
}
