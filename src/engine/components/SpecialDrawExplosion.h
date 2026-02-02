/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Vector2.h"
#include "../utils/Timer.h"

typedef struct Sheet Sheet;

typedef struct SpecialDrawExplosion
{
	int32_t mCounter;
	Vector2 mInitialPosition;
	float mSectionWidth;
	float mSectionHeight;
	float mOffsetsX[3];
	float mOffsetsY[3];
	float mWidths[3];
	bool mDraw;
	int32_t mSides[3]; //Was booleans...
	Sheet* mSheetReference;
	Timer mTimer;
} SpecialDrawExplosion;

