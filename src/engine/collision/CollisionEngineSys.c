/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "CollisionEngineSys.h"

#include "../utils/Macros.h"
#include "../render/Color.h"
#include "../core/GameHelper.h"
#include "../core/Func.h"
#include "../globals/ObjectTypes.h"
#include "CollisionCheckData.h"
#include "../leveldata/LevelData.h"
#include "../render/DrawTool.h"
#include "../math/Math.h"
#include "../utils/Utils.h"
#include "../globals/Globals.h"
#include "../utils/Logger.h"
#include "../../third_party/stb_ds.h"
#include "../render/SpriteBatch.h"

#define TILE_SIZE GLOBAL_DEF_TILE_SIZE

#define RESOLUTION_LIMIT 40
#define CORNER_CHECKS_LEN 8

#define USE_SCREEN_DISTANCE_CULL
#ifdef USE_SCREEN_DISTANCE_CULL
#define SCREEN_DISTANCE_CULL_WIDTH (1280 * BODY_PHYSICS_SCALER)
#define SCREEN_DISTANCE_CULL_HEIGHT (720 * BODY_PHYSICS_SCALER)
#endif

static const Color mDebugColorActive = { 0, 255, 0, 191 };
static const Color mDebugColorTouched = { 127, 0, 127, 191 };
static const Color mDebugColor = { 128, 128, 128, 127 };
static const Color mDebugGridColor = { 255, 0, 0, 51 };
static Body** arr_bodies;

DrawRectangle* arr_debug_node_rectangles;
DrawRectangle* arr_debug_many_rectangles;

static bool _mCornerChecks[CORNER_CHECKS_LEN];
static ComponentPack* _mImprintPack;
static bool _mIsDynamicGravityDisabled;

static int32_t GetTilePos1D(int32_t i, int32_t j, CollisionEngine* data)
{
	return i + (j * data->mCollisionGridSize.Width);
}
static bool IsBodyUnderResolveLimit(Body* body)
{
	return (body->mInternalResolverCounter < RESOLUTION_LIMIT);
}

//private
void CollisionEngineSys_ResolveBody(CollisionEngine* data, bool isVertical, Body* body)
{
	if (IsBodyUnderResolveLimit(body))
	{
		CollisionEngineSys_ResolveWithBakedCollision(data, body, isVertical);

		CollisionEngineSys_ResolveWithOtherBodies(data, body, isVertical);
	}
	else
	{
		return;
	}
}
void CollisionEngineSys_CollisionHelper(Body* body, int32_t myDirectionX, int32_t myDirectionY, int32_t collisionBit)
{
	body->mLastCollisionBitTouched = collisionBit;

	if (myDirectionY == 1)
	{
		body->mFramesInAir = 0;
		body->mTouchedDown = true;
	}

	if (myDirectionY == -1)
	{
		body->mTouchedUp = true;
	}

	if (myDirectionX == 1)
	{
		body->mTouchedRight = true;
	}

	if (myDirectionX == -1)
	{
		body->mTouchedLeft = true;
	}
}
void CollisionEngineSys_ResolveWithOtherBodies(CollisionEngine* data, Body* bodyOne, bool isVertical)
{
	if (bodyOne->mIgnoreBodies || bodyOne->mIgnoreAllCollision || bodyOne->mIsDisabled || bodyOne->mIsStatic || !IsBodyUnderResolveLimit(bodyOne))
	{
		return;
	}

	bodyOne->mInternalResolverCounter += 1;

	for (int32_t i = 0; i < arrlen(arr_bodies); i += 1)
	{
		Body* bodyTwo = arr_bodies[i];

		if (CollisionEngineSys_IsCollisionValid(bodyOne, bodyTwo, isVertical)) // Basic checks, ending with collision detection callback code.
		{
			bool pushed = false;

			if (CollisionEngineSys_ResolvePush(data, bodyOne, bodyTwo, isVertical))
			{
				pushed = true;
			}
			else
			{
				if (CollisionEngineSys_ResolvePush(data, bodyTwo, bodyOne, isVertical))
				{
					pushed = true;
				}
			}

			if (pushed)
			{
				CollisionEngineSys_ResolveWithOtherBodies(data, bodyOne, isVertical); // Restart this function - the array from the broad search is no longer 100% valid after pushing.
				return;
			}
		}
	}
}
bool CollisionEngineSys_ResolvePush(CollisionEngine* data, Body* pushingBody, Body* pushedBody, bool isVertical)
{
	if (Body_IsPushesThisThingSet(pushingBody))
	{
		if ((Get_Entity(pushingBody->mOwner) == ENTITY_NOTHING) || (Get_Entity(pushedBody->mOwner) == ENTITY_NOTHING))
		{
			return false;
		}
		if (!Utils_StringEqualTo(pushingBody->mPushesThisThing, (Get_Name(Get_Entity(pushedBody->mOwner)))))
		{
			return false;
		}
	}
	else
	{
		if (!pushingBody->mPushes) // Push check time.
		{
			return false;
		}
	}

	if (pushedBody->mCannotBePushed || pushedBody->mIsStatic)
	{
		return false;
	}

	Rectangle otherPhysicsRectangle = Body_GetPhysicsRect(pushingBody);
	if (CollisionEngineSys_PushBody(data, false, isVertical, pushedBody,&otherPhysicsRectangle))
	{
		Rectangle pushedBodyRectangle = Body_GetRect(pushedBody);
		Rectangle pushingBodyRectangle = Body_GetRect(pushingBody);
		if (Rectangle_Center(&pushedBodyRectangle).Y < Rectangle_Center(&pushingBodyRectangle).Y)
		{
			if (pushingBody->mIsMovingPlatform && isVertical && (pushedBody->mMovingPlatformEntityNumber == ENTITY_NOTHING) 
				&& (pushedBody->mLastDirection.Y > 0)) // Moving platform check.
			{
				pushedBody->mMovingPlatformEntityNumber = pushingBody->mOwner;
			}
		}

		if (data->mIsNullDividePlus)
		{
			CollisionEngineSys_HandleNullDividePlusPushing(data, pushingBody, pushedBody, isVertical);
		}

		return true;
	}
	else
	{
		return false;
	}
}
void CollisionEngineSys_HandleNullDividePlusPushing(CollisionEngine* data, Body* pushingBody, Body* pushedBody, bool isVertical)
{
	if (data->mDefaultBodyPushAmount != 0)
	{
		int32_t pushingDirectionX = pushingBody->mLastDirection.X;
		int32_t pushingDirectionY = pushingBody->mLastDirection.Y;

		if (isVertical)
		{
			pushingDirectionX = 0;
		}
		else
		{
			pushingDirectionY = 0;
		}

		if (pushingBody->mBodyPushAmount == 0)
		{
			Body_QueueMove(pushedBody, (float)(pushingDirectionX * data->mDefaultBodyPushAmount), (float)(pushingDirectionY * data->mDefaultBodyPushAmount));
			Body_QueueMove(pushingBody, (float)(-pushingDirectionX * data->mDefaultBodyPushAmount), (float)(-pushingDirectionY * data->mDefaultBodyPushAmount));
		}
		else
		{
			Body_QueueMove(pushedBody, (float)(pushingDirectionX * pushingBody->mBodyPushAmount), (float)(pushingDirectionY * pushingBody->mBodyPushAmount));
			Body_QueueMove(pushingBody, (float)(-pushingDirectionX * pushedBody->mBodyPushAmount), (float)(-pushingDirectionY * pushedBody->mBodyPushAmount));
		}
	}
}
void CollisionEngineSys_ImprintToCollisionGridHelper(CollisionEngine* data, CollisionImprintData* imprint)
{
	for (int32_t i = 0; i < imprint->mWidth; i += 1)
	{
		for (int32_t j = 0; j < imprint->mHeight; j += 1)
		{
			int32_t tempX = imprint->mPosition.X + i;
			int32_t tempY = imprint->mPosition.Y + j;
			if ((tempX >= 0) && (tempY >= 0) && (tempX < data->mCollisionGridSize.Width) && (tempY < data->mCollisionGridSize.Height))
			{
				data->mCollisionGrid[GetTilePos1D(tempX, tempY, data)] = imprint->mType;
			}
			else
			{
				Logger_LogError("Collision imprint out of bounds");
			}
		}
	}
}
bool CollisionEngineSys_IsPointSafe(CollisionEngine* data, int32_t x, int32_t y)
{
	int32_t gridWidth = data->mCollisionGridSize.Width;
	int32_t gridHeight = data->mCollisionGridSize.Height;
	if ((x < 0) || (y < 0) || (x >= gridWidth) || (y >= gridHeight))
	{
		return false;
	}

	return true;
}
bool CollisionEngineSys_CheckPoint(CollisionEngine* data, float checkX, float checkY, int32_t directionX, int32_t directionY, Body* body, bool isVertical)
{
	Point point = CollisionEngineSys_GetCollisionGridPosition(checkX, checkY);
	if ((point.X >= 0) && (point.Y >= 0) && (point.X < data->mCollisionGridSize.Width) // Check for safety make this a universal method some time.
		&& (point.Y < data->mCollisionGridSize.Height))
	{
		if (data->mCollisionGrid[GetTilePos1D(point.X, point.Y, data)] != 0)
		{
			int32_t tempX = (point.X * TILE_SIZE);
			int32_t tempY = (point.Y * TILE_SIZE);
			int32_t tempWidth = TILE_SIZE;
			int32_t tempHeight = TILE_SIZE;

			//collision check
			bool collide = false;
			int32_t physicsScaler = BODY_PHYSICS_SCALER;
			Rectangle physicsRect = Body_GetPhysicsRect(body);
			Rectangle testRect = { tempX * physicsScaler, tempY * physicsScaler, tempWidth * physicsScaler, tempHeight * physicsScaler };
			if (Rectangle_Intersects(&physicsRect, &testRect))
			{
				collide = true;
			}

			if (collide)
			{
				CollisionCheckData collisionCheckData = { 0 };
				int32_t collisionToCheck = data->mCollisionGrid[GetTilePos1D(point.X, point.Y, data)];
				if (!FixedListEightInt_Contains(&body->mLastCollisionsTouched, collisionToCheck))
				{
					FixedListEightInt_Add(&body->mLastCollisionsTouched, collisionToCheck);
				}
				if (FixedListEightInt_Contains(&body->mIgnore, collisionToCheck))
				{
					return false;
				}

				collisionCheckData.mCollisionToReport = collisionToCheck;

				GameHelper_BakedCollisionCheck((float)tempX, (float)tempY, collisionToCheck, body, isVertical, &collisionCheckData);

				if (!collisionCheckData.mDisableCollision)
				{
					bool move = body->mCollideWithCollision(body, tempX, tempY, tempWidth, tempHeight, directionX, directionY, collisionCheckData.mCollisionToReport, isVertical);

					if (move)
					{
						Rectangle otherPhysicsRectangle = { tempX * physicsScaler, tempY * physicsScaler, tempWidth * physicsScaler, tempHeight * physicsScaler };
						CollisionEngineSys_PushBody(data, true, isVertical, body, &otherPhysicsRectangle);

						body->mTouchedCollision = true;

						CollisionEngineSys_CollisionHelper(body, directionX, directionY, collisionCheckData.mCollisionToReport);
						return true;
					}
				}
			}
		}
	}
	return false;
}

//public
void CollisionEngineSys_DisableDynamicGravity(void)
{
	_mIsDynamicGravityDisabled = true;
}
void CollisionEngineSys_CopyCollisionGridFromPristine(CollisionEngine* data)
{
	Utils_memcpy(data->mCollisionGrid, data->mCollisionGridPristine, sizeof(int32_t) * data->mCollisionGridSize.Width * data->mCollisionGridSize.Height);
}
void CollisionEngineSys_UpdateRoutine(Entity owner, CollisionEngine* data)
{
	_mImprintPack = Get_ComponentPack(C_CollisionImprintData);
	ComponentPack* bodyPack = Get_ComponentPack(C_Body);

	// Prepare collision grid
	CollisionEngineSys_CopyCollisionGridFromPristine(data);
	PackIterator iter = PackIterator_Begin;
	while (ComponentPack_Next(_mImprintPack, &iter))
	{
		CollisionEngineSys_ImprintToCollisionGridHelper(data, (CollisionImprintData*)iter.mComponent);
	}
	//

	// House keeping
	iter = PackIterator_Begin;
	while (ComponentPack_Next(bodyPack, &iter))
	{
		Body* body = (Body*)iter.mComponent;
		if (!body->mIsDisabled)
		{
			CollisionEngineSys_HandleBodiesHousekeepingHelper(data, body);
			arrput(arr_bodies, body);
		}
	}
	//

	CollisionEngineSys_Step(data, true); // Step Y

	CollisionEngineSys_Step(data, false); // Step X

	for (int32_t i = 0; i < arrlen(arr_bodies); i += 1)
	{
		Body* body = arr_bodies[i];
		body->mPhysicsVelocity = Vector2_Zero;
	}

	arrsetlen(arr_debug_many_rectangles, 0);

	CollisionEngineSys_DebugGenerateDebugRectangles(data);

	arrsetlen(arr_bodies, 0);
	Utils_ResetArrayAsBool(_mCornerChecks, CORNER_CHECKS_LEN, false);
	_mImprintPack = NULL;
}
bool CollisionEngineSys_PushBody(CollisionEngine* data, bool isBakedCollision, bool isVertical, Body* body, const Rectangle* otherPhysicsRectangle)
{
	int32_t xDiff;
	int32_t yDiff;

	Rectangle myPhysicsRectangle = Body_GetPhysicsRect(body);

	int32_t rightDif = Rectangle_Left(otherPhysicsRectangle) - Rectangle_Right(&myPhysicsRectangle) - 1; // Get minimums.
	int32_t leftDif = Rectangle_Right(otherPhysicsRectangle) - Rectangle_Left(&myPhysicsRectangle) + 1;
	int32_t bottomDif = Rectangle_Top(otherPhysicsRectangle) - Rectangle_Bottom(&myPhysicsRectangle) - 1;
	int32_t topDif = Rectangle_Bottom(otherPhysicsRectangle) - Rectangle_Top(&myPhysicsRectangle) + 1;

	if (Math_abs(topDif) < Math_abs(bottomDif)) // Find minimum.
	{
		yDiff = topDif;
	}
	else
	{
		yDiff = bottomDif;
	}

	if (Math_abs(rightDif) < Math_abs(leftDif))
	{
		xDiff = rightDif;
	}
	else
	{
		xDiff = leftDif;
	}

	if (isVertical) // Zero out the incorrect one.
	{
		xDiff = 0;
	}
	else
	{
		yDiff = 0;
	}

	body->mPhysicsPosition.X += xDiff;
	body->mPhysicsPosition.Y += yDiff;

	bool hasPushed = false;
	if ((xDiff != 0) || (yDiff != 0)) // We have pushed if these are not 0.
	{
		if (isVertical) // Lock to integer after being pushed.
		{
			body->mPhysicsPosition.Y = (float)((int32_t)(body->mPhysicsPosition.Y));
		}
		else
		{
			body->mPhysicsPosition.X = (float)((int32_t)(body->mPhysicsPosition.X));
		}

		if (!body->mUsesBulletCollisionEngine && !body->mIsParticle)
		{
			if (body->mPushes || Body_IsPushesThisThingSet(body)) // Recursively resolve bodies until there is no problems.
			{
				CollisionEngineSys_ResolveBody(data, isVertical, body);
			}
		}

		hasPushed = true;
	}

	return hasPushed;
}
bool CollisionEngineSys_IsCollisionValid(Body* bodyOne, Body* bodyTwo, bool isVertical)
{
#ifdef USE_SCREEN_DISTANCE_CULL
	if ((Math_fabsf(bodyOne->mPhysicsPosition.X - bodyTwo->mPhysicsPosition.X) > SCREEN_DISTANCE_CULL_WIDTH) ||
		(Math_fabsf(bodyOne->mPhysicsPosition.Y - bodyTwo->mPhysicsPosition.Y) > SCREEN_DISTANCE_CULL_HEIGHT))
	{
		return false;
	}
#endif

	if (bodyOne->mOwner == bodyTwo->mOwner)
	{
		return false;
	}

	if (bodyOne->mIsDisabled || bodyTwo->mIsDisabled)
	{
		return false;
	}

	if (bodyOne->mIgnoreBodies || bodyTwo->mIgnoreBodies)
	{
		return false;
	}

	if (bodyOne->mIgnoreAllCollision || bodyTwo->mIgnoreAllCollision)
	{
		return false;
	}

	if (bodyOne->mIsStatic && bodyTwo->mIsStatic)
	{
		return false;
	}

	if (((bodyOne->mType == OBJECTTYPES_PLAYER) && bodyTwo->mIgnorePlayer) ||
		((bodyTwo->mType == OBJECTTYPES_PLAYER) && bodyOne->mIgnorePlayer))
	{
		return false;
	}

	if (((bodyOne->mType != OBJECTTYPES_PLAYER) && bodyTwo->mIgnoreEverythingExceptPlayer) ||
		(bodyOne->mIgnoreEverythingExceptPlayer && (bodyTwo->mType != OBJECTTYPES_PLAYER)))
	{
		return false;
	}

	if ((bodyOne->mCollideWithBulletsOnly && !bodyTwo->mIsBullet) || (!bodyOne->mIsBullet && bodyTwo->mCollideWithBulletsOnly))
	{
		return false;
	}

	if (!bodyOne->mCollideWithBulletsNoMatterWhat && !bodyTwo->mCollideWithBulletsNoMatterWhat)
	{
		if ((bodyOne->mIgnoreBullets && bodyTwo->mIsBullet) || (bodyTwo->mIgnoreBullets && bodyOne->mIsBullet))
		{
			return false;
		}
	}

	if (FixedListEightInt_Contains(&bodyOne->mIgnore, bodyTwo->mType))
	{
		return false;
	}

	if (FixedListEightInt_Contains(&bodyTwo->mIgnore, bodyOne->mType))
	{
		return false;
	}

	//Rectangle intersection routine split in two for optimization reasons...
	//X AXIS
	int32_t rect1_width = bodyOne->mPhysicsWidth;
	int32_t rect1_x = (int32_t)(bodyOne->mPhysicsPosition.X - (rect1_width / 2));

	int32_t rect2_width = bodyTwo->mPhysicsWidth;
	int32_t rect2_x = (int32_t)(bodyTwo->mPhysicsPosition.X - (rect2_width / 2));

	if ((rect2_x >= (rect1_x + rect1_width)) || (rect1_x >= (rect2_x + rect2_width)))
	{
		return false;
	}
	//
	//Y AXIS
	int32_t rect1_height = bodyOne->mPhysicsHeight;
	int32_t rect1_y = (int32_t)(bodyOne->mPhysicsPosition.Y - (rect1_height / 2));

	int32_t rect2_height = bodyTwo->mPhysicsHeight;
	int32_t rect2_y = (int32_t)(bodyTwo->mPhysicsPosition.Y - (rect2_height / 2));

	if ((rect2_y >= (rect1_y + rect1_height)) || (rect1_y >= (rect2_y + rect2_height)))
	{
		return false;
	}
	//
	//

	/*
	bool rect_intersects = ((rect2_x < (rect1_x + rect1_width)) &&
		(rect1_x < (rect2_x + rect2_width)) &&
		(rect2_y < (rect1_y + rect1_height)) &&
		(rect1_y < (rect2_y + rect2_height)));

	if (!rect_intersects)
	{
		return false;
	}
	*/

	//Hard coded this...
	/*
	if (!Body_GetPhysicsRect(bodyOne).Intersects(Body_GetPhysicsRect(bodyTwo)))
	{
		return false;
	}
	*/

	if (!bodyOne->mIsMovingPlatform || !bodyTwo->mIsMovingPlatform) // One way platform check.
	{
		if (bodyOne->mIsOneWayPlatform || bodyTwo->mIsOneWayPlatform)
		{
			if (isVertical)
			{
				bool valid;
				if (bodyOne->mIsOneWayPlatform)
				{
					valid = CollisionEngineSys_DoPlatformCollision(bodyOne, bodyTwo);
				}
				else
				{
					valid = CollisionEngineSys_DoPlatformCollision(bodyTwo, bodyOne);
				}
				if (!valid)
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}

	int32_t bodyOneDirectionX = isVertical ? 0 : bodyOne->mLastDirection.X; // Directions that are given to callbacks are only for the current step type.
	int32_t bodyOneDirectionY = isVertical ? bodyOne->mLastDirection.Y : 0;
	int32_t bodyTwoDirectionX = isVertical ? 0 : bodyTwo->mLastDirection.X;
	int32_t bodyTwoDirectionY = isVertical ? bodyTwo->mLastDirection.Y : 0;

	bool beforeCollisionCheck1 = bodyOne->mBeforeCollideWithBody(bodyOne, bodyTwo, bodyOneDirectionX, bodyOneDirectionY, bodyTwoDirectionX, bodyTwoDirectionY, isVertical);
	bool beforeCollisionCheck2 = bodyTwo->mBeforeCollideWithBody(bodyTwo, bodyOne, bodyTwoDirectionX, bodyTwoDirectionY, bodyOneDirectionX, bodyOneDirectionY, isVertical);
	if (!beforeCollisionCheck1 || !beforeCollisionCheck2)
	{
		return false;
	}

	if (!FixedListEightInt_Contains(&bodyOne->mContact, bodyTwo->mOwner) && !FixedListEightInt_Contains(&bodyTwo->mContact, bodyOne->mOwner)) // Contact confirmed! Callback code going.
	{
		if (bodyTwo->mPushes)
		{
			CollisionEngineSys_CollisionHelper(bodyOne, bodyOneDirectionX, bodyOneDirectionY, bodyOne->mType);
		}
		bodyOne->mCollideWithBody(bodyOne, bodyTwo, bodyOneDirectionX, bodyOneDirectionY, bodyTwoDirectionX, bodyTwoDirectionY, isVertical);
		FixedListEightInt_Add(&bodyOne->mContact, bodyTwo->mOwner);

		if (bodyOne->mPushes)
		{
			CollisionEngineSys_CollisionHelper(bodyTwo, bodyTwoDirectionX, bodyTwoDirectionY, bodyTwo->mType);
		}
		bodyTwo->mCollideWithBody(bodyTwo, bodyOne, bodyTwoDirectionX, bodyTwoDirectionY, bodyOneDirectionX, bodyOneDirectionY, isVertical);
		FixedListEightInt_Add(&bodyTwo->mContact, bodyOne->mOwner);
	}

	bodyOne->mTouchedBody = true;
	bodyTwo->mTouchedBody = true;

	return true;
}
void CollisionEngineSys_UpdateParticleBody(CollisionEngine* data, Body* body)
{
	body->mInternalResolverCounter = 0;
	if (body->mIsDisabled)
	{
		return;
	}

	Body_UpdateLastLogicalPositionToCurrent(body);
	Body_UpdateLastDirection(body);

	body->mPhysicsPosition.Y += body->mPhysicsVelocity.Y; // Y
	CollisionEngineSys_ResolveWithBakedCollision(data, body, true);
	if (!body->mIsDisabled) // X
	{
		body->mPhysicsPosition.X += body->mPhysicsVelocity.X;
		CollisionEngineSys_ResolveWithBakedCollision(data, body, false);
	}

	body->mPhysicsVelocity = Vector2_Zero;
}
void CollisionEngineSys_RemoveAllCollisionImprints(void)
{
	GameState_UnsetAll(Get_ActiveGameState(), C_CollisionImprintData);
}
void CollisionEngineSys_ImprintToCollisionGrid(CollisionEngine* data, float pixelX, float pixelY, int32_t width, int32_t height, int32_t type)
{
	Entity collisionEntity = Get_FirstSetEntity(C_CollisionEngine);
	ComponentPack* imprintPack = Get_ComponentPack(C_CollisionImprintData);
	Point point = CollisionEngineSys_GetCollisionGridPosition(pixelX, pixelY);
	CollisionImprintData* imprintData = (CollisionImprintData*)ComponentPack_Set2(imprintPack, collisionEntity, true);
	imprintData->mPosition = point;
	imprintData->mWidth = width;
	imprintData->mHeight = height;
	imprintData->mType = type;
	CollisionEngineSys_ImprintToCollisionGridHelper(data, imprintData);
}
int32_t CollisionEngineSys_GetCollisionBit(CollisionEngine* data, float x, float y)
{
	Point point = CollisionEngineSys_GetCollisionGridPosition(x, y);
	return data->mCollisionGrid[GetTilePos1D(point.X, point.Y, data)];
}
int32_t CollisionEngineSys_GetCollisionBitSafe(CollisionEngine* data, float x, float y, int32_t returnValue)
{
	Point point = CollisionEngineSys_GetCollisionGridPosition(x, y);
	if (!CollisionEngineSys_IsPointSafe(data, point.X, point.Y))
	{
		return returnValue;
	}
	return data->mCollisionGrid[GetTilePos1D(point.X, point.Y, data)];
}
int32_t CollisionEngineSys_GetCollisionBitSafeGrid(CollisionEngine* data, int32_t x, int32_t y, int32_t returnValue)
{
	if (!CollisionEngineSys_IsPointSafe(data, x, y))
	{
		return returnValue;
	}

	return data->mCollisionGrid[GetTilePos1D(x, y, data)];
}
int32_t CollisionEngineSys_GetCollisionBitGrid(CollisionEngine* data, int32_t x, int32_t y)
{
	return data->mCollisionGrid[GetTilePos1D(x, y, data)];
}
void CollisionEngineSys_SetupCollisionGrid(CollisionEngine* data, LevelData* level)
{
	data->mCollisionGridSize.X = 0;
	data->mCollisionGridSize.Y = 0;
	data->mCollisionGridSize.Width = LevelData_GetGridSizeWidth(level);
	data->mCollisionGridSize.Height = LevelData_GetGridSizeHeight(level);

	if (level->_mIsMetaMap)
	{
		data->mCollisionGridPristine = LevelData_CreateEmptyCollisionArray(level);
		data->mCollisionGrid = LevelData_CreateEmptyCollisionArray(level);
	}
	else
	{
		data->mCollisionGridPristine = LevelData_CreateCollisionArray(level);
		data->mCollisionGrid = LevelData_CreateEmptyCollisionArray(level);
		CollisionEngineSys_CopyCollisionGridFromPristine(data);
	}
}
bool CollisionEngineSys_DoPlatformCollision(Body* platformBody, Body* thingBody)
{
	Vector2 velocity = Body_GetVelocity(thingBody);
	Vector2 lastLogicalPosition = Body_GetLastLogicalPosition(thingBody);
	float halfHeightOffset = (float)(Body_GetHeight(thingBody) / 2);
	float leeway = GameHelper_GetMovingPlatformLeeway();
	float heightCheck = lastLogicalPosition.Y + halfHeightOffset - leeway;
	Rectangle bodyRectangle = Body_GetRect(platformBody);
	float platformTop = (float)Rectangle_Top(&bodyRectangle);
	if ((velocity.Y <= 0) || (heightCheck >= platformTop))
	{
		return false;
	}
	return true;
}
Point CollisionEngineSys_GetCollisionGridPosition(float x, float y)
{
	Point temp = { (int32_t)(x / TILE_SIZE), (int32_t)(y / TILE_SIZE) };
	return temp;
}
bool CollisionEngineSys_CheckSurroundingCollision(CollisionEngine* data, int32_t bodyX, int32_t bodyY, int32_t xDirection, int32_t yDirection, 
	const int32_t* collisionToCheck, int32_t collisionToCheckLen)
{
	Point point = Point_Add(CollisionEngineSys_GetCollisionGridPosition((float)bodyX, (float)bodyY), Point_Create(xDirection, yDirection));

	if ((point.X < 0) || (point.Y < 0) || (point.X >= data->mCollisionGridSize.Width) || (point.Y >= data->mCollisionGridSize.Height))
	{
		return false;
	}

	int32_t tilePos1D = GetTilePos1D(point.X, point.Y, data);
	if (collisionToCheckLen == 0)
	{
		if (data->mCollisionGrid[tilePos1D] != OBJECTTYPES_NOTHING)
		{
			return true;
		}
	}
	else
	{
		for (int32_t i = 0; i < collisionToCheckLen; i += 1)
		{
			if (collisionToCheck[i] != OBJECTTYPES_NOTHING)
			{
				if (data->mCollisionGrid[tilePos1D] == collisionToCheck[i])
				{
					return true;
				}
			}
		}
	}

	return false;
}
bool CollisionEngineSys_IsRectIntersectsCollision(CollisionEngine* data, int32_t x, int32_t y, int32_t width, int32_t height)
{
	int32_t expansionX = ((width / TILE_SIZE) + 1);
	int32_t expansionY = ((height / TILE_SIZE) + 1);

	for (int32_t i = 0; i < 1 + (expansionX * 2); i += 1)
	{
		for (int32_t j = 0; j < 1 + (expansionY * 2); j += 1)
		{
			Point point = CollisionEngineSys_GetCollisionGridPosition((float)x, (float)y);
			int32_t tileX = point.X - expansionX;
			int32_t tileY = point.Y - expansionY;

			int32_t currentX = tileX + i;
			int32_t currentY = tileY + j;

			if (!CollisionEngineSys_IsPointSafe(data, currentX, currentY))
			{
				continue;
			}

			if (data->mCollisionGrid[GetTilePos1D(currentX, currentY, data)] == OBJECTTYPES_NOTHING)
			{
				continue;
			}

			currentX *= TILE_SIZE;
			currentY *= TILE_SIZE;

			Rectangle rect1 = { x, y, width, height };
			Rectangle rect2 = { currentX, currentY, TILE_SIZE, TILE_SIZE };
			if (Rectangle_Intersects(&rect1, &rect2))
			{
				return true;
			}
		}
	}
	return false;
}
bool CollisionEngineSys_CheckFeetCollision(CollisionEngine* data, Body* body, bool checkLeft, bool checkRight, int32_t offset, int32_t collisionBit)
{
	Rectangle rect = Body_GetRect(body); // Add Vector2s (quadrants of the rectangle)

	Point pnt1 = CollisionEngineSys_GetCollisionGridPosition((float)(Rectangle_Left(&rect) + offset), (float)(Rectangle_Bottom(&rect) + 1));
	Point pnt2 = CollisionEngineSys_GetCollisionGridPosition((float)(Rectangle_Right(&rect) - offset), (float)(Rectangle_Bottom(&rect) + 1));

	if ((pnt1.X < 0) || (pnt1.Y < 0) || (pnt1.X >= data->mCollisionGridSize.Width) || (pnt1.Y >= data->mCollisionGridSize.Height))
	{
		return false;
	}

	if (collisionBit == OBJECTTYPES_ALL)
	{
		if (checkLeft)
		{
			if (data->mCollisionGrid[GetTilePos1D(pnt1.X, pnt1.Y, data)] != OBJECTTYPES_NOTHING)
			{
				return true;
			}
		}

		if (checkRight)
		{
			if (data->mCollisionGrid[GetTilePos1D(pnt2.X, pnt2.Y, data)] != OBJECTTYPES_NOTHING)
			{
				return true;
			}
		}
	}
	else
	{
		if (checkLeft)
		{
			if (data->mCollisionGrid[GetTilePos1D(pnt1.X, pnt1.Y, data)] == collisionBit)
			{
				return true;
			}
		}

		if (checkRight)
		{
			if (data->mCollisionGrid[GetTilePos1D(pnt2.X, pnt2.Y, data)] == collisionBit)
			{
				return true;
			}
		}
	}
	return false;
}
bool CollisionEngineSys_HasLineOfSight(CollisionEngine* data, float x1, float y1, float x2, float y2, bool respectOneWays)
{
	return CollisionEngineSys_HasLineOfSight2(data, false, NULL, x1, y1, x2, y2, respectOneWays);
}
bool CollisionEngineSys_HasLineOfSight2(CollisionEngine* data, bool draw, SpriteBatch* spriteBatch, float x1, float y1, float x2, float y2, bool respectOneWays)
{
	int32_t directionX = x2 - x1 > 0 ? 1 : -1;
	int32_t directionY = y2 - y1 > 0 ? 1 : -1;

	if (x1 == x2) //Undefined slope, nudge position
	{
		x2 += 1;
	}

	float m = Math_GetSlope(x1, y1, x2, y2);
	float b = Math_GetIntercept(x1, y1, m);
	int32_t distance = (int32_t)(Math_GetLineDistance(x1, y1, x2, y2));
	float seg = (x2 - x1) / distance;
	for (int32_t i = 0; i < distance; i += 1)
	{
		float x = x1 + (i * seg);
		float y = (m * x) + b;

		int32_t collision = CollisionEngineSys_GetCollisionBitSafe(data, x, y, 0);
		if (collision == CollisionEngineSys_GetPlatformUp() && respectOneWays)
		{
			if (directionY == 1)
			{
				return false;
			}
		}
		else if (collision == CollisionEngineSys_GetPlatformRight() && respectOneWays)
		{
			if (directionX == -1)
			{
				return false;
			}
		}
		else if (collision == CollisionEngineSys_GetPlatformDown() && respectOneWays)
		{
			if (directionY == -1)
			{
				return false;
			}
		}
		else if (collision == CollisionEngineSys_GetPlatformLeft() && respectOneWays)
		{
			if (directionX == 1)
			{
				return false;
			}
		}
		else
		{
			if (collision != 0)
			{
				if (GameHelper_IsCollisionSolidForLineOfSight(collision))
				{
					return false;
				}
			}
		}

		if (draw)
		{
			DrawTool_DrawRectangle2(spriteBatch, COLOR_RED, 100, Rectangle_Create((int32_t)x, (int32_t)y, 2, 2), 0, true);
		}
	}

	return true;
}
int32_t CollisionEngineSys_GetPlatformDown(void)
{
	return GAMEHELPER_PLATFORM_DOWN;
}
int32_t CollisionEngineSys_GetPlatformLeft(void)
{
	return GAMEHELPER_PLATFORM_LEFT;
}
int32_t CollisionEngineSys_GetPlatformRight(void)
{
	return GAMEHELPER_PLATFORM_RIGHT;
}
int32_t CollisionEngineSys_GetPlatformUp(void)
{
	return GAMEHELPER_PLATFORM_UP;
}
float CollisionEngineSys_GetCurrentDeceleration(float maxDecel, int32_t framesInAir)
{
	float deceleration = framesInAir * GameHelper_GetGravityDeceleration();
	if (deceleration > maxDecel)
	{
		deceleration = maxDecel;
	}
	return deceleration;
}
void CollisionEngineSys_HandleDynamicGravityY(Body* body)
{
	if (body->mTouchedDown)
	{
		body->mFramesInAir = 0;
	}
	else
	{
		body->mFramesInAir += 1;
	}

	if (!body->mTouchedDown)
	{
		Body_Move(body, 0, CollisionEngineSys_GetCurrentDeceleration(body->mMaxDeceleration, body->mFramesInAir));
	}
	else
	{
		Body_Move(body, 0, 1);
	}
}
void CollisionEngineSys_HandleBodiesHousekeepingHelper(CollisionEngine* data, Body* body)
{
	FixedListEightInt_Clear(&body->mLastCollisionsTouched);
	FixedListEightInt_Clear(&body->mContact);
	Body_UpdateLastLogicalPositionToCurrent(body);
	Body_CommitOnNextUpdateMove(body);

	if (!body->mIsStatic)
	{
		if (!body->mIgnoreGravityX && data->mFixedGravity.X != 0)
		{
			Body_Move(body, data->mFixedGravity.X, 0);
		}
		if (!body->mIgnoreGravityY)
		{
			if (!body->mIgnoreFixedGravityY && data->mFixedGravity.Y != 0)
			{
				Body_Move(body, 0, data->mFixedGravity.Y);
			}
			if (!body->mUseOnlyFixedGravity && !_mIsDynamicGravityDisabled)
			{
				CollisionEngineSys_HandleDynamicGravityY(body);
			}
		}
		else
		{
			body->mFramesInAir = 0;
		}
	}

	body->mTouchedUp = false;
	body->mTouchedDown = false;
	body->mTouchedRight = false;
	body->mTouchedLeft = false;
	body->mTouchedBody = false;
	body->mTouchedCollision = false;
	body->mWasPushedByOneWayCollision = false;
	body->mHackTouchedWind = false;

	Body_UpdateLastDirection(body);

	if (body->mMovingPlatformEntityNumber != ENTITY_NOTHING) // Moving platform code. Body is tied to a platform basically.
	{
		Entity movingPlatform = Get_Entity(body->mMovingPlatformEntityNumber);
		if (movingPlatform != ENTITY_NOTHING)
		{
			Body* movingPlatformBody = Get_Body(movingPlatform);
			body->mPhysicsVelocity.X += movingPlatformBody->mPhysicsVelocity.X;
			body->mPhysicsPosition.Y = ((movingPlatformBody->mPhysicsPosition.Y + movingPlatformBody->mPhysicsVelocity.Y) - (movingPlatformBody->mPhysicsHeight / 2) - (body->mPhysicsHeight / 2) - 1);
		}
		body->mMovingPlatformEntityNumber = ENTITY_NOTHING;
	}
}
void CollisionEngineSys_Step(CollisionEngine* data, bool isVertical)
{
	for (int32_t i = 0; i < arrlen(arr_bodies); i += 1) // First move and test high priority bodies....
	{
		Body* body = arr_bodies[i];

		if (body->mIsHighPriorityBody)
		{
			CollisionEngineSys_ApplyBodyVelocity(data, body, isVertical);

			CollisionEngineSys_ResolveWithOtherBodies(data, body, isVertical);
		}
	}

	for (int32_t i = 0; i < arrlen(arr_bodies); i += 1) // Move for the step, do initial collision check with baked collision.
	{
		Body* body = arr_bodies[i];

		if (!body->mIsHighPriorityBody)
		{
			CollisionEngineSys_ApplyBodyVelocity(data, body, isVertical);
		}

		CollisionEngineSys_ResolveWithBakedCollision(data, body, isVertical);
	}

	for (int32_t i = 0; i < arrlen(arr_bodies); i += 1) // After collision... now test normal thing rectangles.
	{
		CollisionEngineSys_ResolveWithOtherBodies(data, arr_bodies[i], isVertical);
	}
}
void CollisionEngineSys_ResolveWithBakedCollision(CollisionEngine* data, Body* body, bool vertical)
{
	if (body->mIgnoreAllCollision || body->mIsDisabled || body->mIgnoreBakedCollision || body->mCollideWithBulletsOnly || body->mIgnoreEverythingExceptPlayer || body->mIsStatic
		|| !IsBodyUnderResolveLimit(body))
	{
		return;
	}

	body->mInternalResolverCounter += 1;

	int32_t directionX = vertical ? 0 : body->mLastDirection.X;
	int32_t directionY = vertical ? body->mLastDirection.Y : 0;

	Rectangle rect = Body_GetRect(body);

	int32_t top = Rectangle_Top(&rect);
	int32_t right = Rectangle_Right(&rect);
	int32_t bottom = Rectangle_Bottom(&rect);
	int32_t left = Rectangle_Left(&rect);
	int32_t centerX = Rectangle_Center(&rect).X;
	int32_t centerY = Rectangle_Center(&rect).Y;

	if (CollisionEngineSys_CheckPoint(data, (float)centerX, (float)top, directionX, directionY, body, vertical)) // TOP
	{
		return;
	}
	else if (CollisionEngineSys_CheckPoint(data, (float)right, (float)top, directionX, directionY, body, vertical)) // TOP RIGHT
	{
		return;
	}
	else if (CollisionEngineSys_CheckPoint(data, (float)left, (float)top, directionX, directionY, body, vertical)) // TOP LEFT
	{
		return;
	}
	else if (CollisionEngineSys_CheckPoint(data, (float)right, (float)centerY, directionX, directionY, body, vertical)) // RIGHT
	{
		return;
	}
	else if (CollisionEngineSys_CheckPoint(data, (float)left, (float)centerY, directionX, directionY, body, vertical)) // LEFT
	{
		return;
	}
	else if (CollisionEngineSys_CheckPoint(data, (float)right, (float)bottom, directionX, directionY, body, vertical)) // BOTTOM RIGHT
	{
		return;
	}
	else if (CollisionEngineSys_CheckPoint(data, (float)left, (float)bottom, directionX, directionY, body, vertical)) // BOTTOM LEFT
	{
		return;
	}
	else if (CollisionEngineSys_CheckPoint(data, (float)centerX, (float)bottom, directionX, directionY, body, vertical)) // BOTTOM
	{
		return;
	}

	return;
}
void CollisionEngineSys_ApplyBodyVelocity(CollisionEngine* data, Body* body, bool isVertical)
{
	body->mInternalResolverCounter = 0;

	if (isVertical)
	{
		if (body->mPhysicsVelocity.Y != 0)
		{
			body->mPhysicsPosition.Y += body->mPhysicsVelocity.Y;
		}
	}
	else
	{
		if (body->mPhysicsVelocity.X != 0)
		{
			body->mPhysicsPosition.X += body->mPhysicsVelocity.X;
		}
	}
}
void CollisionEngineSys_DrawRoutine(Entity owner, CollisionEngine* data, SpriteBatch* spriteBatch)
{
	if (arrlen(arr_debug_many_rectangles) <= 0)
	{
		return;
	}

	SpriteBatch_DrawManyRectangle(spriteBatch, 100, arr_debug_many_rectangles);
}
void CollisionEngineSys_DrawTiles(GameState* scene)
{

}
void CollisionEngineSys_DebugGenerateDebugRectangles(CollisionEngine* data)
{
	if (!GLOBALS_DEBUG_SHOW_INGAME_COLLISION || Globals_DebugIsEditorMode())
	{
		return;
	}

	for (int32_t i = 0; i < arrlen(arr_bodies); i += 1)
	{
		Body* body = arr_bodies[i];
		Color drawColor = body->mTouchedBody ? mDebugColorActive : mDebugColorTouched;
		DrawRectangle drawRect;
		Rectangle bodyRect = Body_GetRect(body);
		DrawRectangle_Init(&drawRect, drawColor, bodyRect);
		arrput(arr_debug_many_rectangles, drawRect);
	}

	//for (int32_t i = 0; i < data->mDebugNodeRectangles.size(); i += 1)
	//{
		//mDebugManyRectangles.Add(mDebugNodeRectangles[i]); //Show node info
	//}

	//Draw Tiles
	LevelData* levelData = Get_LevelData();
	Camera* camera = Get_Camera();
	int32_t x1 = Camera_GetX1(camera);
	int32_t x2 = Camera_GetX2(camera, LevelData_GetGridSizeX(levelData));
	int32_t y1 = Camera_GetY1(camera);
	int32_t y2 = Camera_GetY2(camera, LevelData_GetGridSizeY(levelData));
	for (int32_t i = x1; i < x2; i += 1)
	{
		for (int32_t j = y1; j < y2; j += 1)
		{
			int32_t type = data->mCollisionGrid[GetTilePos1D(i, j, data)];
			if (type == OBJECTTYPES_NOTHING)
			{
				continue;
			}

			Color color = Utils_GetCollisionColor(type);

			int32_t rectX = i * TILE_SIZE;
			int32_t rectY = j * TILE_SIZE;
			int32_t rectWidth = TILE_SIZE;
			int32_t rectHeight = TILE_SIZE;

			int32_t halfTile = TILE_SIZE / 2;

			int32_t platformDown = CollisionEngineSys_GetPlatformDown();
			if ((type == CollisionEngineSys_GetPlatformUp()) || (type == platformDown))
			{
				rectHeight /= 2;
				if (type == platformDown)
				{
					rectY += halfTile;
				}
			}
			int32_t platformRight = CollisionEngineSys_GetPlatformRight();
			if ((type == CollisionEngineSys_GetPlatformLeft()) || (type == platformRight))
			{
				rectWidth /= 2;
				if (type == platformRight)
				{
					rectX += halfTile;
				}
			}

			Rectangle tempRectangle = { rectX, rectY, rectWidth, rectHeight };
			DrawRectangle tempDrawRectangle;
			DrawRectangle_Init(&tempDrawRectangle, color, tempRectangle);
			arrput(arr_debug_many_rectangles, tempDrawRectangle);
		}
	}
	//End Draw Tiles
}

System* CollisionEngineSys_CreateSystem(void)
{
	SystemSimple* ss = SystemSimple_Create(C_CollisionEngine);
	ss->_mUpdateRoutine = (SystemSimple_UpdateFunc)CollisionEngineSys_UpdateRoutine;
	ss->_mDrawRoutine = (SystemSimple_DrawFunc)CollisionEngineSys_DrawRoutine;
	return SystemSimple_CreateSystem(ss);
}