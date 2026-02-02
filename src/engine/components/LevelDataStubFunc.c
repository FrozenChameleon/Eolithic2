/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "LevelDataStubFunc.h"

#include "../leveldata/LevelData.h"
#include "../resources/ResourceManagerList.h"

LevelData* LevelDataStubFunc_GetLevelData(const LevelDataStub* data)
{
	Resource* resource = LevelDataStubFunc_GetLevelDataResource(data);
	if (resource != NULL)
	{
		return (LevelData*)resource->mData;
	}
	else
	{
		return NULL;
	}
}
Resource* LevelDataStubFunc_GetLevelDataResource(const LevelDataStub* data)
{
	return ResourceManager_GetResource(ResourceManagerList_LevelData(), data->mLevelName);
}
