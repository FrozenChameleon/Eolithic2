/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "MirrorEffect.h"

#include "../core/Func.h"

void MirrorEffect_SetOn(MirrorEffect* me)
{
	if (!me->mIsOn)
	{
		Do_ResetFrameTimer2(&me->mTimer);
		me->mIsOn = true;
	}
}
void MirrorEffect_SetOff(MirrorEffect* me)
{
	if (me->mIsOn)
	{
		Do_ResetFrameTimer2(&me->mTimer);
		me->mIsOn = false;
	}
}
