/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "ButtonList.h"

#include "Buttons.h"

#define AMOUNT_OF_BUTTONS 14

static bool _mHasInit;
static int32_t _mArray[AMOUNT_OF_BUTTONS];

static void Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	_mArray[0] = BUTTONS_DPadUp;
	_mArray[1] = BUTTONS_DPadDown;
	_mArray[2] = BUTTONS_DPadLeft;
	_mArray[3] = BUTTONS_DPadRight;
	_mArray[4] = BUTTONS_Start;
	_mArray[5] = BUTTONS_Back;
	_mArray[6] = BUTTONS_LeftStick;
	_mArray[7] = BUTTONS_RightStick;
	_mArray[8] = BUTTONS_LeftShoulder;
	_mArray[9] = BUTTONS_RightShoulder;
	_mArray[10] = BUTTONS_A;
	_mArray[11] = BUTTONS_B;
	_mArray[12] = BUTTONS_X;
	_mArray[13] = BUTTONS_Y;

	_mHasInit = true;
}

int32_t ButtonList_GetButton(int32_t index)
{
	Init();

	return _mArray[index];
}

const int32_t* ButtonList_GetArray(void)
{
	Init();

	return _mArray;
}
int32_t ButtonList_GetArrayLength(void)
{
	return AMOUNT_OF_BUTTONS;
}