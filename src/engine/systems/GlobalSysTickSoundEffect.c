/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GlobalSysTickSoundEffect.h"

#include "../audio/SoundEffect.h"

static void Update(void* givenData)
{
	SoundEffect_Tick();
}

System* GlobalSysTickSoundEffect_CreateSystem(void)
{
	System* sys = System_Create();
	sys->_mUpdate = Update;
	return sys;
}
