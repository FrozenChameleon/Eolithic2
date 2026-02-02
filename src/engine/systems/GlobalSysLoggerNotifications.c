/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GlobalSysLoggerNotifications.h"

#include "../globals/Globals.h"
#include "../gamestate/GameStateManager.h"
#include "../utils/Logger.h"

static void DrawHud(void* givenData, SpriteBatch* spriteBatch)
{
	Logger_DrawHud(spriteBatch);
}
static void Update(void* givenData)
{
	Logger_Update();
};

System* GlobalSysLoggerNotifications_CreateSystem(void)
{
	System* sys = System_Create();
	sys->_mDrawHud = DrawHud;
	sys->_mUpdate = Update;
	return sys;
}
