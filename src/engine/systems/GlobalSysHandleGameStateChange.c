/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GlobalSysHandleGameStateChange.h"

#include "../gamestate/GameStateManager.h"

static void Update(void* givenData)
{
	GameStateManager_HandleGameStateChange();
}

System* GlobalSysHandleGameStateChange_CreateSystem(void)
{
	System* sys = System_Create();
	sys->_mUpdate = Update;
	return sys;
}
