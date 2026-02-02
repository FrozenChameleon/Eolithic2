/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GlobalSysTickMusic.h"

#include "../audio/Music.h"

static void Update(void* givenData)
{
	Music_Tick();
}

System* GlobalSysTickMusic_CreateSystem(void)
{
	System* sys = System_Create();
	sys->_mUpdate = Update;
	return sys;
}
