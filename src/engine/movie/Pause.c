/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Pause.h"

void Pause_Update(Pause* p)
{
	if (Timer_Update(&p->mTimer))
	{
		p->mIsComplete = true;
	}
}
bool Pause_IsAtPoint(const Pause* p, int32_t value)
{
	if (value == p->mPoint)
	{
		return true;
	}
	return false;
}
