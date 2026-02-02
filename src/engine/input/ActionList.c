/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "ActionList.h"

#include "../utils/IStringArray.h"
#include "../utils/Logger.h"

const char* ACTIONLIST_GAME_A = "GAME_A";
const char* ACTIONLIST_GAME_B = "GAME_B";
const char* ACTIONLIST_GAME_X = "GAME_X";
const char* ACTIONLIST_GAME_Y = "GAME_Y";
const char* ACTIONLIST_GAME_LB = "GAME_LB";
const char* ACTIONLIST_GAME_LT = "GAME_LT";
const char* ACTIONLIST_GAME_LS = "GAME_LS";
const char* ACTIONLIST_GAME_RB = "GAME_RB";
const char* ACTIONLIST_GAME_RT = "GAME_RT";
const char* ACTIONLIST_GAME_RS = "GAME_RS";
const char* ACTIONLIST_GAME_START = "GAME_START";
const char* ACTIONLIST_GAME_SELECT = "GAME_SELECT";
const char* ACTIONLIST_GAME_LS_UP = "GAME_LS_UP";
const char* ACTIONLIST_GAME_LS_RIGHT = "GAME_LS_RIGHT";
const char* ACTIONLIST_GAME_LS_DOWN = "GAME_LS_DOWN";
const char* ACTIONLIST_GAME_LS_LEFT = "GAME_LS_LEFT";
const char* ACTIONLIST_GAME_RS_UP = "GAME_RS_UP";
const char* ACTIONLIST_GAME_RS_RIGHT = "GAME_RS_RIGHT";
const char* ACTIONLIST_GAME_RS_DOWN = "GAME_RS_DOWN";
const char* ACTIONLIST_GAME_RS_LEFT = "GAME_RS_LEFT";
const char* ACTIONLIST_GAME_DIGITAL_UP = "GAME_DIGITAL_UP";
const char* ACTIONLIST_GAME_DIGITAL_RIGHT = "GAME_DIGITAL_RIGHT";
const char* ACTIONLIST_GAME_DIGITAL_DOWN = "GAME_DIGITAL_DOWN";
const char* ACTIONLIST_GAME_DIGITAL_LEFT = "GAME_DIGITAL_LEFT";
const char* ACTIONLIST_GAME_MENU_SELECT = "GAME_MENU_SELECT";
const char* ACTIONLIST_GAME_MENU_BACK = "GAME_MENU_BACK";
const char* ACTIONLIST_GAME_MENU_UP = "GAME_MENU_UP";
const char* ACTIONLIST_GAME_MENU_RIGHT = "GAME_MENU_RIGHT";
const char* ACTIONLIST_GAME_MENU_DOWN = "GAME_MENU_DOWN";
const char* ACTIONLIST_GAME_MENU_LEFT = "GAME_MENU_LEFT";

const char* ActionList_GetAction(int32_t index)
{
	switch (index)
	{
	case 0:
		return ACTIONLIST_GAME_A;
	case 1:
		return ACTIONLIST_GAME_B;
	case 2:
		return ACTIONLIST_GAME_X;
	case 3:
		return ACTIONLIST_GAME_Y;
	case 4:
		return ACTIONLIST_GAME_LB;
	case 5:
		return ACTIONLIST_GAME_LT;
	case 6:
		return ACTIONLIST_GAME_LS;
	case 7:
		return ACTIONLIST_GAME_RB;
	case 8:
		return ACTIONLIST_GAME_RT;
	case 9:
		return ACTIONLIST_GAME_RS;
	case 10:
		return ACTIONLIST_GAME_START;
	case 11:
		return ACTIONLIST_GAME_SELECT;
	case 12:
		return ACTIONLIST_GAME_LS_UP;
	case 13:
		return ACTIONLIST_GAME_LS_RIGHT;
	case 14:
		return ACTIONLIST_GAME_LS_DOWN;
	case 15:
		return ACTIONLIST_GAME_LS_LEFT;
	case 16:
		return ACTIONLIST_GAME_RS_UP;
	case 17:
		return ACTIONLIST_GAME_RS_RIGHT;
	case 18:
		return ACTIONLIST_GAME_RS_DOWN;
	case 19:
		return ACTIONLIST_GAME_RS_LEFT;
	case 20:
		return ACTIONLIST_GAME_DIGITAL_UP;
	case 21:
		return ACTIONLIST_GAME_DIGITAL_RIGHT;
	case 22:
		return ACTIONLIST_GAME_DIGITAL_DOWN;
	case 23:
		return ACTIONLIST_GAME_DIGITAL_LEFT;
	case 24:
		return ACTIONLIST_GAME_MENU_SELECT;
	case 25:
		return ACTIONLIST_GAME_MENU_BACK;
	case 26:
		return ACTIONLIST_GAME_MENU_UP;
	case 27:
		return ACTIONLIST_GAME_MENU_RIGHT;
	case 28:
		return ACTIONLIST_GAME_MENU_DOWN;
	case 29:
		return ACTIONLIST_GAME_MENU_LEFT;
	}

	Logger_LogWarning("Trying to get action from action list that does not exist!");

	return ACTIONLIST_GAME_A;
}

static IStringArray* _mActionNames;

IStringArray* ActionList_GetArray(void)
{
	if (_mActionNames == NULL)
	{
		_mActionNames = IStringArray_Create();
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_A);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_B);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_X);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_Y);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_LB);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_LT);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_LS);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_RB);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_RT);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_RS);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_START);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_SELECT);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_LS_UP);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_LS_RIGHT);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_LS_DOWN);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_LS_LEFT);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_RS_UP);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_RS_RIGHT);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_RS_DOWN);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_RS_LEFT);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_DIGITAL_UP);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_DIGITAL_RIGHT);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_DIGITAL_DOWN);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_DIGITAL_LEFT);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_MENU_SELECT);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_MENU_BACK);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_MENU_UP);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_MENU_RIGHT);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_MENU_DOWN);
		IStringArray_Add(_mActionNames, ACTIONLIST_GAME_MENU_LEFT);
	}

	return _mActionNames;
}
