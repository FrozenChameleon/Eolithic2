/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

/* Derived from code by Ethan Lee (Copyright 2009-2024).
 * Released under the Microsoft Public License.
 * See fna.LICENSE for details.

 * Derived from code by the Mono.Xna Team (Copyright 2006).
 * Released under the MIT License.
 * See monoxna.LICENSE for details.
 */

#include "MathHelper.h"

#include "stdbool.h"

static bool _mHasCalculatedMachineEpsilonFloat;
static float _mMachineEpsilonFloat;

static float GetMachineEpsilonFloat(void)
{
	float machineEpsilon = 1.0f;
	float comparison;

	// Keep halving the working value of machineEpsilon until we get a number that
	// when added to 1.0f will still evaluate as equal to 1.0f.
	do
	{
		machineEpsilon *= 0.5f;
		comparison = (1.0f + machineEpsilon);
	} while (comparison > 1.0f);

	return machineEpsilon;
}

float MathHelper_ToDegrees(float radians)
{
	return (float)(radians * 57.295779513082320876798154814105);
}
float MathHelper_ToRadians(float degrees)
{
	return (float)(degrees * 0.017453292519943295769236907684886);
}
float MathHelper_GetMachineEpsilonFloat(void)
{
	if (!_mHasCalculatedMachineEpsilonFloat)
	{
		_mMachineEpsilonFloat = GetMachineEpsilonFloat();
		_mHasCalculatedMachineEpsilonFloat = true;
	}

	return _mMachineEpsilonFloat;
}
float MathHelper_Clamp(float value, float min, float max)
{
	// First we check to see if we're greater than the max.
	value = (value > max) ? max : value;

	// Then we check to see if we're less than the min.
	value = (value < min) ? min : value;

	// There's no check to see if min > max.
	return value;
}
float MathHelper_Lerp(float value1, float value2, float amount)
{
	return value1 + (value2 - value1) * amount;
}
