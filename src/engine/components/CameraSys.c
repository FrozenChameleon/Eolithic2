/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "CameraSys.h"

#include "../math/Math.h"
#include "../utils/Cvars.h"

void CameraSys_UpdateLastRenderPositionRoutine(Camera* data)
{
	Camera_UpdateLastRenderPosition(data);
}
void CameraSys_InitRoutine(Entity owner, Camera* data)
{
	CameraSys_InitCamera(data);
}
void CameraSys_InitCamera(Camera* data)
{
	data->mHingeGateLeft = -1;
	data->mHingeGateRight = -1;
	data->mHingeGateTop = -1;
	data->mHingeGateBottom = -1;

	data->mWorldZoom = 1;
	data->mCameraSpeed = 1;
	data->mCurrentShakeMul = 1;

	data->mHingeSpeedLimit.X = 3.0f;
	data->mHingeSpeedLimit.Y = 3.0f;

	int32_t width = Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_WIDTH);
	int32_t height = Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_HEIGHT);
	Camera_Resize(data, width, height);
}
void CameraSys_UpdateRoutine(Entity owner, Camera* data)
{
	CameraSys_UpdateCamera(data);
}
void CameraSys_UpdateCamera(Camera* data)
{
	//Update shake no matter what
	data->mCurrentShake = data->mNewShake;
	data->mCurrentShakeMul = data->mNewShakeMul;
	data->mNewShake = Point_Zero;
	data->mNewShakeMul = 1;
	//

	if (data->mIsUpdateDisabled)
	{
		return;
	}

	if (data->mIsHingedCamera)
	{
		int32_t halfWidth = Camera_GetWidth(data) / 2;
		int32_t halfHeight = Camera_GetHeight(data) / 2;
		if (data->mHingeGateLeft != -1)
		{
			if ((data->mTargetPosition.X - halfWidth) < data->mHingeGateLeft)
			{
				data->mTargetPosition.X = (float)(data->mHingeGateLeft + halfWidth);
			}
		}
		if (data->mHingeGateRight != -1)
		{
			if ((data->mTargetPosition.X + halfWidth) > data->mHingeGateRight)
			{
				data->mTargetPosition.X = (float)(data->mHingeGateRight - halfWidth);
			}
		}
		if (data->mHingeGateTop != -1)
		{
			if ((data->mTargetPosition.Y - halfHeight) < data->mHingeGateTop)
			{
				data->mTargetPosition.Y = (float)(data->mHingeGateTop + halfHeight);
			}
		}
		if (data->mHingeGateBottom != -1)
		{
			if ((data->mTargetPosition.Y + halfHeight) > data->mHingeGateBottom)
			{
				data->mTargetPosition.Y = (float)(data->mHingeGateBottom - halfHeight);
			}
		}
	}

	float differenceX = ((data->mTargetPosition.X - data->mCurrentPosition.X) * data->mCameraSpeed);
	if (data->mChangedCameraHingeX)
	{
		if (Math_fabsf(differenceX) > data->mHingeSpeedLimit.X)
		{
			differenceX = data->mHingeSpeedLimit.X * Math_SignumFloat(differenceX);
		}
		if (data->mCurrentPosition.X + differenceX == data->mTargetPosition.X)
		{
			data->mChangedCameraHingeX = false;
		}
	}

	float differenceY = ((data->mTargetPosition.Y - data->mCurrentPosition.Y) * data->mCameraSpeed);
	if (data->mChangedCameraHingeY)
	{
		if (Math_fabsf(differenceY) > data->mHingeSpeedLimit.Y)
		{
			differenceY = data->mHingeSpeedLimit.Y * Math_SignumFloat(differenceY);
		}
		if (data->mCurrentPosition.Y + differenceY == data->mTargetPosition.Y)
		{
			data->mChangedCameraHingeY = false;
		}
	}

	if (differenceX != 0)
	{
		if (!data->mIsHingedCamera || Math_SignumFloat(differenceX) == Math_SignumInt(data->mHingeDirection.X) || data->mHingeDualDirectionX)
		{
			data->mCurrentPosition.X += differenceX;
		}
	}
	if (differenceY != 0)
	{
		if (!data->mIsHingedCamera || Math_SignumFloat(differenceY) == Math_SignumInt(data->mHingeDirection.Y) || data->mHingeDualDirectionY)
		{
			data->mCurrentPosition.Y += differenceY;
		}
	}

	data->mLastDifference.X = differenceX;
	data->mLastDifference.Y = differenceY;
}

System* CameraSys_CreateSystem(void)
{
	SystemSimple* ss = SystemSimple_Create(C_Camera);
	ss->_mUpdateLastRenderPositionRoutine = (SystemSimple_UpdateLastRenderPositionFunc)CameraSys_UpdateLastRenderPositionRoutine;
	ss->_mInitRoutine = (SystemSimple_InitFunc)CameraSys_InitRoutine;
	ss->_mUpdateRoutine = (SystemSimple_UpdateFunc)CameraSys_UpdateRoutine;
	return SystemSimple_CreateSystem(ss);
}
