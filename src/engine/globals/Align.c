/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Align.h"

int32_t Align_ToAlign(bool value)
{
	if (value == true)
	{
		return ALIGN_CENTER;
	}
	else
	{
		return ALIGN_LEFT;
	}
}
