/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Vector2.h"
#include "../utils/Timer.h"

typedef struct Sheet Sheet;

typedef struct SpecialMcDeathEffect
{
	int32_t mDisplayType;
	int32_t mCounter;
	float mSectionWidth;
	float mSectionHeight;
	float mSpeed;
	Vector2 mPositionOne;
	Vector2 mLastPositionOne;
	Vector2 mPositionTwo;
	Vector2 mLastPositionTwo;
	Vector2 mInitialPosition;
	Timer mTimer;
	Sheet* mSheetReference;
	Sheet* mSheetUpperLeft;
	Sheet* mSheetUpperRight;
	Sheet* mSheetLowerRight;
	Sheet* mSheetLowerLeft;
} SpecialMcDeathEffect;

