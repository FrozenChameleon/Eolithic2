#pragma once

#include "stdint.h"
#include "stdbool.h"

typedef struct TimerDouble
{
    bool mIsGoingDown;
    int mFrameLimit;
    int mCurrentFrame;
} TimerDouble;

void TimerDouble_Setup(TimerDouble* timer, int time);
void TimerDouble_Reset(TimerDouble* timer);
float TimerDouble_SolveForSpeed(TimerDouble* timer, float x1, float x2, int time);
float TimerDouble_SolveForSpeed(TimerDouble* timer, float distance, int time);
void TimerDouble_SolveForTime(TimerDouble* timer, float x1, float x2, float speed);
void TimerDouble_SolveForTime(TimerDouble* timer, float distance, float speed);
bool TimerDouble_Update(TimerDouble* timer);
float TimerDouble_GetPercentage(TimerDouble* timer);
float TimerDouble_GetInversePercentage(TimerDouble* timer);