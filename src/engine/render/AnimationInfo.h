/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "stdbool.h"

typedef struct AnimationInfo
{
	bool mHasStart;
	char mSheetName[EE_FILENAME_MAX];
	char mBaseName[EE_FILENAME_MAX];
	int32_t mFrames;
	int32_t mNumberOfDigits;
} AnimationInfo;