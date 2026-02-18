/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Body.h"

#include "../render/DrawTool.h"
#include "../math/Math.h"
#include "../utils/Utils.h"

bool Body_DefaultBeforeColliderBody(Body* myBody, Body* otherBody, int32_t myDirectionX, int32_t myDirectionY, int32_t otherDirectionX, int32_t otherDirectionY, bool isVertical)
{
	return true;
}
void Body_DefaultColliderBody(Body* myBody, Body* otherBody, int32_t myDirectionX, int32_t myDirectionY, int32_t otherDirectionX, int32_t otherDirectionY, bool isVertical)
{

}
bool Body_DefaultColliderCollision(Body* myBody, int32_t x, int32_t y, int32_t width, int32_t height, int32_t myDirectionX, int32_t myDirectionY, int32_t collisionBit, bool isVertical)
{
	return true;
}
void Body_Setup(Body* body, int32_t width, int32_t height, bool isParticle)
{
	body->mIsParticle = isParticle;
	body->mPhysicsOriginalWidth = width * BODY_PHYSICS_SCALER;
	body->mPhysicsOriginalHeight = height * BODY_PHYSICS_SCALER;
	body->mPhysicsWidth = body->mPhysicsOriginalWidth;
	body->mPhysicsHeight = body->mPhysicsOriginalHeight;
}
void Body_UpdateLastRenderPosition(Body* body)
{
	body->mPhysicsLastRenderPosition = body->mPhysicsPosition;
}
bool Body_IsPushesThisThingSet(Body* body)
{
	if (Utils_StringEqualTo(body->mPushesThisThing, EE_STR_EMPTY))
	{
		return false;
	}
	else
	{
		return true;
	}
}
void Body_DrawBody(Body* body, SpriteBatch* spriteBatch, double deltaTime, int32_t depth, Color color)
{
	DrawTool_DrawRectangle2(spriteBatch, color, depth, Body_GetRect(body), 0, false);
}
void Body_ResetCollideWithCollision(Body* body)
{
	body->mCollideWithCollision = NULL;
	body->mCollideWithCollision = Body_DefaultColliderCollision;
}
void Body_ResetCollideWithBody(Body* body)
{
	body->mCollideWithBody = NULL;
	body->mCollideWithBody = Body_DefaultColliderBody;
}
void Body_ResetBeforeCollideWithBody(Body* body)
{
	body->mBeforeCollideWithBody = NULL;
	body->mBeforeCollideWithBody = Body_DefaultBeforeColliderBody;
}
void Body_Resize(Body* body, int32_t width, int32_t height)
{
	body->mPhysicsWidth = (width * BODY_PHYSICS_SCALER);
	body->mPhysicsHeight = (height * BODY_PHYSICS_SCALER);
}
void Body_FlipBody(Body* body)
{
	int32_t temp = body->mPhysicsWidth;
	body->mPhysicsWidth = body->mPhysicsHeight;
	body->mPhysicsHeight = temp;
}
void Body_MoveVector2(Body* body, Vector2 pixelMovement)
{
	Body_Move(body, pixelMovement.X, pixelMovement.Y);
}
void Body_Move(Body* body, float pixelX, float pixelY)
{
	if (!body->mIsDisabled && !body->mIsStatic)
	{
		body->mPhysicsVelocity.X += (pixelX * BODY_PHYSICS_SCALER);
		body->mPhysicsVelocity.Y += (pixelY * BODY_PHYSICS_SCALER);
	}
}
Vector2 Body_GetPosition(Body* body)
{
	return Vector2_DivFloat(body->mPhysicsPosition, BODY_PHYSICS_SCALER);
}
Rectangle Body_GetRect(Body* body)
{
	int32_t width = Body_GetWidth(body);
	int32_t height = Body_GetHeight(body);
	Vector2 position = Body_GetPosition(body);
	Rectangle tempRect;
	tempRect.X = (int32_t)(position.X - (width / 2));
	tempRect.Y = (int32_t)(position.Y - (height / 2));
	tempRect.Width = width;
	tempRect.Height = height;
	return tempRect;
}
Rectangle Body_GetPhysicsRect(Body* body)
{
	int32_t width = body->mPhysicsWidth;
	int32_t height = body->mPhysicsHeight;
	Rectangle tempRect;
	tempRect.X = (int32_t)(body->mPhysicsPosition.X - (width / 2));
	tempRect.Y = (int32_t)(body->mPhysicsPosition.Y - (height / 2));
	tempRect.Width = width;
	tempRect.Height = height;
	return tempRect;
}
int32_t Body_GetWidth(Body* body)
{
	return body->mPhysicsWidth / BODY_PHYSICS_SCALER;
}
void Body_SetWidth(Body* body, int32_t pixelWidth)
{
	body->mPhysicsWidth = pixelWidth * BODY_PHYSICS_SCALER;
}
int32_t Body_GetHeight(Body* body)
{
	return body->mPhysicsHeight / BODY_PHYSICS_SCALER;
}
void Body_SetHeight(Body* body, int32_t pixelHeight)
{
	body->mPhysicsHeight = pixelHeight * BODY_PHYSICS_SCALER;
}
int32_t Body_GetOriginalWidth(Body* body)
{
	return body->mPhysicsOriginalWidth / BODY_PHYSICS_SCALER;
}
int32_t Body_GetOriginalHeight(Body* body)
{
	return body->mPhysicsOriginalHeight / BODY_PHYSICS_SCALER;
}
void Body_SetPositionVector2(Body* body, Vector2 pixelMovement)
{
	Body_SetPosition(body, pixelMovement.X, pixelMovement.Y);
}
void Body_SetPosition(Body* body, float pixelX, float pixelY)
{
	Body_SetPositionX(body, pixelX);
	Body_SetPositionY(body, pixelY);
}
void Body_SetPositionX(Body* body, float pixelX)
{
	body->mPhysicsPosition.X = pixelX * BODY_PHYSICS_SCALER;
}
void Body_SetPositionY(Body* body, float pixelY)
{
	body->mPhysicsPosition.Y = pixelY * BODY_PHYSICS_SCALER;
}
void Body_ForcePositionVector2(Body* body, Vector2 pixelPosition)
{
	Body_ForcePosition(body, pixelPosition.X, pixelPosition.Y);
}
void Body_ForcePosition(Body* body, float pixelX, float pixelY)
{
	Body_ForcePositionX(body, pixelX);
	Body_ForcePositionY(body, pixelY);
}
void Body_ForcePositionX(Body* body, float pixelX)
{
	Body_SetPositionX(body, pixelX);
	body->mPhysicsLastRenderPosition.X = body->mPhysicsPosition.X;
	body->mPhysicsLastLogicalPosition.X = body->mPhysicsPosition.X;
}
void Body_ForcePositionY(Body* body, float pixelY)
{
	Body_SetPositionY(body, pixelY);
	body->mPhysicsLastRenderPosition.Y = body->mPhysicsPosition.Y;
	body->mPhysicsLastLogicalPosition.Y = body->mPhysicsPosition.Y;
}
void Body_ForceMoveVector2(Body* body, Vector2 pixelMovement)
{
	Body_ForceMove(body, pixelMovement.X, pixelMovement.Y);
}
void Body_ForceMove(Body* body, float pixelX, float pixelY)
{
	Body_ForceMoveX(body, pixelX);
	Body_ForceMoveY(body, pixelY);
}
void Body_ForceMoveX(Body* body, float pixelX)
{
	body->mPhysicsPosition.X += pixelX * BODY_PHYSICS_SCALER;
	body->mPhysicsLastRenderPosition.X = body->mPhysicsPosition.X;
	body->mPhysicsLastLogicalPosition.X = body->mPhysicsPosition.X;
}
void Body_ForceMoveY(Body* body, float pixelY)
{
	body->mPhysicsPosition.Y += pixelY * BODY_PHYSICS_SCALER;
	body->mPhysicsLastRenderPosition.Y = body->mPhysicsPosition.Y;
	body->mPhysicsLastLogicalPosition.Y = body->mPhysicsPosition.Y;
}
Vector2 Body_GetLastRenderPosition(Body* body)
{
	return Vector2_DivFloat(body->mPhysicsLastRenderPosition, BODY_PHYSICS_SCALER);
}
void Body_QueueMoveVector2(Body* body, Vector2 movement)
{
	Body_QueueMove(body, movement.X, movement.Y);
}
void Body_QueueMove(Body* body, float pixelX, float pixelY)
{
	body->mPhysicsQueuedVelocity.X += pixelX * BODY_PHYSICS_SCALER;
	body->mPhysicsQueuedVelocity.Y += pixelY * BODY_PHYSICS_SCALER;
}
Vector2 Body_GetQueuedVelocity(Body* body)
{
	return Vector2_DivFloat(body->mPhysicsQueuedVelocity, BODY_PHYSICS_SCALER);
}
void Body_CommitOnNextUpdateMove(Body* body)
{
	if (!body->mIsDisabled && !body->mIsStatic)
	{
		Vector2_AddAssign(&body->mPhysicsVelocity, body->mPhysicsQueuedVelocity);
		body->mPhysicsQueuedVelocity = Vector2_Zero;
	}
}
void Body_ForceVelocity(Body* body, Vector2 pixelVelocity)
{
	Body_ForceVelocityX(body, pixelVelocity.X);
	Body_ForceVelocityY(body, pixelVelocity.Y);
}
void Body_ForceVelocityX(Body* body, float pixelVelocityX)
{
	body->mPhysicsVelocity.X = pixelVelocityX * BODY_PHYSICS_SCALER;
}
void Body_ForceVelocityY(Body* body, float pixelVelocityY)
{
	body->mPhysicsVelocity.Y = pixelVelocityY * BODY_PHYSICS_SCALER;
}
Vector2 Body_GetVelocity(Body* body)
{
	return Vector2_DivFloat(body->mPhysicsVelocity, BODY_PHYSICS_SCALER);
}
void Body_UpdateLastDirection(Body* body)
{
	body->mLastDirection.X = Math_SignumFloat(body->mPhysicsVelocity.X);
	body->mLastDirection.Y = Math_SignumFloat(body->mPhysicsVelocity.Y);
}
Vector2 Body_GetMovementDifference(Body* body)
{
	Vector2 difference = Vector2_Sub(body->mPhysicsPosition, body->mPhysicsLastLogicalPosition);
	return Vector2_DivFloat(difference, BODY_PHYSICS_SCALER);
}
void Body_UpdateLastLogicalPositionToCurrent(Body* body)
{
	body->mPhysicsLastLogicalPosition = body->mPhysicsPosition;
}
Vector2 Body_GetLastLogicalPosition(Body* body)
{
	return Vector2_DivFloat(body->mPhysicsLastLogicalPosition, BODY_PHYSICS_SCALER);
}