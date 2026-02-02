/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GlobalSysUpdateInput.h"

#include "../input/Input.h"

static void Update(void* givenData)
{
	Input_Update(false);
}

System* GlobalSysUpdateInput_CreateSystem(void)
{
	System* sys = System_Create();
	sys->_mUpdate = Update;
	return sys;
}
