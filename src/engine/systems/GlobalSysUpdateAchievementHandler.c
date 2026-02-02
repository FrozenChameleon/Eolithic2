/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GlobalSysUpdateAchievementHandler.h"

#include "../achievement/AchievementHandler.h"

static void Update(void* givenData)
{
	AchievementHandler_Update();
}

System* GlobalSysUpdateAchievementHandler_CreateSystem(void)
{
	System* sys = System_Create();
	sys->_mUpdate = Update;
	return sys;
}
