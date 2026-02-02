/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Timer.h"

typedef struct ShakeCamera
{
	int32_t mMinX;
	int32_t mMaxX;
	int32_t mMinY;
	int32_t mMaxY;
	Timer mTimer;
} ShakeCamera;