/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "FreezeEntityTillOnScreenSys.h"

#include "Camera.h"
#include "FreezeEntityTillOnScreen.h"
#include "../gamestate/ComponentPack.h"
#include "../core/Func.h"
#include "../math/Rectangle.h"

static void Update(void* givenData)
{
	Camera* camera = Get_Camera();
	ComponentPack* pack = Get_ComponentPack(C_FreezeEntityTillOnScreen);
	PackIterator iter = PackIterator_Begin;
	while (ComponentPack_Next(pack, &iter))
	{
		FreezeEntityTillOnScreen* freezeData = (FreezeEntityTillOnScreen*)iter.mComponent;
		const Rectangle* freezeRect = &freezeData->mRectangle;
		if (Camera_IntersectsCameraRectMul(camera, freezeRect, CAMERA_EXTENDED_CAMERA))
		{
			if (freezeData->mFacesPlayer)
			{
				Do_FacePlayer(iter.mEntity, freezeData->mIsFacingFlipped);
			}
			if (Camera_IntersectsCameraRect(camera, freezeRect))
			{
				Do_SetBodyDisabled(iter.mEntity, false);
				Do_SetUpdateDisabled(iter.mEntity, false);
				ComponentPack_UnsetAtIndex(pack, iter.mIndex);
			}
		}
	}
}

System* FreezeEntityTillOnScreenSys_CreateSystem(void)
{
	System* sys = System_Create();
	sys->_mUpdate = Update;
	return sys;
}
