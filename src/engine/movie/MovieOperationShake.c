/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "MovieOperationShake.h"

#include "../utils/Utils.h"
#include "../math/Random32.h"
#include "MovieGlobals.h"

void MovieOperationShake_Init(MovieOperationShake* shake, const char* key, MovieImage* image, int32_t min, int32_t max, int32_t time)
{
	Utils_memset(shake, 0, sizeof(MovieOperationShake));

	shake->mType = MOVIE_OPERATION_TYPE_SHAKE;
	shake->mIsComplete = false;
	shake->mMinimum = min;
	shake->mMaximum = max;
	shake->mContinual = false;
	if (time == -1)
	{
		shake->mContinual = true;
	}
	Utils_strlcpy(shake->mKey, key, EE_FILENAME_MAX);
	shake->mTimerWait = Timer_Create(time);
	shake->mImage = image;
}

void MovieOperationShake_SpeedUp(MovieOperationShake* shake)
{
	if (!shake->mContinual)
	{
		do
		{
			MovieOperationShake_Update(shake);
		} while (!shake->mIsComplete);
	}
}
void MovieOperationShake_Update(MovieOperationShake* shake)
{
	if (Timer_Update(&shake->mTimerWait) && !shake->mContinual)
	{
		shake->mIsComplete = true;

		shake->mImage->mShake = Vector2_Zero;
	}
	else
	{
		Random32* sharedRandom = Globals_GetSharedRandom();
		shake->mImage->mShake.X = (float)(shake->mMinimum + Random32_NextInt(sharedRandom, shake->mMaximum - shake->mMinimum));
		shake->mImage->mShake.Y = (float)(shake->mMinimum + Random32_NextInt(sharedRandom, shake->mMaximum - shake->mMinimum));
	}
}
bool MovieOperationShake_DoesNotBlock(MovieOperationShake* shake)
{
	if (shake->mContinual)
	{
		return true;
	}

	return false;
}
void MovieOperationShake_Broadcast(MovieOperationShake* shake, const char* key, const char* value)
{
	if (Utils_StringEqualTo(key, "shake"))
	{
		if (Utils_StringEqualTo(value, shake->mKey))
		{
			shake->mIsComplete = true;
			shake->mImage->mShake = Vector2_Zero;
		}
	}
}
