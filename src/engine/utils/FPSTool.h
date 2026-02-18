/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"

typedef struct FPSTool
{
	int32_t mCurrentFps;
	int32_t mFrames;
	double mTime;
} FPSTool;

bool FPSTool_Update(FPSTool* data, double deltaTime);