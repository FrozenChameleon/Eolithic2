/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "LevelCameraDataInstance.h"
#include "../utils/Utils.h"

void LevelCameraDataInstance_Reset(LevelCameraDataInstance* lcdi)
{
	lcdi->mIsComplete = false;
	lcdi->mIsCompletePermanently = false;
	Utils_memset(&lcdi->mData, 0, sizeof(LevelCameraData));
}
void LevelCameraDataInstance_ResetFullyNoMatterWhat(LevelCameraDataInstance* lcdi)
{
	lcdi->mIsCompletePermanently = false;
	lcdi->mIsComplete = false;
}
void LevelCameraDataInstance_ResetIfNotCompletePermanently(LevelCameraDataInstance* lcdi)
{
	if (!lcdi->mIsCompletePermanently)
	{
		lcdi->mIsComplete = false;
	}
}
void LevelCameraDataInstance_SetCompletePermanently(LevelCameraDataInstance* lcdi)
{
	if (!lcdi->mData.mIsPersistent)
	{
		lcdi->mIsCompletePermanently = true;
		lcdi->mIsComplete = true;
	}
}
void LevelCameraDataInstance_SetComplete(LevelCameraDataInstance* lcdi)
{
	if (!lcdi->mData.mIsPersistent)
	{
		lcdi->mIsComplete = true;
	}
}
bool LevelCameraDataInstance_IsComplete(LevelCameraDataInstance* lcdi)
{
	return lcdi->mIsComplete;
}
void LevelCameraDataInstance_ResetState(LevelCameraDataInstance* lcdi)
{
	lcdi->mIsComplete = false;
}