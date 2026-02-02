/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GlobalSysHideMouse.h"

#include "../utils/Cvars.h"
#include "../core/Game.h"
#include "../input/Input.h"
#include "../render/Renderer.h"
#include "../core/Window.h"

static int32_t mCounter;

static void Update(void* givenData)
{
	if (!Cvars_GetAsBool(CVARS_USER_IS_AUTO_HIDE_MOUSE))
	{
		return;
	}

	if (Input_MouseHasChangedPosition() || !Window_IsFullscreen())
	{
		mCounter = 0;
		if (!Window_IsTheMouseVisible())
		{
			Window_SetTheMouseVisible(true);
		}
	}
	else
	{
		if (Window_IsTheMouseVisible())
		{
			mCounter += 1;
			if (mCounter > Cvars_GetAsInt(CVARS_ENGINE_AUTO_HIDE_MOUSE_TIME))
			{
				Window_SetTheMouseVisible(false);
			}
		}
	}
}

System* GlobalSysHideMouse_CreateSystem(void)
{
	System* sys = System_Create();
	sys->_mUpdate = Update;
	return sys;
}
