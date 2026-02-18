/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Camera.h"

#include "../utils/Macros.h"
#include "../core/Func.h"
#include "../globals/Globals.h"
#include "../math/Random32.h"
#include "../math/Math.h"
#include "../utils/Cvars.h"
#include "../utils/Utils.h"
#include "CameraSys.h"

#define TILE_SIZE GLOBAL_DEF_TILE_SIZE

void Camera_UpdateLastRenderPosition(Camera* camera)
{
	camera->mLastRenderedPosition = camera->mCurrentPosition;
}
bool Camera_IsHorizontalTube(const Camera* camera)
{
	return Camera_GetWidth(camera) > Camera_GetHeight(camera);
}
bool Camera_IsCameraAtTargetPosition(const Camera* camera)
{
	if (Vector2_EqualTo(camera->mLastDifference, Vector2_Zero))
	{
		return true;
	}
	return false;
}
Vector2 Camera_GetInterpCameraAsVector2(const Camera* camera, double deltaTime)
{
	float x = Camera_GetRenderedInterpolatedX(camera, deltaTime);
	float y = Camera_GetRenderedInterpolatedY(camera, deltaTime);

	x += (camera->mCurrentShake.X * camera->mCurrentShakeMul);
	y += (camera->mCurrentShake.Y * camera->mCurrentShakeMul);

#ifdef EDITOR_MODE
	if (Cvars_GetAsBool(CVARS_EDITOR_STRICT_CAMERA))
	{
		x = (float)((int32_t)(x));
		y = (float)((int32_t)(y));
	}
#endif

	Vector2 returnVector = { x, y };
	return returnVector;
}
Matrix Camera_GetInterpCamera(const Camera* camera, float offsetX, float offsetY, double deltaTime, int32_t scale)
{
	Vector2 pos = Camera_GetInterpCameraAsVector2(camera, deltaTime);
	return Camera_GetTranslation(offsetX + pos.X, offsetY + pos.Y, 
		camera->mWorldRotation, (1.0f / camera->mWorldZoom) * scale, 
		(float)(camera->mWorldWidth * scale), (float)(camera->mWorldHeight * scale));
}
float Camera_GetRenderedInterpolatedX(const Camera* camera, double deltaTime)
{
	float x;
	if (camera->mLastRenderedPosition.X == camera->mCurrentPosition.X)
	{
		x = camera->mLastRenderedPosition.X;
	}
	else
	{
		x = (float)Utils_GetInterpolated(deltaTime, camera->mCurrentPosition.X, camera->mLastRenderedPosition.X);
	}
	return x;
}
float Camera_GetRenderedInterpolatedY(const Camera* camera, double deltaTime)
{
	float y;
	if (camera->mLastRenderedPosition.Y == camera->mCurrentPosition.Y)
	{
		y = camera->mLastRenderedPosition.Y;
	}
	else
	{
		y = (float)Utils_GetInterpolated(deltaTime, camera->mCurrentPosition.Y, camera->mLastRenderedPosition.Y);
	}
	return y;
}
Matrix Camera_GetTranslation(float x, float y, float rotation, float zoom, float width, float height)
{
	Vector3 temp;

	temp.X = -x;
	temp.Y = -y;
	temp.Z = 0;
	Matrix model = Matrix_CreateTranslation(temp);

	Matrix rot = Matrix_CreateRotationZ(rotation);

	temp.X = zoom;
	temp.Y = zoom;
	temp.Z = 0;
	Matrix scale = Matrix_CreateScale(temp);

	temp.X = width * 0.5f;
	temp.Y = height * .5f;
	temp.Z = 0;
	Matrix world = Matrix_CreateTranslation(temp);

	return Matrix_Mul(&model, Matrix_Mul(&rot, Matrix_Mul(&scale, world)));
}
void Camera_Resize(Camera* camera, int32_t width, int32_t height)
{
	camera->mWorldWidth = width;
	camera->mWorldHeight = height;
}
bool Camera_MoveCameraSomewhereVector2(Camera* camera, bool immediate, Vector2 target, float speed)
{
	return Camera_MoveCameraSomewhere(camera, immediate, target.X, target.Y, speed);
}
bool Camera_MoveCameraSomewhere(Camera* camera, bool immediate, float targetX, float targetY, float speed)
{
	double distance = Get_Distance6(camera->mCurrentPosition.X, camera->mCurrentPosition.Y, targetX, targetY);
	if (speed < distance)
	{
		double angle = Math_GetAngle(camera->mCurrentPosition.X, camera->mCurrentPosition.Y, targetX, targetY);
		float movementX = (float)(Get_VectorFromRadianAngleX(angle) * speed);
		float movementY = (float)(Get_VectorFromRadianAngleY(angle) * speed);
		if (immediate)
		{
			camera->mCurrentPosition.X += movementX;
			camera->mCurrentPosition.Y += movementY;
		}
		else
		{
			camera->mTargetPosition.X = camera->mCurrentPosition.X + movementX;
			camera->mTargetPosition.Y = camera->mCurrentPosition.Y + movementY;
		}
		return false;
	}
	else
	{
		float movementX = targetX - camera->mCurrentPosition.X;
		float movementY = targetY - camera->mCurrentPosition.Y;
		if (immediate)
		{
			camera->mCurrentPosition.X += movementX;
			camera->mCurrentPosition.Y += movementY;
		}
		else
		{
			camera->mTargetPosition.X = camera->mCurrentPosition.X + movementX;
			camera->mTargetPosition.Y = camera->mCurrentPosition.Y + movementY;
		}
		return true;
	}
}
int32_t Camera_GetWidthMul(const Camera* camera, float mul)
{
	return (int32_t)Camera_GetWidthFloatMul(camera, mul);
}
int32_t Camera_GetWidth(const Camera* camera)
{
	return Camera_GetWidthMul(camera, 1);
}
float Camera_GetWidthFloatMul(const Camera* camera, float mul)
{
	return (camera->mWorldWidth * camera->mWorldZoom) * mul;
}
float Camera_GetWidthFloat(const Camera* camera)
{
	return Camera_GetWidthFloatMul(camera, 1);
}
int32_t Camera_GetHeightMul(const Camera* camera, float mul)
{
	return (int32_t)Camera_GetHeightFloatMul(camera, mul);
}
int32_t Camera_GetHeight(const Camera* camera)
{
	return Camera_GetHeightMul(camera, 1);
}
float Camera_GetHeightFloatMul(const Camera* camera, float mul)
{
	return (camera->mWorldHeight * camera->mWorldZoom) * mul;
}
float Camera_GetHeightFloat(const Camera* camera)
{
	return Camera_GetHeightFloatMul(camera, 1);
}
int32_t Camera_GetLeftMul(const Camera* camera, float mul)
{
	return (int32_t)Camera_GetLeftFloatMul(camera, mul);
}
int32_t Camera_GetLeft(const Camera* camera)
{
	return Camera_GetLeftMul(camera, 1);
}
float Camera_GetLeftFloatMul(const Camera* camera, float mul)
{
	return camera->mCurrentPosition.X - (Camera_GetWidthFloatMul(camera, mul) / 2);
}
float Camera_GetLeftFloat(const Camera* camera)
{
	return Camera_GetLeftFloatMul(camera, 1);
}
Vector2 Camera_GetUpperLeft(const Camera* camera)
{
	return Camera_GetUpperLeftMul(camera, 1);
}
Vector2 Camera_GetUpperLeftMul(const Camera* camera, float mul)
{
	Vector2 tempVec = { Camera_GetLeftFloatMul(camera, mul), Camera_GetTopFloatMul(camera, 1) };
	return tempVec;
}
int32_t Camera_GetTopMul(const Camera* camera, float mul)
{
	return (int32_t)Camera_GetTopFloatMul(camera, mul);
}
int32_t Camera_GetTop(const Camera* camera)
{
	return Camera_GetTopMul(camera, 1);
}
float Camera_GetTopFloatMul(const Camera* camera, float mul)
{
	return camera->mCurrentPosition.Y - (Camera_GetHeightFloatMul(camera, mul) / 2);
}
float Camera_GetTopFloat(const Camera* camera)
{
	return Camera_GetTopFloatMul(camera, 1);
}
int32_t Camera_GetRight(const Camera* camera)
{
	return Camera_GetLeftMul(camera, 1) + Camera_GetWidthMul(camera, 1);
}
int32_t Camera_GetRightMul(const Camera* camera, float mul)
{
	return Camera_GetLeftMul(camera, mul) + Camera_GetWidthMul(camera, mul);
}
float Camera_GetRightFloatMul(const Camera* camera, float mul)
{
	return Camera_GetLeftFloatMul(camera, mul) + Camera_GetWidthFloatMul(camera, mul);
}
float Camera_GetRightFloat(const Camera* camera)
{
	return Camera_GetRightFloatMul(camera, 1);
}
int32_t Camera_GetBottom(const Camera* camera)
{
	return Camera_GetTopMul(camera, 1) + Camera_GetHeightMul(camera, 1);
}
int32_t Camera_GetBottomMul(const Camera* camera, float mul)
{
	return Camera_GetTopMul(camera, mul) + Camera_GetHeightMul(camera, mul);
}
float Camera_GetBottomFloatMul(const Camera* camera, float mul)
{
	return Camera_GetTopFloatMul(camera, mul) + Camera_GetHeightFloatMul(camera, mul);
}
float Camera_GetBottomFloat(const Camera* camera)
{
	return Camera_GetBottomFloatMul(camera, 1);
}
float Camera_GetCenterXFloat(const Camera* camera)
{
	return Camera_GetLeftFloat(camera) + (Camera_GetWidthFloat(camera) / 2.0f);
}
int32_t Camera_GetCenterX(const Camera* camera)
{
	return Camera_GetLeft(camera) + (Camera_GetWidth(camera) / 2);
}
float Camera_GetCenterYFloat(const Camera* camera)
{
	return Camera_GetTopFloat(camera) + (Camera_GetHeightFloat(camera) / 2.0f);
}
int32_t Camera_GetCenterY(const Camera* camera)
{
	return Camera_GetTop(camera) + (Camera_GetHeight(camera) / 2);
}
int32_t Camera_GetX1Mul(const Camera* camera, float mul)
{
	return Math_MaxInt((Camera_GetLeftMul(camera, mul) / TILE_SIZE) - 1, 0);
}
int32_t Camera_GetX1(const Camera* camera)
{
	return Camera_GetX1Mul(camera, 1);
}
int32_t Camera_GetY1Mul(const Camera* camera, float mul)
{
	return Math_MaxInt((Camera_GetTopMul(camera, mul) / TILE_SIZE) - 1, 0);
}
int32_t Camera_GetY1(const Camera* camera)
{
	return Camera_GetY1Mul(camera, 1);
}
int32_t Camera_GetX2Mul(const Camera* camera, int32_t gridSizeX, float mul)
{
	return Math_MinInt(Camera_GetX1Mul(camera, mul) + (Camera_GetWidthMul(camera, mul) / TILE_SIZE) + 2 + 1, gridSizeX);
}
int32_t Camera_GetX2(const Camera* camera, int32_t gridSizeX)
{
	return Camera_GetX2Mul(camera, gridSizeX, 1);
}
int32_t Camera_GetY2Mul(const Camera* camera, int32_t gridSizeY, float mul)
{
	return Math_MinInt(Camera_GetY1Mul(camera, mul) + (Camera_GetHeightMul(camera, mul) / TILE_SIZE) + 2 + 1, gridSizeY);
}
int32_t Camera_GetY2(const Camera* camera, int32_t gridSizeY)
{
	return Camera_GetY2Mul(camera, gridSizeY, 1);
}
bool Camera_IntersectsCameraRect(const Camera* camera, const Rectangle* rect)
{
	return Camera_IntersectsCamera(camera, rect->X, rect->Y, rect->Width, rect->Height, 1);
}
bool Camera_IntersectsCameraRectMul(const Camera* camera, const Rectangle* rect, float mul)
{
	return Camera_IntersectsCamera(camera, rect->X, rect->Y, rect->Width, rect->Height, mul);
}
bool Camera_IntersectsCamera(const Camera* camera, int32_t x, int32_t y, int32_t width, int32_t height, float mul)
{
	Rectangle rect1 = { x, y, width, height };
	Rectangle rect2 = Camera_GetRectangle(camera, mul);
	return Rectangle_Intersects(&rect1, &rect2);
}
Rectangle Camera_GetRectangle(const Camera* camera, float mul)
{
	Rectangle returnRect = { Camera_GetLeftMul(camera, mul), Camera_GetTopMul(camera, mul), Camera_GetWidthMul(camera, mul), Camera_GetHeightMul(camera, mul) };
	return returnRect;
}
int32_t Camera_GetScreenWidthInTiles(const Camera* camera)
{
	return Camera_GetWidth(camera) / TILE_SIZE;
}
int32_t Camera_GetScreenHeightInTiles(const Camera* camera)
{
	return Camera_GetHeight(camera) / TILE_SIZE;
}
void Camera_Move(Camera* camera, float x, float y)
{
	camera->mTargetPosition.X += x;
	camera->mTargetPosition.Y += y;
}
void Camera_MoveToVector2(Camera* camera, Vector2 pos)
{
	Camera_MoveTo(camera, pos.X, pos.Y);
}
void Camera_MoveTo(Camera* camera, float x, float y)
{
	Camera_MoveToX(camera, x);
	Camera_MoveToY(camera, y);
}
void Camera_MoveToX(Camera* camera, float x)
{
	camera->mTargetPosition.X = x;
}
void Camera_MoveToY(Camera* camera, float y)
{
	camera->mTargetPosition.Y = y;
}
void Camera_SetPositionPoint(Camera* camera, Point pos)
{
	Camera_SetPosition(camera, (float)pos.X, (float)pos.Y);
}
void Camera_SetPositionVector2(Camera* camera, Vector2 pos)
{
	Camera_SetPosition(camera, pos.X, pos.Y);
}
void Camera_SetPosition(Camera* camera, float x, float y)
{
	camera->mTargetPosition.X = x;
	camera->mTargetPosition.Y = y;

	CameraSys_UpdateCamera(camera);

	camera->mCurrentPosition = camera->mTargetPosition;

	camera->mLastRenderedPosition = camera->mCurrentPosition;
}
void Camera_SetHingeDirectionX(Camera* camera, int32_t hingeDirectionX)
{
	if (hingeDirectionX != camera->mHingeDirection.X)
	{
		camera->mChangedCameraHingeX = true;
	}
	camera->mHingeDirection.X = hingeDirectionX;
}
void Camera_SetHingeDirectionY(Camera* camera, int32_t hingeDirectionY)
{
	if (hingeDirectionY != camera->mHingeDirection.Y)
	{
		camera->mChangedCameraHingeY = true;
	}
	camera->mHingeDirection.Y = hingeDirectionY;
}
void Camera_SetHingeDualDirectionX(Camera* camera, bool hingeDualDirectionX)
{
	if (hingeDualDirectionX != camera->mHingeDualDirectionX)
	{
		camera->mChangedCameraHingeX = true;
	}
	camera->mHingeDualDirectionX = hingeDualDirectionX;
}
void Camera_SetHingeDualDirectionY(Camera* camera, bool hingeDualDirectionY)
{
	if (hingeDualDirectionY != camera->mHingeDualDirectionY)
	{
		camera->mChangedCameraHingeY = true;
	}
	camera->mHingeDualDirectionY = hingeDualDirectionY;
}
void Camera_SetIsHingedCamera(Camera* camera, bool isHingedCamera)
{
	camera->mIsHingedCamera = isHingedCamera;
}
void Camera_SetHingeSpeedLimit(Camera* camera, float hingeLimitX, float hingeLimitY)
{
	camera->mHingeSpeedLimit.X = hingeLimitX;
	camera->mHingeSpeedLimit.Y = hingeLimitY;
}
void Camera_SetHingeGateLeft(Camera* camera, int32_t value)
{
	if (value != -1)
	{
		camera->mChangedCameraHingeX = true;
	}
	camera->mHingeGateLeft = value;
}
void Camera_SetHingeGateRight(Camera* camera, int32_t value)
{
	if (value != -1)
	{
		camera->mChangedCameraHingeX = true;
	}
	camera->mHingeGateRight = value;
}
void Camera_SetHingeGateTop(Camera* camera, int32_t value)
{
	if (value != -1)
	{
		camera->mChangedCameraHingeY = true;
	}
	camera->mHingeGateTop = value;
}
void Camera_SetHingeGateBottom(Camera* camera, int32_t value)
{
	if (value != -1)
	{
		camera->mChangedCameraHingeY = true;
	}
	camera->mHingeGateBottom = value;
}
void Camera_ShakeCameraMul(Camera* camera, float mul, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY)
{
	if (Cvars_GetAsBool(CVARS_USER_IS_SCREENSHAKE_DISABLED))
	{
		return;
	}

	camera->mNewShakeMul = mul;
	Random32* sharedRandom = Globals_GetSharedRandom();
	int32_t randomX = Random32_NextInt(sharedRandom, (maxX - minX) + 1);
	int32_t randomY = Random32_NextInt(sharedRandom, (maxY - minY) + 1);
	int32_t directionX = Random32_NextBoolean(sharedRandom) ? 1 : -1;
	int32_t directionY = Random32_NextBoolean(sharedRandom) ? 1 : -1;
	camera->mNewShake.X = (minX + randomX) * directionX;
	camera->mNewShake.Y = (minY + randomY) * directionY;
}
void Camera_ShakeCamera(Camera* camera, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY)
{
	Camera_ShakeCameraMul(camera, 1, minX, maxX, minY, maxY);
}
