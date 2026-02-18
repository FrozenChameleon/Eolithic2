/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "FPSTool.h"

bool FPSTool_Update(FPSTool* data, double deltaTime)
{
	data->mTime += deltaTime;
	data->mFrames += 1;
	if (data->mTime >= 1)
	{
		data->mCurrentFps = data->mFrames;
		data->mFrames = 0;
		data->mTime -= 1;
		return true;
	}
	return false;
}
