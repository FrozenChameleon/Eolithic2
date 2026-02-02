/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Timer.h"

typedef struct Pause
{
	bool mIsComplete;
	bool mIsLoaded;
	int32_t mPoint;
	Timer mTimer;
} Pause;

void Pause_Update(Pause* p);
bool Pause_IsAtPoint(const Pause* p, int32_t value);