/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "FontMapData.h"

#include "../utils/Utils.h"

void FontMapData_Init(FontMapData* fmd)
{
	Utils_memset(fmd, 0, sizeof(FontMapData));

	fmd->mScaleFactor = 1;
	fmd->mLanguages = IStringArray_Create();
}
