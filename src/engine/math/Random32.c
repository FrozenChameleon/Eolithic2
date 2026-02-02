/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Random32.h"

#define INITIAL_SHIFTS 32
static const uint32_t DEFAULT_SEED = 777777777;

void Random32_Shift(Random32* data)
{
	data->mState ^= data->mState << 2;
	data->mState ^= data->mState >> 7;
	data->mState ^= data->mState << 7;
}
void Random32_SetSeed(Random32* data, uint32_t seed)
{
	if (seed == 0)
	{
		data->mState = DEFAULT_SEED;
	}
	else
	{
		data->mState = seed;
	}

	for (int32_t i = 0; i < INITIAL_SHIFTS; i += 1)
	{
		Random32_Shift(data);
	}
}
int32_t Random32_NextDirection(Random32* data)
{
	return Random32_NextBoolean(data) ? 1 : -1;
}
bool Random32_NextBoolean(Random32* data)
{
	return Random32_NextInt(data, 2) == 0;
}
int32_t Random32_NextInt(Random32* data, int32_t givenMaxValue)
{
	return (int32_t)(givenMaxValue * Random32_NextDouble(data));
}
float Random32_NextFloat(Random32* data)
{
	return (float)Random32_NextDouble(data);
}
double Random32_NextDouble(Random32* data)
{
	Random32_Shift(data);
	return data->mState / 4294967295.0; //uint32_t max value.
}