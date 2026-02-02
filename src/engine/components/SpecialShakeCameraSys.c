/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "SpecialShakeCameraSys.h"

#include "../core/Func.h"

void SpecialShakeCameraSys_UpdateRoutine(Entity owner, ShakeCamera* data)
{
	Do_ShakeCamera(data->mMinX, data->mMaxX, data->mMinY, data->mMaxY);
	if (Timer_Update(&data->mTimer))
	{
		Do_SetComplete(owner);
	}
}
void SpecialShakeCameraSys_Setup(ShakeCamera* data, int32_t timerLimit, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY)
{
	data->mTimer.mLimit = timerLimit;
	data->mMinX = minX;
	data->mMaxX = maxX;
	data->mMinY = minY;
	data->mMaxY = maxY;
}
void SpecialShakeCameraSys_Create(int32_t timerLimit, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY)
{
	Entity entity = Do_BuildNewEntityWithName("Shake Camera");
	Do_InitComponent(C_ShakeCamera, entity);
	SpecialShakeCameraSys_Setup((ShakeCamera*)Get_Component(C_ShakeCamera, entity), timerLimit, minX, maxX, minY, maxY);
}

System* SpecialShakeCameraSys_CreateSystem(void)
{
	SystemSimple* ss = SystemSimple_Create(C_ShakeCamera);
	ss->_mUpdateRoutine = (SystemSimple_UpdateFunc)SpecialShakeCameraSys_UpdateRoutine;
	return SystemSimple_CreateSystem(ss);
}
