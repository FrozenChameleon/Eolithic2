/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "HitFlashKitSys.h"

#include "../core/Func.h"
#include "../render/ShaderProgram.h"
#include "../components/DrawActor.h"
#include "../utils/Utils.h"

static ShaderProgram* _mShaderProgram;

void HitFlashKitSys_InitRoutine(Entity owner, HitFlashKit* data)
{
	if (_mShaderProgram == NULL)
	{
		_mShaderProgram = ShaderProgram_GetShaderWhiteHitFlash();
	}
}
void HitFlashKitSys_UpdateRoutine(Entity owner, HitFlashKit* data)
{
	if (data->mIsDisabled)
	{
		return;
	}

	if (!data->mShowHitFlash)
	{
		return;
	}

	bool wasSuccessful;
	DrawActor* drawActor = (DrawActor*)TryGet_Component(C_DrawActor, owner, &wasSuccessful);
	if (!wasSuccessful)
	{
		return;
	}

	if (Utils_IsStringEmpty(data->mState))
	{
		drawActor->mShaderProgram = _mShaderProgram;
	}
	else
	{
		Do_SetShader2(owner, data->mState, _mShaderProgram);
	}
	if (Timer_Update(&data->mHitFlashTimer))
	{
		data->mShowHitFlash = false;
		if (Utils_IsStringEmpty(data->mState))
		{
			drawActor->mShaderProgram = NULL;
		}
		else
		{
			Do_SetShader2(owner, data->mState, NULL);
		}
	}
}

System* HitFlashKitSys_CreateSystem(void)
{
	SystemSimple* ss = SystemSimple_Create(C_HitFlashKit);
	ss->_mInitRoutine = (SystemSimple_InitFunc)HitFlashKitSys_InitRoutine;
	ss->_mUpdateRoutine = (SystemSimple_UpdateFunc)HitFlashKitSys_UpdateRoutine;
	return SystemSimple_CreateSystem(ss);
}
