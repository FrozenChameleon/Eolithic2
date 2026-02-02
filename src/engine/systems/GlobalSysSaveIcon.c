/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GlobalSysSaveIcon.h"

#include "../render/Animation.h"
#include "../render/SpriteBatch.h"
#include "../utils/Utils.h"
#include "../utils/Cvars.h"
#include "../render/Color.h"
#include "../render/Sheet.h"

static Animation _mSaveIcon;

static bool HasInit(void)
{
	return (_mSaveIcon.mSheetsLen != 0);
}
static void InitAnimation(void)
{
	const char* spriteSheet = Cvars_Get(CVARS_ENGINE_SAVE_ICON);
	int32_t flipTimer = Cvars_GetAsInt(CVARS_ENGINE_SAVE_ICON_FLIP_SPEED);
	Animation_Init(&_mSaveIcon, spriteSheet, flipTimer);
}

static void Update(void* givenData)
{
	if (!HasInit())
	{
		InitAnimation();
		return;
	}

	if (Utils_CheckSave(true))
	{
		Animation_Update(&_mSaveIcon);
	}
	else
	{
		Animation_ResetAnimation(&_mSaveIcon);
	}
}
static void DrawHud(void* givenData, SpriteBatch* spriteBatch)
{
	if (!HasInit())
	{
		InitAnimation();
		return;
	}

	if (Utils_CheckSave(false))
	{
		int32_t scale = Cvars_GetAsInt(CVARS_ENGINE_SAVE_ICON_SCALE);
		int32_t offsetX = Cvars_GetAsInt(CVARS_ENGINE_SAVE_ICON_OFFSET_X);
		int32_t offsetY = Cvars_GetAsInt(CVARS_ENGINE_SAVE_ICON_OFFSET_Y);
		Sheet* sheet = Animation_GetCurrentSheet(&_mSaveIcon);
		Rectangle rect = sheet->mRectangle;
		rect.Width *= scale;
		rect.Height *= scale;
		Sheet_Draw5(sheet, spriteBatch, COLOR_WHITE, 100, false, false,
			NULL, Vector2_Create((float)(Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_RENDER_WIDTH) - rect.Width + offsetX), (float)offsetY),
			Vector2_Create2((float)scale), 0, false, false, Vector2_Zero);
	}
}

System* GlobalSysSaveIcon_CreateSystem(void)
{
	System* sys = System_Create();
	sys->_mUpdate = Update;
	sys->_mDrawHud = DrawHud;
	return sys;
}
