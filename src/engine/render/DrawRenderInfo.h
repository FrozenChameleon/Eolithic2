/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../leveldata/ImageDataInstance.h"

typedef struct DrawRenderInfo
{
	int32_t mState;
	int32_t mPhase;
	ImageDataInstance mRender;
} DrawRenderInfo;

