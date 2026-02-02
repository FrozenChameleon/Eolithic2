/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Timer.h"

typedef struct HitFlashKit
{
	int32_t mState;
	bool mIsDisabled;
	bool mShowHitFlash;
	Timer mHitFlashTimer;
} HitFlashKit;

void HitFlashKit_Setup(HitFlashKit* hfk, int32_t time);
void HitFlashKit_ResetFlash(HitFlashKit* hfk);