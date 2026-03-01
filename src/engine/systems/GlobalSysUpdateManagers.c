/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "GlobalSysUpdateManagers.h"

#include "../leveldata/AnimTile.h"
#include "../leveldata/Prop.h"
#include "../resources/ResourceList.h"

static void Update(void* givenData)
{
	{
		ResourceMan* animTileMan = ResourceList_AnimTile();
		int64_t len = ResourceMan_Length(animTileMan);
		for (int32_t i = 0; i < len; i += 1)
		{
			AnimTile_UpdateResource((AnimTile*)ResourceMan_GetResourceDataByIndex(animTileMan, i));
		}
	}

	{
		ResourceMan* propMan = ResourceList_Prop();
		int64_t len = ResourceMan_Length(propMan);
		for (int32_t i = 0; i < len; i += 1)
		{
			Prop_UpdateResource((Prop*)ResourceMan_GetResourceDataByIndex(propMan, i));
		}
	}
}

System* GlobalSysUpdateManagers_CreateSystem(void)
{
	System* sys = System_Create();
	sys->_mUpdate = Update;
	return sys;
}
