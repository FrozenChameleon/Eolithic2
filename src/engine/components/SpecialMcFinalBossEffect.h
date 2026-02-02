/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../render/Animation.h"
#include "../math/Point.h"
#include "../math/Vector2.h"

typedef struct SpecialMcFinalBossEffect
{
	int32_t mLineCounter;
	Point mRandomOffset;
	Vector2 mPosition;
	Timer mTimerBleed;
	Timer mTimerLine;
	Animation mAnimation;
	char mAnimationName[EE_FILENAME_MAX];
	Vector2 mBloodOffset;
	bool mDisableShaking;
	int32_t mBloodCutoff;
	int32_t mForcedDirection;
} SpecialMcFinalBossEffect;

