/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "MovieOperationPan.h"

#include "../utils/Utils.h"

void MovieOperationPan_Init(MovieOperationPan* pan, MovieImage* image, Vector2 speed, int32_t time)
{
	Utils_memset(pan, 0, sizeof(MovieOperationPan));

	pan->mType = MOVIE_OPERATION_TYPE_PAN;
	pan->mIsComplete = false;
	pan->mSpeed = speed;
	pan->mTimerWait = Timer_Create(time);
	pan->mImage = image;
}

void MovieOperationPan_SpeedUp(MovieOperationPan* pan)
{
	do
	{
		MovieOperationPan_Update(pan);
	} while (!pan->mIsComplete);
}
void MovieOperationPan_Update(MovieOperationPan* pan)
{
	if (Timer_Update(&pan->mTimerWait))
	{
		pan->mIsComplete = true;
	}
	else
	{
		Vector2_AddAssign(&pan->mImage->mPosition, pan->mSpeed);
	}
}
