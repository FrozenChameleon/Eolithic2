/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "SysDrawProps.h"

#include "../core/Func.h"
#include "../leveldata/LevelData.h"

static void Draw(void* givenData, SpriteBatch* spriteBatch)
{
	LevelData_DrawProps(Get_LevelData(), spriteBatch, Get_Camera());
}

System* SysDrawProps_CreateSystem(void)
{
	System* sys = System_Create();
	sys->_mDraw = Draw;
	return sys;
}
