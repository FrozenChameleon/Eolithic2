/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GlobalSysUpdateManagers.h"

#include "../leveldata/AnimTile.h"
#include "../leveldata/Prop.h"
#include "../resources/ResourceManagerList.h"

static void Update(void* givenData)
{
	{
		ResourceManager* animTileMan = ResourceManagerList_AnimTile();
		int64_t len = ResourceManager_Length(animTileMan);
		for (int32_t i = 0; i < len; i += 1)
		{
			AnimTile_UpdateResource((AnimTile*)ResourceManager_GetResourceDataByIndex(animTileMan, i));
		}
	}

	{
		ResourceManager* propMan = ResourceManagerList_Prop();
		int64_t len = ResourceManager_Length(propMan);
		for (int32_t i = 0; i < len; i += 1)
		{
			Prop_UpdateResource((Prop*)ResourceManager_GetResourceDataByIndex(propMan, i));
		}
	}
}

System* GlobalSysUpdateManagers_CreateSystem(void)
{
	System* sys = System_Create();
	sys->_mUpdate = Update;
	return sys;
}
