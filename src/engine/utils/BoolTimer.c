#include "BoolTimer.h"

void BoolTimer_ResetAndTurnOn(BoolTimer* data)
{
	BoolTimer_Reset(data);
	data->mIsOn = true;
}
void BoolTimer_Reset(BoolTimer* data)
{
	Timer_Reset(&data->mTimer);
	data->mIsOn = false;
}
bool BoolTimer_Update(BoolTimer* data)
{
	if (!data->mIsOn)
	{
		return false;
	}

	if (Timer_Update(&data->mTimer))
	{
		data->mIsOn = false;
		return true;
	}

	return false;
}
