/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GlobalSysDrawFPS.h"

#include "../utils/Cvars.h"
#include "../render/Color.h"
#include "../utils/Utils.h"
#include "../core/GameUpdater.h"
#include "../math/Rectangle.h"
#include "../render/DrawTool.h"
#include "../render/SpriteBatch.h"
#include "../utils/Logger.h"

#define TIME_TO_STOP_DRAWING 5

static int32_t _mPrintCounter;
static int32_t _mDoNotDrawThisCounter = TIME_TO_STOP_DRAWING;
static int32_t _mOffsetFromCorner;
static const char* _mFont = "game";

void GlobalSysDrawFPS_SetOffsetFromCorner(int32_t offset)
{
	_mOffsetFromCorner = offset;
}
void GlobalSysDrawFPS_SetFont(const char* font)
{
	_mFont = font;
}
void GlobalSysDrawFPS_DoNotDrawThisRightNow(void)
{
	_mDoNotDrawThisCounter = TIME_TO_STOP_DRAWING;
}
static void Update(void* givenData)
{
	_mPrintCounter += 1;
	if (_mPrintCounter > 60)
	{
		//Logger_LogInformation(GameUpdater_GetFpsString());
		_mPrintCounter = 0;
	}

	if (_mDoNotDrawThisCounter > 0)
	{
		_mDoNotDrawThisCounter -= 1;
	}
}
static void DrawHud(void* givenData, SpriteBatch* spriteBatch)
{
	if (_mDoNotDrawThisCounter > 0)
	{
		return;
	}

	if (!Cvars_GetAsBool(CVARS_USER_IS_SHOWING_FPS))
	{
		return;
	}

	const char* stringToDraw = GameUpdater_GetFpsString();
	Rectangle bounds = DrawTool_GetBounds(stringToDraw, _mFont);
	int32_t shadowOffset = 1;
	float x = (float)_mOffsetFromCorner;
	float y = (float)(Utils_GetInternalRenderHeight() - bounds.Height - shadowOffset - _mOffsetFromCorner);
	SpriteBatch_DrawString(spriteBatch, _mFont, stringToDraw, COLOR_BLACK, 200, Vector2_Create(x + shadowOffset, y + shadowOffset));
	SpriteBatch_DrawString(spriteBatch, _mFont, stringToDraw, COLOR_WHITE, 200, Vector2_Create(x, y));
}

System* GlobalSysDrawFPS_CreateSystem(void)
{
	System* sys = System_Create();
	sys->_mDrawHud = DrawHud;
	sys->_mUpdate = Update;
	return sys;
}
