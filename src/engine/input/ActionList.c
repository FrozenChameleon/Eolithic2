/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "ActionList.h"

#include "../utils/IStringArray.h"
#include "../utils/Logger.h"

static IStringArray* _mActionNames;
static bool _mHasInit;

static void Init(void)
{
	if (_mHasInit)
	{
		return;
	}

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

	_mHasInit = true;
}

const char* ActionList_GetAction(int32_t index)
{
	Init();

	if (index > (ACTIONLIST_LENGTH - 1))
	{
		Logger_LogWarning("Trying to get action from action list that does not exist!");
		return ACTIONLIST_GAME_A;
	}

	return IStringArray_Get(_mActionNames, index);
}
IStringArray* ActionList_GetArray(void)
{
	Init();

	return _mActionNames;
}
