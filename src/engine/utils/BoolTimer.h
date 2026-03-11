#pragma once

#include "Timer.h"

typedef struct BoolTimer
{
	Timer mTimer;
	bool mIsOn;
} BoolTimer;

void BoolTimer_ResetAndTurnOn(BoolTimer* data);
void BoolTimer_Reset(BoolTimer* data);
bool BoolTimer_Update(BoolTimer* data);
