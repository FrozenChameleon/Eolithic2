/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "SysResetBounceData.h"

#include "../utils/Utils.h"

void SysResetBounceData_UpdateRoutine(Entity owner, BounceData* data)
{
	if (!data->mWasUsed)
	{
		Utils_memset(data, 0, sizeof(BounceData));
	}
	data->mWasUsed = false;
}

System* SysResetBounceData_CreateSystem(void)
{
	SystemSimple* ss = SystemSimple_Create(C_BounceData);
	ss->_mUpdateRoutine = (SystemSimple_UpdateFunc)SysResetBounceData_UpdateRoutine;
	return SystemSimple_CreateSystem(ss);
}
