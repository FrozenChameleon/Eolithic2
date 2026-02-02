/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "SysResetFloatyMovementData.h"

#include "../utils/Utils.h"

void SysResetFloatyMovementData_UpdateRoutine(Entity owner, FloatyMovementData* data)
{
	if (!data->mWasUsed)
	{
		Utils_memset(data, 0, sizeof(FloatyMovementData));
	}
	data->mWasUsed = false;
}

System* SysResetFloatyMovementData_CreateSystem(void)
{
	SystemSimple* ss = SystemSimple_Create(C_FloatyMovementData);
	ss->_mUpdateRoutine = (SystemSimple_UpdateFunc)SysResetFloatyMovementData_UpdateRoutine;
	return SystemSimple_CreateSystem(ss);
}