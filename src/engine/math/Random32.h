/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"

typedef struct Random32
{
	uint32_t mState;
} Random32;

void Random32_Shift(Random32* data);
void Random32_SetSeed(Random32* data, uint32_t seed);
int32_t Random32_NextDirection(Random32* data);
bool Random32_NextBoolean(Random32* data);
int32_t Random32_NextInt(Random32* data, int32_t givenMaxValue);
float Random32_NextFloat(Random32* data);
double Random32_NextDouble(Random32* data);