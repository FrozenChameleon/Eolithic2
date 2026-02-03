#include "Buttons.h"

static bool _mHasInit;
static int32_t _mArray[BUTTONS_AMOUNT_OF_BUTTONS];

static void Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	_mArray[0] = BUTTONS_DPADUP;
	_mArray[1] = BUTTONS_DPADDOWN;
	_mArray[2] = BUTTONS_DPADLEFT;
	_mArray[3] = BUTTONS_DPADRIGHT;
	_mArray[4] = BUTTONS_START;
	_mArray[5] = BUTTONS_BACK;
	_mArray[6] = BUTTONS_LEFTSTICK;
	_mArray[7] = BUTTONS_RIGHTSTICK;
	_mArray[8] = BUTTONS_LEFTSHOULDER;
	_mArray[9] = BUTTONS_RIGHTSHOULDER;
	_mArray[10] = BUTTONS_A;
	_mArray[11] = BUTTONS_B;
	_mArray[12] = BUTTONS_X;
	_mArray[13] = BUTTONS_Y;

	_mHasInit = true;
}

int32_t Buttons_GetButton(int32_t index)
{
	Init();

	return _mArray[index];
}

const int32_t* Buttons_GetArray(void)
{
	Init();

	return _mArray;
}
int32_t Buttons_GetArrayLength(void)
{
	return BUTTONS_AMOUNT_OF_BUTTONS;
}