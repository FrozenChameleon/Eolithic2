#include "TimerDouble.h"

#include "Logger.h"
#include "../math/Math.h"

void TimerDouble_Setup(TimerDouble* timer, int time)
{
	if (time % 2 != 0)
	{
		Logger_Log(LOGGER_WARNING, "FrameTimerDouble time needs to be divideable by 2.");
	}
	timer->mIsGoingDown = false;
	timer->mFrameLimit = time / 2;
	timer->mCurrentFrame = 0;
}
void TimerDouble_Reset(TimerDouble* timer)
{
	timer->mIsGoingDown = false;
	timer->mCurrentFrame = 0;
}
float TimerDouble_SolveForSpeed(TimerDouble* timer, float x1, float x2, int time)
{
	float distance = Math_fabsf(x1 - x2);
	return TimerDouble_SolveForSpeed(timer, distance, time);
}
float TimerDouble_SolveForSpeed(TimerDouble* timer, float distance, int time)
{
	float tempSpeed = distance / time;
	TimerDouble_Setup(timer, time);
	return tempSpeed * 2;
}
void TimerDouble_SolveForTime(TimerDouble* timer, float x1, float x2, float speed)
{
	float distance = Math_fabsf(x1 - x2);
	TimerDouble_SolveForTime(timer, distance, speed);
}
void TimerDouble_SolveForTime(TimerDouble* timer, float distance, float speed)
{
	float floatTime = distance / (speed * .5f);
	int time = (int)floatTime;
	TimerDouble_Setup(timer, time);
}
bool TimerDouble_Update(TimerDouble* timer)
{
	if (timer->mIsGoingDown)
	{
		timer->mCurrentFrame -= 1;
	}
	else
	{
		timer->mCurrentFrame += 1;
	}

	if (timer->mCurrentFrame <= 0 && timer->mIsGoingDown)
	{
		timer->mIsGoingDown = false;
		return true;
	}
	else if (timer->mCurrentFrame >= timer->mFrameLimit)
	{
		timer->mIsGoingDown = true;
	}

	return false;
}
float TimerDouble_GetPercentage(TimerDouble* timer)
{
	float temp1 = timer->mCurrentFrame;
	float temp2 = timer->mFrameLimit;
	return temp1 / temp2;
}
float TimerDouble_GetInversePercentage(TimerDouble* timer)
{
	return 1.0f - TimerDouble_GetPercentage(timer);
}
