/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Func.h"

#include "../utils/Macros.h"
#include "../components/FreezeEntityTillOnScreen.h"
#include "../components/TagIsPlayer.h"
#include "../components/Camera.h"
#include "../components/SceneCameraData.h"
#include "../components/SceneMenuData.h"
#include "../components/StringSettings.h"
#include "../components/StepCounter.h"
#include "../components/FakePosition.h"
#include "../components/NodeMovingKit.h"
#include "../components/Move.h"
#include "../components/DrawActor.h"
#include "../components/IntTag.h"
#include "../components/SpecialShakeCameraSys.h"
#include "../components/PlayerNumber.h"
#include "../components/MoveGetter.h"
#include "../components/Name.h"
#include "../components/TagIsActor.h"
#include "../components/GridPosition.h"
#include "../components/InitialPosition.h"
#include "../components/HitFlashKit.h"
#include "../components/MirrorEffect.h"
#include "../components/StunFrames.h"
#include "../components/BounceData.h"
#include "../components/LevelFrameCounter.h"
#include "../components/TagIsDrawDisabled.h"
#include "../components/TagIsComplete.h"
#include "../components/TagIsEnemy.h"
#include "../components/Nodes.h"
#include "../components/TagIsExtraBody.h"
#include "../components/TagIsUpdateDisabled.h"
#include "../components/Step.h"
#include "../components/StepTimer.h"
#include "../components/FloatyMovementData.h"
#include "../components/TagIsBlockingLineOfSight.h"
#include "../components/IdleCircleData.h"
#include "../components/TagIsIgnoringTransitions.h"
#include "../components/DrawActorSys.h"
#include "../components/ParentNumber.h"
#include "../math/PointRectangle.h"
#include "../leveldata/LevelCameraDataInstance.h"
#include "../collision/CollisionEngineSys.h"
#include "../utils/Utils.h"
#include "../math/Math.h"
#include "../math/Random32.h"
#include "../math/Points.h"
#include "../math/Vectors.h"
#include "../utils/Cvars.h"
#include "../render/SpriteBatch.h"
#include "../render/DrawTool.h"
#include "../render/Animation.h"
#include "../leveldata/ParticleInstance.h"
#include "../render/Color.h"
#include "../globals/Globals.h"
#include "../input/Input.h"
#include "../input/InputPlayer.h"
#include "../audio/SoundEffect.h"
#include "../audio/Music.h"
#include "../gamesave/GameSaveManager.h"
#include "../core/GameHelper.h"
#include "../leveldata/LevelData.h"
#include "../globals/Directions.h"
#include "../leveldata/ThingSettings.h"
#include "../render/Renderer.h"
#include "../gamestate/GameStateManager.h"
#include "float.h"
#include "../utils/Logger.h"
#include "../../third_party/stb_ds.h"
#include "../gamestate/EntitySearch.h"
#include "../resources/ResourceManagerList.h"
//#include "../components/MoveGetterSys.h" //UNUSED
//#include "../utils/Tuning.h" //UNUSED

#define TILE_SIZE GLOBAL_DEF_TILE_SIZE
#define HALF_TILE_SIZE GLOBAL_DEF_HALF_TILE_SIZE

static int32_t _mAttackIdCounter;

static Point* Get_ArrGridNodes(Entity entity)
{
	bool wasSuccessful;
	Nodes* component = (Nodes*)TryGet_Component(C_Nodes, entity, &wasSuccessful);
	if (!wasSuccessful)
	{
		return NULL;
	}
	else
	{
		return component->arr_given_nodes;
	}
}

//REGION DO
Entity Do_CreatePlayer(float x, float y, const char* name)
{
	Entity player = Do_BuildActor2(x, y, NULL, name);
	int32_t playerNumber = Get_AmountOfPlayers();
	Do_SetPlayerNumber(player, playerNumber);
	Do_SetBoolTag(C_TagIsPlayer, player, true);
	return player;
}
int32_t Get_AmountOfPlayers(void)
{
	int32_t count = 0;
	EntitySearch* search = Get_EntitiesInPlay();
	for (int32_t i = 0; i < search->len; i += 1)
	{
		if (Get_PlayerNumber(search->entities[i]) != -1)
		{
			count += 1;
		}
	}
	return count;
}
bool Has_Players(void)
{
	if (Get_AmountOfPlayers() > 0)
	{
		return true;
	}
	return false;
}
Entity Get_Player(void)
{
	return Get_PlayerByNumber(0);
}
Entity Get_ClosestPlayer(float x, float y)
{
	return Get_PlayerByNumber(0);
}
Entity Get_PlayerByNumber(int32_t givenPlayerNumber)
{
	ComponentPack* pack = Get_ComponentPack(C_PlayerNumber);

	PackIterator iter = PackIterator_Begin;
	while (ComponentPack_Next(pack, &iter))
	{
		PlayerNumber* playerNumber = (PlayerNumber*)iter.mComponent;
		if (playerNumber->mTag == givenPlayerNumber)
		{
			return iter.mEntity;
		}
	}

	return ENTITY_NOTHING;
}
LevelCameraDataInstance* Get_CameraDataInstances(void)
{
	return Get_SceneCameraData()->mCameraData;
}
SceneCameraData* Get_SceneCameraData(void)
{
	return ((SceneCameraData*)Get_FirstSetComponent(C_SceneCameraData));
}

//private
void Do_HandleNormalCamera(bool useAutoHingeCamera, bool forceCameraPosition, LevelCameraDataInstance* instance)
{
	//UNUSED
}
void Do_HandleTransitionCamera(bool useAutoHingeCamera, bool forceCameraPosition, LevelCameraDataInstance* instance)
{
	//UNUSED
}
//
LevelCameraDataInstance* Do_HandleCamera(bool useAutoHingeCamera, bool forceCameraPosition)
{
	return Do_HandleCamera2(useAutoHingeCamera, forceCameraPosition, false, -1);
}
LevelCameraDataInstance* Do_HandleCamera2(bool useAutoHingeCamera, bool forceCameraPosition, bool manualSize, int32_t size)
{
	return NULL; //UNUSED
}
void Do_LoadCamera(bool useAutoHingeCamera, bool forceCameraPosition, LevelCameraDataInstance* instance)
{
	if (instance->mData.mIsTransition)
	{
		Do_HandleTransitionCamera(useAutoHingeCamera, forceCameraPosition, instance);
	}
	else
	{
		Do_HandleNormalCamera(useAutoHingeCamera, forceCameraPosition, instance);
	}
}
void Do_ResetCollisionGrid(void)
{
	if (!Is_ComponentPackPresent(C_CollisionEngine))
	{
		return;
	}

	CollisionEngineSys_SetupCollisionGrid(Get_CollisionEngine(), Get_LevelData());
}
void Do_LoadCameraNumber(bool useAutoHingeCamera, bool forceCameraPosition, int32_t number)
{
	/*
	std_vector<LevelCameraDataInstance>& cameras = Get_CameraDataInstances();
	for (int32_t i = 0; i < cameras.size(); i += 1)
	{
		LevelCameraDataInstance* instance = &cameras[i];
		if (instance->mData.mId == number)
		{
			Do_LoadCamera(useAutoHingeCamera, forceCameraPosition, instance);
			return;
		}
	}
	*/
}
void Do_ResetCameraDataFullyNoMatterWhat(void)
{
	/*
	std_vector<LevelCameraDataInstance>& array = Get_CameraDataInstances();
	for (int32_t i = 0; i < array.size(); i += 1)
	{
		array[i].ResetFullyNoMatterWhat();
	}
	*/
}
void Do_ResetCameraDataIfNotCompletePermanently(void)
{
	/*
	std_vector<LevelCameraDataInstance>& array = Get_CameraDataInstances();
	for (int32_t i = 0; i < array.size(); i += 1)
	{
		array[i].ResetIfNotCompletePermanently();
	}
	*/
}
void Do_SetCompleteCameraDataToPermanentlyComplete(void)
{
	/*
	std_vector<LevelCameraDataInstance>& array = Get_CameraDataInstances();
	for (int32_t i = 0; i < array.size(); i += 1)
	{
		LevelCameraDataInstance* data = &array[i];
		if (data->IsComplete())
		{
			data->SetCompletePermanently();
		}
	}
	*/
}
Vector2 Do_FindThingPositionInTileData(const char* name)
{
	LevelData* levelData = Get_LevelData();
	int32_t x2 = LevelData_GetGridSizeWidth(levelData);
	int32_t y2 = LevelData_GetGridSizeHeight(levelData);
	for (int32_t i = 0; i < x2; i += 1)
	{
		for (int32_t j = 0; j < y2; j += 1)
		{
			Tile* tile = LevelData_GetTile(levelData, i, j);
			ThingInstance* the_arr_instances = tile->arr_instances;
			for (int32_t k = 0; k < arrlen(the_arr_instances); k += 1)
			{
				ThingInstance* thingInstance = &the_arr_instances[k];
				if (thingInstance == NULL)
				{
					continue;
				}

				const char* thingInstanceName = ThingInstance_GetName(thingInstance);
				if (Utils_StringEqualTo(thingInstanceName, name))
				{
					return Vector2_Create((float)((i * TILE_SIZE) + HALF_TILE_SIZE), (float)((j * TILE_SIZE) + HALF_TILE_SIZE));
				}
			}
		}
	}

	return Vectors_NegativeOne;
}
void Do_InitAllPermanentThings(void)
{
	LevelData* levelData = Get_LevelData();
	Do_InitPermanentThingsByGridCoordinates(0, 0, LevelData_GetGridSizeWidth(levelData), LevelData_GetGridSizeHeight(levelData));
}
void Do_InitPermanentThingsByRealCoordinates(float realX1, float realY1, float realX2, float realY2, bool startThings)
{
	int32_t x1 = (int32_t)(realX1 / TILE_SIZE);
	int32_t y1 = (int32_t)(realY1 / TILE_SIZE);
	int32_t x2 = (int32_t)(realX2 / TILE_SIZE);
	int32_t y2 = (int32_t)(realY2 / TILE_SIZE);
	Do_InitPermanentThingsByGridCoordinates(x1, y1, x2, y2);
}
void Do_InitPermanentThingsByRoomBounds(LevelCameraDataInstance* instance, bool startThings)
{
	PointRectangle* pointRect = LevelCameraData_GetActiveBoundsPointRectangle(&instance->mData);
	Do_InitPermanentThingsByRealCoordinates((float)pointRect->mPointOne.X, (float)pointRect->mPointOne.Y, 
		(float)pointRect->mPointTwo.X, (float)pointRect->mPointTwo.Y, startThings);
}
void Do_InitPermanentThingsByGridCoordinates(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
	LevelData* levelData = Get_LevelData();
	for (int32_t i = x1; i < x2; i += 1)
	{
		for (int32_t j = y1; j < y2; j += 1)
		{
			ThingInstance* arr_instances = LevelData_GetTile(levelData, i, j)->arr_instances;
			for (int32_t k = 0; k < arrlen(arr_instances); k += 1)
			{
				Do_BuildThingFromData(i, j, &arr_instances[k]);
			}
		}
	}
}
void Do_InitPermanentThingsByGridCoordinates2(int32_t x, int32_t y)
{
	//Do_InitPermanentThingsByGridCoordinates(x, y, STR_NOTHING);
}
void Do_InitPermanentThingsByGridCoordinates3(int32_t x, int32_t y, const char* onlyWithThisName)
{
	/*
	OeLevelData* levelData = Get_LevelData();
	std_vector<ThingInstance>& dataArray = levelData->GetTile(x, y)->mInstances;
	for (int32_t k = 0; k < dataArray.size(); k += 1)
	{
		ThingInstance* instance = &dataArray[k];
		if (Utils_StringEqualTo(onlyWithThisName, STR_NOTHING))
		{
			Do_BuildThingFromData(x, y, instance);
		}
		else
		{
			if (onlyWithThisName == instance->mName)
			{
				Do_BuildThingFromData(x, y, instance);
			}
		}
	}
	*/
}

//private
Entity Do_BuildThingFromData(int32_t i, int32_t j, ThingInstance* data)
{
	if (data == NULL)
	{
		return ENTITY_NOTHING;
	}

	Point buildOffset = Point_Zero;
	bool buildThing = true;

	//OFFSET
	{
		StringPair pairOffsetX = ThingInstance_GetSetting(data, THINGINSTANCE_SETTING_DTN_OFFSET_X);
		if ((pairOffsetX.mKey != NULL) && !Utils_StringEqualTo(pairOffsetX.mKey, EE_STR_EMPTY))
		{
			buildOffset.X = Utils_ParseDirection(pairOffsetX.mValue) * HALF_TILE_SIZE;
			StringPair pairOffsetY = ThingInstance_GetSetting(data, THINGINSTANCE_SETTING_DTN_OFFSET_Y);
			if ((pairOffsetY.mKey != NULL) && !Utils_StringEqualTo(pairOffsetY.mKey, EE_STR_EMPTY))
			{
				buildOffset.Y = Utils_ParseDirection(pairOffsetY.mValue) * HALF_TILE_SIZE;
			}
		}
	}

	//DIFFICULTY
	{
		/*StringPair pairDifficulty = StringPair_Empty; //UNUSED
		int32_t currentDifficulty = Tuning_GetCurrentDifficulty()
		switch (currentDifficulty)
		{
		case 0:
			pairDifficulty = ThingInstance_GetSetting(data, THINGINSTANCE_SETTING_BLN_DIFFICULTY_EASY);
			break;
		case 1:
			pairDifficulty = ThingInstance_GetSetting(data, THINGINSTANCE_SETTING_BLN_DIFFICULTY_NORMAL);
			break;
		case 2:
			pairDifficulty = ThingInstance_GetSetting(data, THINGINSTANCE_SETTING_BLN_DIFFICULTY_HARD);
			break;
		case 3:
			pairDifficulty = ThingInstance_GetSetting(data, THINGINSTANCE_SETTING_BLN_DIFFICULTY_VERY_HARD);
			break;
		}
		if (!Utils_StringEqualTo(pairDifficulty.mKey, EE_STR_EMPTY) && !Utils_StringEqualTo(pairDifficulty.mKey, EE_STR_NOT_SET))
		{
			buildThing = Utils_ParseBooleanFromChar(pairDifficulty.mValue);
		}*/
	}

	buildOffset.X += HALF_TILE_SIZE;
	buildOffset.Y += HALF_TILE_SIZE;
	if (buildThing)
	{
		int32_t initialPositionX = (i * TILE_SIZE) + buildOffset.X;
		int32_t initialPositionY = (j * TILE_SIZE) + buildOffset.Y;
		const char* thingName = ThingInstance_GetName(data);
		return Do_BuildActor5(i, j, (float)initialPositionX, (float)initialPositionY, data, thingName);
	}
	else
	{
		return ENTITY_NOTHING;
	}
}
//
Entity Do_BuildNewEntity(void)
{
	return GameState_BuildNewEntity(Get_ActiveGameState());
}
Entity Do_BuildNewEntityWithName(const char* name)
{
	Entity temp = Do_BuildNewEntity();
	Do_SetName(temp, name);
	return temp;
}
void Do_FreezeEntityTillOnScreen(Entity entity)
{
	Do_FreezeEntityTillOnScreen2(entity, false, false);
}
void Do_FreezeEntityTillOnScreen2(Entity entity, bool facesPlayer, bool isFacingFlipped)
{
	Do_SetBodyDisabled(entity, true);
	Do_SetUpdateDisabled(entity, true);
	FreezeEntityTillOnScreen* data = (FreezeEntityTillOnScreen*)Get_Component(C_FreezeEntityTillOnScreen, entity);
	data->mRectangle = Body_GetRect(Get_Body(entity));
	data->mFacesPlayer = facesPlayer;
	data->mIsFacingFlipped = isFacingFlipped;
}
void Do_TurnOnMirrorEffect(Entity entity)
{
	MirrorEffect* mirrorEffect = (MirrorEffect*)Get_Component(C_MirrorEffect, entity);
	MirrorEffect_SetOn(mirrorEffect);
}
void Do_TurnOffMirrorEffect(Entity entity)
{
	MirrorEffect* mirrorEffect = (MirrorEffect*)Get_Component(C_MirrorEffect, entity);
	MirrorEffect_SetOff(mirrorEffect);
}
void Do_ActivateHitFlashKit(Entity entity)
{
	((HitFlashKit*)Get_Component(C_HitFlashKit, entity))->mShowHitFlash = true;
}
void Do_SetBodyMaxDecelerationToInfinite(Entity entity)
{
	Do_SetBodyMaxDeceleration(entity, FLT_MAX);
}
void Do_SetBodyMaxDeceleration(Entity entity, float value)
{
	Get_Body(entity)->mMaxDeceleration = value;
}
int32_t Get_NextBossMove(Entity entity)
{
	return 0;/*
	MoveGetter* getter = Get_Component(C_MoveGetter, entity);
	return MoveGetterSys_GetMove(getter, Get_Random(entity));*/
}
int32_t Get_IntTag(Entity entity)
{
	bool wasSuccessful;
	IntTag* component = (IntTag*)TryGet_Component(C_IntTag, entity, &wasSuccessful);
	if (!wasSuccessful)
	{
		return -1;
	}
	else
	{
		return component->mTag;
	}
}
void Do_SetIntTag(Entity entity, int32_t value)
{
	IntTag* component = (IntTag*)Get_Component(C_IntTag, entity);
	component->mTag = value;
}
int32_t Get_StepCounter(Entity entity)
{
	StepCounter* component = (StepCounter*)Get_Component(C_StepCounter, entity);
	return component->mCounter;
}
void Do_IncrementStepCounter(Entity entity)
{
	StepCounter* component = (StepCounter*)Get_Component(C_StepCounter, entity);
	component->mCounter += 1;
}
void Do_ResetStepCounter(Entity entity)
{
	StepCounter* component = (StepCounter*)Get_Component(C_StepCounter, entity);
	component->mCounter = 0;
}
void Do_SetMove(Entity entity, int32_t value)
{
	Move* component = (Move*)Get_Component(C_Move, entity);
	component->mMove = value;
	Do_SetStep(entity, 0);
}
int32_t Get_Move(Entity entity)
{
	Move* component = (Move*)Get_Component(C_Move, entity);
	return component->mMove;
}
void Do_SetStep(Entity entity, int32_t value)
{
	Step* component = (Step*)Get_Component(C_Step, entity);
	component->mStep = value;
	Do_ResetStepCounter(entity);
	Do_ResetStepTimer(entity);
}
int32_t Get_Step(Entity entity)
{
	Step* component = (Step*)Get_Component(C_Step, entity);
	return component->mStep;
}
int32_t Get_AmountOfBounces(Entity entity)
{
	BounceData* data = (BounceData*)Get_Component(C_BounceData, entity);
	return data->mBounces;
}
bool Do_Bounce(Entity entity, float dampener, float jumpSpeed, float velocityIncrease, bool fallFirstBounce)
{
	BounceData* data = (BounceData*)Get_Component(C_BounceData, entity);
	bool returnValue = false;
	if (data->mIsNotFirstFrame)
	{
		if (Is_TouchingCollision(entity) && Is_TouchingDown(entity))
		{
			data->mBounces += 1;
			data->mVelocity = 0;
			returnValue = true;
		}
	}
	else
	{
		if (fallFirstBounce)
		{
			data->mVelocity = jumpSpeed;
		}
		data->mIsNotFirstFrame = true;
	}
	for (int32_t i = 0; i < data->mBounces; i += 1)
	{
		jumpSpeed *= dampener;
	}
	Do_MoveAbsolute(entity, 0, -jumpSpeed + data->mVelocity);
	data->mVelocity += velocityIncrease;
	data->mWasUsed = true;
	return returnValue;
}
void Do_ResetStepTimer(Entity entity)
{
	StepTimer* data = (StepTimer*)Get_Component(C_StepTimer, entity);
	data->mTimer = Timer_Zero;
}
Timer* Get_StepTimer(Entity entity)
{
	StepTimer* data = (StepTimer*)Get_Component(C_StepTimer, entity);
	return &data->mTimer;
}
bool Do_UpdateStepTimer(Entity entity, int32_t limit)
{
	StepTimer* data = (StepTimer*)Get_Component(C_StepTimer, entity);
	data->mTimer.mLimit = limit;
	return Timer_Update(&data->mTimer);
}
void Do_SetBodyFramesInAir(Entity entity, int32_t value)
{
	Get_Body(entity)->mFramesInAir = value;
}
void Do_SetBodyIgnoreGravityY(Entity entity, bool value)
{
	Get_Body(entity)->mIgnoreGravityY = value;
}
void Do_SetBodyIsOneWayPlatform(Entity entity, bool value)
{
	Get_Body(entity)->mIsOneWayPlatform = value;
}
void Do_SetBodyIsMovingPlatform(Entity entity, bool value)
{
	Get_Body(entity)->mIsMovingPlatform = value;
}
void Do_SetBodyCannotBePushed(Entity entity, bool value)
{
	Get_Body(entity)->mCannotBePushed = value;
}
void Do_SetBodyPushes(Entity entity, bool value)
{
	Get_Body(entity)->mPushes = value;
}
void Do_SetBodyCollideWithBody(Entity entity, void (*colliderBody)(Body* myBody, Body* otherBody, int32_t myDirectionX, int32_t myDirectionY, int32_t otherDirectionX, int32_t otherDirectionY, bool isVertical))
{
	Get_Body(entity)->mCollideWithBody = colliderBody;
}
void Do_SetBodyCollideWithCollision(Entity entity, bool (*colliderCollision)(Body* myBody, int32_t x, int32_t y, int32_t width, int32_t height, int32_t myDirectionX, int32_t myDirectionY, int32_t collisionBit, bool isVertical))
{
	Get_Body(entity)->mCollideWithCollision = colliderCollision;
}
void Do_SetBodyIsBullet(Entity entity, bool value)
{
	Get_Body(entity)->mIsBullet = value;
}
void Do_SetBodyIgnoreEverythingExceptPlayer(Entity entity, bool value)
{
	Get_Body(entity)->mIgnoreEverythingExceptPlayer = value;
}
void Do_SetBodyIgnoreBullets(Entity entity, bool value)
{
	Get_Body(entity)->mIgnoreBullets = value;
}
void Do_SetBodyIgnoreBakedCollision(Entity entity, bool value)
{
	Get_Body(entity)->mIgnoreBakedCollision = value;
}
void Do_SetBodyType(Entity entity, int32_t type)
{
	Get_Body(entity)->mType = type;
}
void Do_SetBodyIgnoreAllCollision(Entity entity, bool value)
{
	Get_Body(entity)->mIgnoreAllCollision = value;
}
void Do_SetBodyStatic(Entity entity, bool value)
{
	Get_Body(entity)->mIsStatic = value;
}
void Do_SetBodyDisabled(Entity entity, bool value)
{
	Get_Body(entity)->mIsDisabled = value;
}
EntitySearch* Do_SearchForEntitiesWithName(const char* name)
{
	return Do_SearchForEntitiesWithName2(name, false);
}
EntitySearch* Do_SearchForEntitiesWithName2(const char* name, bool isReverse)
{
	return EntitySearch_SearchForEntitiesWithThisName(EntitySearch_CreateNewAllEntitySearch(), EntitySearch_CreateNewBlankSearch(), name, isReverse);
}
EntitySearch* Do_SearchForEntitiesWithIntTag(int32_t value)
{
	return Do_SearchForEntitiesWithIntTag2(value, false);
}
EntitySearch* Do_SearchForEntitiesWithIntTag2(int32_t value, bool isReverse)
{
	return EntitySearch_SearchForEntitiesWithThisIntTag(EntitySearch_CreateNewAllEntitySearch(), EntitySearch_CreateNewBlankSearch(), value, isReverse);
}
EntitySearch* Do_SearchForChildren(Entity entity)
{
	return Do_SearchForChildren2(entity, false);
}
EntitySearch* Do_SearchForChildren2(Entity entity, bool isReverse)
{
	return EntitySearch_SearchForEntitiesWithThisParentNumber(EntitySearch_CreateNewAllEntitySearch(), 
		EntitySearch_CreateNewBlankSearch(), Get_EntityNumber(entity), isReverse);
}
EntitySearch* Do_SearchForChildrenWithName(Entity entity, const char* name)
{
	return Do_SearchForChildrenWithName2(entity, name, false);
}
EntitySearch* Do_SearchForChildrenWithName2(Entity entity, const char* name, bool isReverse)
{
	return EntitySearch_SearchForEntitiesWithThisName(Do_SearchForChildren2(entity, isReverse), EntitySearch_CreateNewBlankSearch(), name, isReverse);
}
EntitySearch* Do_SearchForChildrenWithIntTag(Entity entity, int32_t value)
{
	return Do_SearchForChildrenWithIntTag2(entity, value, false);
}
EntitySearch* Do_SearchForChildrenWithIntTag2(Entity entity, int32_t value, bool isReverse)
{
	return EntitySearch_SearchForEntitiesWithThisIntTag(Do_SearchForChildren2(entity, isReverse), EntitySearch_CreateNewBlankSearch(), value, isReverse);
}
void Do_SetIsBlockingLineOfSight(Entity entity, bool value)
{
	Do_SetBoolTag(C_TagIsBlockingLineOfSight, entity, true);
}
void Do_DestroyAllEnemies(void)
{
	EntitySearch* search = Do_SearchForEntitiesWithComponent(C_TagIsEnemy);
	for (int32_t i = 0; i < search->len; i += 1)
	{
		Do_SetComplete(search->entities[i]);
	}
}
MoveGetter* Do_InitMoveGetter(Entity entity, const char* name, int32_t movesToRemember)
{
	MoveGetter* moveGetter = (MoveGetter*)Do_InitComponent(C_MoveGetter, entity);
	//moveGetter->Setup(name, movesToRemember); //UNUSED
	return moveGetter;
}
NodeMovingKit* Do_InitNodeMovingKit(Entity entity, int32_t type, bool moveContacts)
{
	NodeMovingKit* kit = (NodeMovingKit*)Do_InitComponent(C_NodeMovingKit, entity);
	//kit->Setup(entity, type, moveContacts); //UNUSED
	return kit;
}
HitFlashKit* Do_InitHitFlashKit(Entity entity, int32_t time)
{
	HitFlashKit* kit = (HitFlashKit*)Do_InitComponent(C_HitFlashKit, entity);
	//kit->Setup(time); //UNUSED
	return kit;
}
MirrorEffect* Do_InitMirrorEffect(Entity entity, Color color, int32_t depth)
{
	MirrorEffect* effect = (MirrorEffect*)Do_InitComponent(C_MirrorEffect, entity);
	effect->mColor = color;
	effect->mDepth = depth;
	return effect;
}
void Do_SetRotate(Entity entity, float value)
{
	Get_DrawActor(entity)->mRotation = value;
}
void Do_Rotate(Entity entity, float value)
{
	Get_DrawActor(entity)->mRotation += value;
}
void Do_IdleCircle(Entity entity, float radius, bool useX, bool useY, int32_t idleDegreeIncrease)
{
	IdleCircleData* data = (IdleCircleData*)Get_Component(C_IdleCircleData, entity);
	Do_MoveCircle2(entity, (float)data->mIdleDegree, 0, radius, useX, useY);
	data->mIdleDegree += idleDegreeIncrease;
}
void Do_SetAsUsingBulletCollisionEngine(Entity entity)
{
	//WILLNOTDO20240323//entity._mUsesBulletCollisionEngine = true;
}
bool Do_FloatyMoveSomewhere(Entity entity, float destX, float destY, float speedAccel, float speedLimit)
{
	return Do_FloatyMoveSomewhere2(entity, destX, destY, speedAccel, speedLimit, false);
}
bool Do_FloatyMoveSomewhere2(Entity entity, float destX, float destY, float speedAccel, float speedLimit, bool doNotResetMovementAtEnd)
{
	FloatyMovementData* data = (FloatyMovementData*)Get_Component(C_FloatyMovementData, entity);
	data->mWasUsed = true;
	float distanceToTarget = (float)Get_Distance2(entity, destX, destY);
	float timeToTarget = 0;
	if (data->mCurrentSpeed != 0)
	{
		timeToTarget = distanceToTarget / data->mCurrentSpeed;
	}
	float timeToDecel = data->mCurrentSpeed / speedAccel;
	if (timeToDecel > timeToTarget)
	{
		data->mCurrentSpeed -= speedAccel;
	}
	else
	{
		data->mCurrentSpeed += speedAccel;
	}
	data->mCurrentSpeed = Math_MinFloat(data->mCurrentSpeed, speedLimit);
	data->mCurrentSpeed = Math_MaxFloat(data->mCurrentSpeed, speedAccel);
	if (Do_MoveSomewhere(entity, destX, destY, data->mCurrentSpeed))
	{
		if (!doNotResetMovementAtEnd)
		{
			data->mCurrentSpeed = 0;
		}
		return true;
	}
	return false;
}
bool Do_FloatyMoveSomewhere3(float* currentSpeed, Vector2* moveThis, Vector2 destination, float speedAccel, float speedLimit, bool doNotResetMovementAtEnd)
{
	return false;
	/*
	float distanceToTarget = (float)(Get_Distance(*moveThis, destination));
	float timeToTarget = 0;
	if (*currentSpeed != 0)
	{
		timeToTarget = distanceToTarget / *currentSpeed;
	}
	float timeToDecel = *currentSpeed / speedAccel;
	if (timeToDecel > timeToTarget)
	{
		*currentSpeed -= speedAccel;
	}
	else
	{
		*currentSpeed += speedAccel;
	}
	*currentSpeed = Math_Min(*currentSpeed, speedLimit);
	*currentSpeed = Math_Max(*currentSpeed, speedAccel);
	if (Do_MoveSomewhereVector2(moveThis, destination, *currentSpeed))
	{
		if (!doNotResetMovementAtEnd)
		{
			*currentSpeed = 0;
		}
		return true;
	}
	return false;
	*/
}
void Do_FloatyMove(Entity entity, float directionX, float directionY, float speedAccel, float speedLimit)
{
	FloatyMovementData* data = (FloatyMovementData*)Get_Component(C_FloatyMovementData, entity);
	data->mWasUsed = true;
	data->mCurrentSpeed += speedAccel;
	data->mCurrentSpeed = Math_MinFloat(data->mCurrentSpeed, speedLimit);
	Do_Move(entity, directionX, directionY, data->mCurrentSpeed);
}
void Do_InitRandom(Entity entity)
{
	Do_InitComponent(C_Random32, entity);
}
void Do_InitBossRandom(Entity entity)
{
	Do_InitRandom(entity);
	int32_t frameCount = Get_LevelFrameCount();
	int32_t seed;
	if (frameCount == 0)
	{
		seed = GameStateManager_GetUniqueMapSeed();
	}
	else
	{
		seed = frameCount;
	}
	Random32_SetSeed(Get_Random(entity), (uint32_t)(seed));
}
void Do_SetScale(Entity entity, float x, float y)
{
	DrawActor* drawActor = Get_DrawActor(entity);
	drawActor->mScale.X = x;
	drawActor->mScale.Y = y;
}
bool Do_DestroyIfUnderCameraHinge(Entity entity)
{
	if (Is_UnderCameraHinge(entity))
	{
		Do_SetComplete(entity);
		return true;
	}
	return false;
}
bool Do_DestroyIfOffScreen(Entity entity)
{
	if (!Is_OnScreen(entity))
	{
		Do_SetComplete(entity);
		return true;
	}
	return false;
}
void Do_SetIgnoringTransitions(Entity entity, bool value)
{
	Do_SetBoolTag(C_TagIsIgnoringTransitions, entity, value);
}
void Do_DrawBodyRectangle(Entity entity, SpriteBatch* spriteBatch, double delta, int32_t depth, Color color)
{
	Body_DrawBody(Get_Body(entity), spriteBatch, delta, depth, color);
}
void Do_SetDepthOverride(Entity entity, int32_t value)
{
	Get_DrawActor(entity)->mUniversalDepthOverride = value;
}
void Do_ResetDepthOverride(Entity entity)
{
	Do_SetDepthOverride(entity, -1);
}
void Do_SetDepthOverride2(Entity entity, int32_t state, int32_t value)
{
	DrawActorSys_SetDepthOverride(entity, state, value);
}
void Do_ResetDepthOverride2(Entity entity, int32_t state)
{
	Do_SetDepthOverride2(entity, state, -1);
}
void Do_SetDrawExtraPasses(Entity entity, int32_t extra)
{
	Get_DrawActor(entity)->mExtraPasses = extra;
}
void Do_SetBlendStateAdditive(Entity entity)
{
	Get_DrawActor(entity)->mIsBlendStateAdditive = true;
}
void Do_SetBlendStateNormal(Entity entity)
{
	Get_DrawActor(entity)->mIsBlendStateAdditive = false;
}
void Do_DrawFullscreenRectangle(SpriteBatch* spriteBatch, Color color)
{
	/*
	DrawTool_DrawRectangle(spriteBatch, color, 100,
		Rectangle(0, 0, Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_WIDTH), Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_HEIGHT)),
		0, false);
		*/
}
void Do_SetAnimationTimeLimit(Entity entity, int32_t state, int32_t phase, int32_t time)
{
	Get_Animation(entity, state, phase)->mFlipTimer.mLimit = time;
}
void Do_SetShader(Entity entity, ShaderProgram* program)
{
	Get_DrawActor(entity)->mShaderProgram = program;
}
void Do_SetShader2(Entity entity, int32_t state, ShaderProgram* program)
{
	DrawActorSys_SetShaderProgram(entity, state, program);
}
void Do_SetShaderAsWhiteFlash(Entity entity)
{
	Do_SetShader(entity, ShaderProgram_GetShaderWhiteHitFlash());
}
void Do_SetupShaderMultiColorLength(ShaderProgram* program, int32_t length)
{
	program->mShaderType = RENDERER_SHADER_TYPE_MULTI_COLOR_REPLACE;
	program->mMultiColorReplaceLength = length;
	Utils_memset(program->mMultiColorReplace, 0, sizeof(uint8_t) * SHADER_PROGRAM_MAX_REPLACE_LENGTH);
}
ShaderProgram* Do_CreateShader(const char* name)
{
	return NULL;
	//return ShaderProgram_LoadShader(name);
}
void Do_SetupShaderMultiColor(ShaderProgram* program, int32_t i, uint8_t targetR, uint8_t targetG, uint8_t targetB,
	uint8_t replaceR, uint8_t replaceG, uint8_t replaceB)
{
	Do_SetupShaderMultiColor2(program, i, targetR, targetG, targetB, replaceR, replaceG, replaceB, 255);
}
void Do_SetupShaderMultiColor2(ShaderProgram* program, int32_t i, uint8_t targetR, uint8_t targetG, uint8_t targetB,
	uint8_t replaceR, uint8_t replaceG, uint8_t replaceB, uint8_t alpha)
{
	program->mShaderType = RENDERER_SHADER_TYPE_MULTI_COLOR_REPLACE;
	int32_t off = i * SHADER_PROGRAM_MAX_REPLACE_ENTRY_OFFSET;
	program->mMultiColorReplace[off] = targetR;
	program->mMultiColorReplace[off + 1] = targetG;
	program->mMultiColorReplace[off + 2] = targetB;
	program->mMultiColorReplace[off + 3] = replaceR;
	program->mMultiColorReplace[off + 4] = replaceG;
	program->mMultiColorReplace[off + 5] = replaceB;
	program->mMultiColorReplaceAlpha = alpha;
}
Body* Do_InitBody(Entity entity, int32_t width, int32_t height)
{
	Body* body = (Body*)Do_InitComponent(C_Body, entity);
	Body_Setup(body, width, height, false);
	Body_ForcePosition(body, Get_InitialX(entity), Get_InitialY(entity));
	return body;
}
Body* Do_InitExtraBody(Entity entity, int32_t width, int32_t height, bool isFollower)
{
	Entity newBodyEntity = Do_BuildNewEntity();
	Do_SetParentNumber(newBodyEntity, entity);
	Do_SetBoolTag(C_TagIsExtraBody, newBodyEntity, true);
	Do_SetName(newBodyEntity, "Extra Body");
	Body* body = Do_InitBody(newBodyEntity, width, height);
	Body_ForcePosition(body, Get_InitialX(entity), Get_InitialY(entity));
	body->mIsExtraBody = true;
	body->mIsExtraBodyThatFollows = isFollower;
	return body;
}
void Do_SetDrawDisabled(Entity entity, bool value)
{
	Do_SetBoolTag(C_TagIsDrawDisabled, entity, value);
}
void Do_SetDrawActorUpdateDisabled(Entity entity, bool value)
{
	Get_DrawActor(entity)->mIsUpdateDisabled = value;
}
void Do_TagAsEnemy(Entity entity)
{
	Do_SetBoolTag(C_TagIsEnemy, entity, true);
}
void Do_SetUpdateDisabled(Entity entity, bool value)
{
	Do_SetBoolTag(C_TagIsUpdateDisabled, entity, value);
}
void Do_SetUpdateAndDrawDisabled(Entity entity, bool value)
{
	Do_SetUpdateDisabled(entity, value);
	Do_SetDrawDisabled(entity, value);
}
void Do_SetStunFrames(Entity entity, int32_t value)
{
	StunFrames* component = (StunFrames*)Get_Component(C_StunFrames, entity);
	component->mTag = value;
}
void Do_SetStateRotation(Entity entity, int32_t state, float rotation)
{
	DrawActorSys_SetStateRotation(entity, state, rotation);
}
DrawActor* Get_DrawActor(Entity entity)
{
	return (DrawActor*)Get_Component(C_DrawActor, entity);
}

//private
void Do_SetArrNodes(Entity entity, Point* arr_nodes)
{
	((Nodes*)Get_Component(C_Nodes, entity))->arr_given_nodes = arr_nodes;
}
void Do_SetArrStringSettings(Entity entity, StringPair* arr_settings)
{
	((StringSettings*)Get_Component(C_StringSettings, entity))->arr_given_settings = arr_settings;
}
void Do_SetComplete(Entity entity)
{
	Do_SetBoolTag(C_TagIsComplete, entity, true);
}
void Do_SetGridPosition(Entity entity, int32_t x, int32_t y)
{
	GridPosition* gridPosition = (GridPosition*)Get_Component(C_GridPosition, entity);
	gridPosition->mGridPosition.X = x;
	gridPosition->mGridPosition.Y = y;
}
void Do_SetInitialPosition(Entity entity, float x, float y)
{
	InitialPosition* initialPosition = (InitialPosition*)Get_Component(C_InitialPosition, entity);
	initialPosition->mInitialPosition.X = x;
	initialPosition->mInitialPosition.Y = y;
}
void Do_SetPosition(Entity entity, float x, float y)
{
	FakePosition* position = (FakePosition*)Get_Component(C_FakePosition, entity);
	position->mFakePosition.X = x;
	position->mFakePosition.Y = y;
}
void Do_SetName(Entity entity, const char* name)
{
	Name* component = (Name*)Get_Component(C_Name, entity);
	Utils_strlcpy(component->mName, name, EE_FILENAME_MAX);
}
void Do_SetDrawOffset(Entity entity, int32_t x, int32_t y)
{
	DrawActor* drawActor = Get_DrawActor(entity);
	drawActor->mOffset.X = (float)x;
	drawActor->mOffset.Y = (float)y;
}
void Do_DrawLineOfSight(Entity entity, SpriteBatch* spriteBatch, float offsetX, float offsetY, bool limit, int32_t limitAngle)
{
	if (!GLOBALS_DEBUG_SHOW_INGAME_COLLISION)
	{
		return;
	}

	if (limit)
	{
		if (!Is_InLineOfSightWithPlayer2(entity, offsetX, offsetY, limitAngle))
		{
			return;
		}
	}

	Vector2 position = Get_Position(entity);
	CollisionEngineSys_HasLineOfSight2(Get_CollisionEngine(), true, spriteBatch, position.X + offsetX, position.Y + offsetY, position.X, position.Y, true);
}
void Do_MoveCircle(Entity entity, float degree, float degreeOffset, float radius)
{
	Do_MoveCircle2(entity, degree, degreeOffset, radius, true, true);
}
void Do_MoveCircle2(Entity entity, float degree, float degreeOffset, float radius, bool useX, bool useY)
{
	Do_MoveCircle3(entity, degree, degreeOffset, radius, useX, useY, 1, 1, 1, 1);
}
void Do_MoveCircle3(Entity entity, float degree, float degreeOffset, float radius, bool useX, bool useY, int32_t directionX, int32_t directionY, float mulX, float mulY)
{
	float x = (float)Get_CurrentCircleX(degree, degreeOffset, radius);
	float y = (float)Get_CurrentCircleY(degree, degreeOffset, radius);

	Body_Move(Get_Body(entity), useX ? x * directionX * mulX : 0, useY ? y * directionY * mulY : 0);
}
bool Do_MoveCircleAroundPosition(Entity entity, float degree, float degreeOffset, float radius, Vector2 position, float speed)
{
	Vector2 dest = Get_MoveCircleAroundPositionDestination(degree, degreeOffset, radius, position);
	return Do_MoveSomewhereVector2(entity, dest, speed);
}
Vector2 Get_MoveCircleAroundPositionDestination(float degree, float degreeOffset, float radius, Vector2 position)
{
	Vector2 circle = { (float)Get_CurrentCircleX(degree, degreeOffset, radius), (float)Get_CurrentCircleY(degree, degreeOffset, radius) };
	return Vector2_Add(position, circle);
}
void Do_DefaultQuickDebugCamera(Entity entity)
{
	//WILLNOTDO, EDITOR_MODE STUFF
	/*
	Vector2 debug = Globals_DEBUG_QUICK_PLAYER_POSITION;
	if (debug.X != -1 && debug.Y != -1)
	{
		Body_ForcePosition(Get_Body(entity), debug.X, debug.Y);
		ref Camera camera = ref Get_Camera();
		Camera_SetPosition(ref camera, debug.X, debug.Y);
		Camera_SetHingeDualDirectionX(ref camera, true);
		Camera_SetHingeDualDirectionY(ref camera, true);
	}
	*/
}
void Do_LoopSound(const char* sound, int32_t number)
{
	SoundEffect_LoopSound(sound, number);
}
void Do_SynchronizeAnimation(Entity entity, int32_t state, int32_t phase)
{
	Animation* anim = Get_Animation(entity, state, phase);
	Do_SynchronizeAnimation2(anim);
}
void Do_SynchronizeAnimation2(Animation* anim)
{
	int32_t timeLimit = anim->mFlipTimer.mLimit;
	int32_t lim = anim->mFrameTimer.mLimit * timeLimit;
	int32_t current = Get_LevelFrameCount() % lim;
	int32_t frame = current / timeLimit;
	int32_t time = current % timeLimit;
	anim->mFrameTimer.mCurrent = frame;
	anim->mFlipTimer.mCurrent = time;
}
void Do_Vibrate(int32_t player, int32_t priority, int32_t frames, float leftMotor, float rightMotor)
{
	InputPlayer_Vibrate(Input_GetPlayer(player), priority, frames, leftMotor, rightMotor);
}
void Do_Vibrate2(Entity entity, int32_t priority, int32_t frames, float leftMotor, float rightMotor)
{
	Do_Vibrate(Get_PlayerNumber(entity), priority, frames, leftMotor, rightMotor);
}
void Do_RandomShake(Entity entity, int32_t state, int32_t amount)
{
	int32_t amountX = Get_RandomBinaryDirection(Globals_GetSharedRandom()) * amount;
	int32_t amountY = Get_RandomBinaryDirection(Globals_GetSharedRandom()) * amount;
	Do_Shake(entity, state, amountX, amountY);
}
void Do_RandomShake2(Entity entity, int32_t amount)
{
	int32_t nudgeX = Get_RandomBinaryDirection(Globals_GetSharedRandom()) * amount;
	int32_t nudgeY = Get_RandomBinaryDirection(Globals_GetSharedRandom()) * amount;
	Vector2* universalNudge = &Get_DrawActor(entity)->mUniversalNudge;
	universalNudge->X = (float)nudgeX;
	universalNudge->Y = (float)nudgeY;
}
void Do_Shake(Entity entity, int32_t state, int32_t amountX, int32_t amountY)
{
	DrawActorSys_SetNudge(entity, state, (float)amountX, (float)amountY);
}
void Do_ClearShake(Entity entity)
{
	Get_DrawActor(entity)->mUniversalNudge = Vector2_Zero;
}
void Do_ClearShake2(Entity entity, int32_t state)
{
	Do_Shake(entity, state, 0, 0);
}
ParticleInstance* Do_AddParticle(Entity entity, const char* name)
{
	return Do_AddParticle3(name, Get_Position(entity), 0, 0);
}
ParticleInstance* Do_AddParticle2(const char* name, Vector2 absolutePos)
{
	return Do_AddParticle5(name, absolutePos.X, absolutePos.Y, 0, 0);
}
ParticleInstance* Do_AddParticle3(const char* name, Vector2 absolutePos, int32_t rangeX, int32_t rangeY)
{
	return Do_AddParticle5(name, absolutePos.X, absolutePos.Y, rangeX, rangeY);
}
ParticleInstance* Do_AddParticle4(const char* name, float absoluteX, float absoluteY)
{
	return Do_AddParticle5(name, absoluteX, absoluteY, 0, 0);
}
ParticleInstance* Do_AddParticle5(const char* name, float absoluteX, float absoluteY, int32_t rangeX, int32_t rangeY)
{
	Point rangePoint = { rangeX, rangeY };
	Point randomWrappedPointInRange = Get_RandomPointInBounds(rangePoint, true, Get_SharedRandom());
	absoluteX += randomWrappedPointInRange.X;
	absoluteY += randomWrappedPointInRange.Y;
	return GameState_GetParticleInstance(Get_ActiveGameState(), name, absoluteX, absoluteY);
}
void Do_AddParticles(const char* name, Vector2 absolutePos, int32_t amount)
{
	Do_AddParticles4(name, absolutePos.X, absolutePos.Y, amount, 0, 0);
}
void Do_AddParticles2(const char* name, float absoluteX, float absoluteY, int32_t amount)
{
	Do_AddParticles4(name, absoluteX, absoluteY, amount, 0, 0);
}
void Do_AddParticles3(const char* name, Vector2 absolutePos, int32_t amount, int32_t rangeX, int32_t rangeY)
{
	Do_AddParticles4(name, absolutePos.X, absolutePos.Y, amount, rangeX, rangeY);
}
void Do_AddParticles4(const char* name, float absoluteX, float absoluteY, int32_t amount, int32_t rangeX, int32_t rangeY)
{
	for (int32_t i = 0; i < amount; i += 1)
	{
		Do_AddParticle5(name, absoluteX, absoluteY, rangeX, rangeY);
	}
}
Point Get_RandomPointInBounds(Point bounds, bool wrapped, Random32* random)
{
	Point pointToReturn = Point_Zero;
	if (bounds.X != 0)
	{
		int32_t valueX = Random32_NextInt(random, bounds.X);
		if (wrapped)
		{
			valueX -= (bounds.X / 2);
		}
		pointToReturn.X = valueX;
	}
	if (bounds.Y != 0)
	{
		int32_t valueY = Random32_NextInt(random, bounds.Y);
		if (wrapped)
		{
			valueY -= (bounds.Y / 2);
		}
		pointToReturn.Y = valueY;
	}
	return pointToReturn;
}
void Do_DestroyParticlesByName(const char* name)
{
	ComponentPack* particles = Get_ComponentPack(C_ParticleInstance);
	PackIterator iter = PackIterator_Begin;
	while (ComponentPack_Next(particles, &iter))
	{
		ParticleInstance* particleData = (ParticleInstance*)iter.mComponent;
		if (Utils_StringEqualTo(particleData->mName, name))
		{
			particleData->mIsComplete = true;
		}
	}
}
void Do_ImprintTile(Vector2 position, int32_t type)
{
	Do_ImprintTile2(position, type, 0, 0);
}
void Do_ImprintTile2(Vector2 position, int32_t type, int32_t offsetX, int32_t offsetY)
{
	CollisionEngineSys_ImprintToCollisionGrid(Get_CollisionEngine(), position.X + offsetX, position.Y + offsetY, 1, 1, type);
}
void Do_ImprintTile3(int32_t type, float x, float y)
{
	CollisionEngineSys_ImprintToCollisionGrid(Get_CollisionEngine(), x, y, 1, 1, type);
}
void Do_ImprintTile4(int32_t type, float x, float y, int32_t offsetX, int32_t offsetY)
{
	CollisionEngineSys_ImprintToCollisionGrid(Get_CollisionEngine(), x + offsetX, y + offsetY, 1, 1, type);
}
void Do_ImprintTiles(Entity entity, float x, float y, int32_t type, int32_t amountX, int32_t amountY)
{
	CollisionEngineSys_ImprintToCollisionGrid(Get_CollisionEngine(), x, y, amountX, amountY, type);
}
void Do_ImprintTiles2(Vector2 position, int32_t type, int32_t width, int32_t height)
{
	Do_ImprintTiles3(position, type, width, height, 0, 0);
}
void Do_ImprintTiles3(Vector2 position, int32_t type, int32_t width, int32_t height, int32_t offsetX, int32_t offsetY)
{
	CollisionEngineSys_ImprintToCollisionGrid(Get_CollisionEngine(), position.X + offsetX, position.Y + offsetY, width, height, type);
}
void Do_ImprintTiles4(int32_t type, float x, float y, int32_t width, int32_t height)
{
	CollisionEngineSys_ImprintToCollisionGrid(Get_CollisionEngine(), x, y, width, height, type);
}
void Do_FlipX(Entity entity)
{
	Do_SetFlipXBool(entity, !Is_FlipX(entity));
}
void Do_SetFlipXBool(Entity entity, bool value)
{
	Get_DrawActor(entity)->mIsFlipX = value;
}
void Do_SetFlipXFloat(Entity entity, float value)
{
	Do_SetFlipXBool(entity, value < 0);
}
void Do_SetFlipXDouble(Entity entity, double value)
{
	Do_SetFlipXBool(entity, value < 0);
}
void Do_SetFlipXInt(Entity entity, int32_t value)
{
	Do_SetFlipXBool(entity, value < 0);
}
void Do_FlipY(Entity entity)
{
	Do_SetFlipYBool(entity, !Is_FlipY(entity));
}
void Do_SetFlipYBool(Entity entity, bool value)
{
	Get_DrawActor(entity)->mIsFlipY = value;
}
void Do_SetFlipYFloat(Entity entity, float value)
{
	Do_SetFlipYBool(entity, value < 0);
}
void Do_SetFlipYDouble(Entity entity, double value)
{
	Do_SetFlipYBool(entity, value < 0);
}
void Do_SetFlipYInt(Entity entity, int32_t value)
{
	Do_SetFlipYBool(entity, value < 0);
}
void Do_AddCameraShake(int32_t timerLimit, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY)
{
	SpecialShakeCameraSys_Create(timerLimit, minX, maxX, minY, maxY);
}
void Do_ShakeCamera(int32_t minX, int32_t maxX, int32_t minY, int32_t maxY)
{
	Camera_ShakeCamera(Get_Camera(), minX, maxX, minY, maxY);
}
void Do_AddCameraShakeWhileOnScreen(Entity entity, int32_t timerLimit, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY, float mul)
{
	if (Is_OnScreen2(entity, mul))
	{
		Do_AddCameraShake(timerLimit, minX, maxX, minY, maxY);
	}
}
void Do_FacePlayer(Entity entity, bool reverse)
{
	Do_FaceSomewhere2(entity, Get_PlayerX(), reverse);
}
void Do_FacePlayer2(Entity entity)
{
	Do_FaceSomewhere(entity, Get_PlayerX());
}
void Do_FaceSomewhere(Entity entity, float x)
{
	Do_FaceSomewhere2(entity, x, false);
}
void Do_FaceSomewhere2(Entity entity, float x, bool reverse)
{
	if (Get_DirectionToSomewhereX(entity, x) == -1)
	{
		Do_SetFlipXBool(entity, reverse ? false : true);
	}
	else
	{
		Do_SetFlipXBool(entity, reverse ? true : false);
	}
}
void Do_ResetFrameTimer(Timer* timer, int32_t time)
{
	Timer_Reset(timer);
	timer->mLimit = time;
}
void Do_ResetFrameTimer2(Timer* timer)
{
	Timer_Reset(timer);
}
void Do_ResetEaseTimer(TimerDouble* timer, int32_t time)
{
	//TimerDouble_Setup(timer, time);
}
bool Do_MoveToPlayer(Entity entity, float speed)
{
	if (!Is_PlayerPresent(true))
	{
		return false;
	}

	return Do_MoveToPlayer2(entity, speed, false, false);
}
bool Do_MoveToPlayer2(Entity entity, float speed, bool nullifyX, bool nullifyY)
{
	if (!Is_PlayerPresent(true))
	{
		return false;
	}

	Entity player = Get_Player();
	float playerX = Get_X(player);
	float playerY = Get_Y(player);
	if (nullifyX)
	{
		playerX = Get_X(entity);
	}
	if (nullifyY)
	{
		playerY = Get_Y(entity);
	}

	return Do_MoveSomewhere(entity, playerX, playerY, speed);
}
void Do_MoveToClosestPlayer(Entity entity, float speed)
{
	Do_MoveToPlayer(entity, speed);
	//OeThing entity = Get_ClosestPlayer();
	//Do_MoveSomewhere(entity.GetCurrentX(), entity.GetCurrentY(), speed);
}
void Do_PlaySoundWhileOnScreen(Entity entity, const char* name)
{
	if (Is_OnScreen(entity))
	{
		Do_PlaySound(name);
	}
}
void Do_PauseAllSounds(int32_t priority)
{
	SoundEffect_PauseAllSounds(priority);
}
void Do_ResumeAllSounds(int32_t priority)
{
	SoundEffect_ResumeAllSounds(priority);
}
void Do_StopAllSounds(void)
{
	SoundEffect_StopAllSounds();
}
void Do_PlaySound(const char* name)
{
	SoundEffect_PlaySound(name);
}
void Do_PlaySoundWithForcedVolume(const char* name, float forcedVolume)
{
	SoundEffect_PlaySoundWithForcedVolume(name, forcedVolume);
}
void Do_StopSound(const char* name)
{
	SoundEffect_StopSound(name);
}
void Do_PauseMusic(int32_t priority)
{
	Music_PauseMusic(priority);
}
void Do_ResumeMusic(int32_t priority)
{
	Music_ResumeMusic(priority);
}
void Do_StopMusic(void)
{
	Music_StopMusic();
}
void Do_PlayMusic(const char* name)
{
	Do_PlayMusic3(name, true, false, false, 0, false, 0);
}
void Do_PlayMusic2(const char* nextTrack, bool isLooping, bool isForced)
{
	Do_PlayMusic3(nextTrack, isLooping, isForced, false, 0, false, 0);
}
void Do_PlayMusic3(const char* name, bool isLooping, bool isForced, bool isFadeIn, int32_t fadeInTime, bool isFadeOut, int32_t fadeOutTime)
{
	Music_PlayMusic(name, isLooping, isForced, isFadeIn, fadeInTime, isFadeOut, fadeOutTime);
}
void Do_PlayPreviousTrack(void)
{
	Music_PlayPreviousTrack();
}
void Do_FadeOutMusic(int32_t time)
{
	Music_FadeOutMusic(time);
}
void Do_DestroyChildren(Entity entity)
{
	EntitySearch* search = Do_SearchForChildren(entity);
	for (int32_t i = 0; i < search->len; i += 1)
	{
		Do_SetComplete(search->entities[i]);
	}
}
void Do_DestroyChildrenByName(Entity entity, const char* name)
{
	Do_DestroyChildrenByName2(entity, name, EE_STR_EMPTY);
}
void Do_DestroyChildrenByName2(Entity entity, const char* name, const char* particle)
{
	EntitySearch* search = Do_SearchForChildrenWithName(entity, name);
	for (int32_t i = 0; i < search->len; i += 1)
	{
		Entity target = search->entities[i];
		if (!Utils_StringEqualTo(particle, EE_STR_EMPTY))
		{
			Do_AddParticle2(particle, Get_Position(target));
		}
		Do_SetComplete(target);
	}
}
void Do_DestroyEntitiesWithName(const char* name)
{
	Do_DestroyEntitiesWithName2(name, EE_STR_EMPTY);
}
void Do_DestroyEntitiesWithName2(const char* name, const char* particle)
{
	EntitySearch* search = Do_SearchForEntitiesWithName(name);
	for (int32_t i = 0; i < search->len; i += 1)
	{
		Entity target = search->entities[i];
		if (!Utils_StringEqualTo(particle, EE_STR_EMPTY))
		{
			Do_AddParticle2(particle, Get_Position(target));
		}
		Do_SetComplete(target);
	}
}
void Do_KillIfUnderCamera(Entity entity)
{
	if (Is_UnderCamera(entity))
	{
		Do_SetComplete(entity);
	}
}
void Do_SetDrawDisabledByModuloOverHalf(Entity entity, int32_t value)
{
	Do_SetDrawDisabledByModuloOverHalf2(entity, value, Get_LevelFrameCount());
}
void Do_SetDrawDisabledByModuloOverHalf2(Entity entity, int32_t value, int32_t source)
{
	Do_SetDrawDisabled(entity, source % value >= value / 2);
}
void Do_SetDrawDisabledByModulo(Entity entity, int32_t value, int32_t target)
{
	Do_SetDrawDisabledByModulo2(entity, value, target, Get_LevelFrameCount());
}
void Do_SetDrawDisabledByModulo2(Entity entity, int32_t value, int32_t target, int32_t source)
{
	Do_SetDrawDisabled(entity, source % value == target);
}
void Do_SetColorByModulo(Entity entity, Color tint, int32_t value, int32_t target)
{
	Do_SetColorByModulo2(entity, tint, value, target, Get_LevelFrameCount());
}
void Do_SetColorByModulo2(Entity entity, Color tint, int32_t value, int32_t target, int32_t source)
{
	if (source % value == target)
	{
		Do_SetColor(entity, tint);
	}
	else
	{
		Do_SetColor(entity, COLOR_WHITE);
	}
}
void Do_SetColorByModuloOverHalf(Entity entity, Color tint, int32_t value)
{
	Do_SetColorByModuloOverHalf2(entity, tint, value, Get_LevelFrameCount());
}
void Do_SetColorByModuloOverHalf2(Entity entity, Color tint, int32_t value, int32_t source)
{
	if (source % value >= value / 2)
	{
		Do_SetColor(entity, tint);
	}
	else
	{
		Do_SetColor(entity, COLOR_WHITE);
	}
}
void Do_SetHitFlashByModulo(Entity entity, int32_t value, int32_t target)
{
	Do_SetHitFlashByModulo2(entity, value, target, Get_LevelFrameCount());
}
void Do_SetHitFlashByModulo2(Entity entity, int32_t value, int32_t target, int32_t source)
{
	if (source % value == target)
	{
		Do_SetShaderAsWhiteFlash(entity);
	}
	else
	{
		Do_SetShader(entity, NULL);
	}
}
void Do_SetHitFlashByModuloOverHalf(Entity entity, int32_t value)
{
	Do_SetHitFlashByModuloOverHalf2(entity, value, Get_LevelFrameCount());
}
void Do_SetHitFlashByModuloOverHalf2(Entity entity, int32_t value, int32_t source)
{
	if (source % value >= value / 2)
	{
		Do_SetShaderAsWhiteFlash(entity);
	}
	else
	{
		Do_SetShader(entity, NULL);
	}
}
bool Do_MoveSomewherePoint(Entity entity, Point target, float speed)
{
	return Do_MoveSomewhere(entity, (float)target.X, (float)target.Y, speed);
}
bool Do_MoveSomewhereVector2(Entity entity, Vector2 target, float speed)
{
	return Do_MoveSomewhere(entity, target.X, target.Y, speed);
}
bool Do_MoveSomewhere(Entity entity, float targetX, float targetY, float speed)
{
	double distance = Get_Distance2(entity, targetX, targetY);
	bool isVeryClose = (Math_fabs(distance - speed) < .0001f);
	if ((speed < distance) && !isVeryClose)
	{
		double angle = Get_AngleToSomewhere(entity, targetX, targetY);
		double movementX = Get_VectorFromRadianAngleX(angle) * speed;
		double movementY = Get_VectorFromRadianAngleY(angle) * speed;
		Do_MoveAbsolute(entity, (float)(movementX), (float)(movementY));
		return false;
	}
	else
	{
		Vector2 currentPos = Get_Position(entity);
		float movementX = targetX - currentPos.X;
		float movementY = targetY - currentPos.Y;
		Do_MoveAbsolute(entity, movementX, movementY);
		return true;
	}
}
bool Do_MoveSomewhereToVector(Vector2* moveThis, Vector2 target, float speed)
{
	double distance = Get_Distance4(*moveThis, target);
	if (speed < distance)
	{
		double angle = Get_AngleToSomewhere2(*moveThis, target);
		double movementX = Get_VectorFromRadianAngleX(angle) * speed;
		double movementY = Get_VectorFromRadianAngleY(angle) * speed;
		Vector2_AddAssign(moveThis, Vector2_Create((float)movementX, (float)movementY));
		return false;
	}
	else
	{
		Vector2 currentPos = *moveThis;
		float movementX = target.X - currentPos.X;
		float movementY = target.Y - currentPos.Y;
		Vector2_AddAssign(moveThis, Vector2_Create(movementX, movementY));
		return true;
	}
}
void Do_SetImage(Entity entity, int32_t state, int32_t phase)
{
	if (!Is_ImagePhaseTheSame(entity, state, phase))
	{
		Do_SetImageForced(entity, state, phase);
	}
}
void Do_SetImage2(Entity entity, int32_t state, int32_t phase, bool carry)
{
	if (!Is_ImagePhaseTheSame(entity, state, phase))
	{
		Do_SetImageForced2(entity, state, phase, carry);
	}
}
void Do_SetImageForced(Entity entity, int32_t state, int32_t phase)
{
	Do_SetImageForced2(entity, state, phase, false);
}
void Do_SetImageForced2(Entity entity, int32_t state, int32_t phase, bool carry)
{
	DrawActorSys_SetImageState2(entity, Get_DrawActor(entity), state, phase, carry);
}
void Do_RestrictOnSides(Entity entity)
{
	if (Is_LeftOfCamera(entity))
	{
		Body_ForcePosition(Get_Body(entity), (float)Camera_GetLeft(Get_Camera()), Get_Y(entity));
	}
	if (Is_RightOfCamera(entity))
	{
		Body_ForcePosition(Get_Body(entity), (float)Camera_GetRight(Get_Camera()), Get_Y(entity));
	}
}
void Do_SetColor(Entity entity, Color color)
{
	Get_DrawActor(entity)->mTintColor = color;
}
void Do_MovePoint(Entity entity, Point move, float speed)
{
	Do_Move(entity, (float)move.X, (float)move.Y, speed);
}
void Do_MoveVector2(Entity entity, Vector2 move, float speed)
{
	Do_Move(entity, move.X, move.Y, speed);
}
void Do_Move(Entity entity, float x, float y, float speed)
{
	Do_MoveAbsolute(entity, x * speed, y * speed);
}
void Do_MovePointAbsolute(Entity entity, Point move)
{
	Do_MoveAbsolute(entity, (float)move.X, (float)move.Y);
}
void Do_MoveVector2Absolute(Entity entity, Vector2 move)
{
	Do_MoveAbsolute(entity, move.X, move.Y);
}
void Do_MoveAbsolute(Entity entity, float x, float y)
{
	FakePosition* position = (FakePosition*)Get_Component(C_FakePosition, entity);
	position->mFakePosition.X += x;
	position->mFakePosition.Y += y;
	bool wasSuccessful;
	Body* body = (Body*)TryGet_Component(C_Body, entity, &wasSuccessful);
	if (wasSuccessful)
	{
		Body_Move(body, x, y);
	}
}
void Do_MoveAtAngle(Entity entity, double angle, float speed)
{
	double vecX = Get_VectorFromRadianAngleX(angle);
	double vecY = Get_VectorFromRadianAngleY(angle);
	double tempX = vecX * speed;
	double tempY = vecY * speed;
	float moveX = (float)tempX;
	float moveY = (float)tempY;
	Do_MoveAbsolute(entity, moveX, moveY);
}

//GETS
Random32* Get_SharedRandom(void)
{
	return Globals_GetSharedRandom();
}
void Do_SetDebugForcedMove(Entity entity)
{
#ifdef EDITOR_MODE
	int32_t forcedMove = Get_DebugForcedMove(entity);
	if (forcedMove >= 0)
	{
		Do_SetMove(entity, forcedMove);
	}
#endif
}
int32_t Get_DebugForcedMove(Entity entity)
{
#ifdef EDITOR_MODE
	int32_t forceMove = Get_TuningAsInt(entity, "DEBUG_FORCED_MOVE");
	if (forceMove < 1)
	{
		return -1;
	}
	return forceMove - 1;
#else
	return -1;
#endif
}
int32_t Get_AmountOfBodies(Entity entity)
{
	return 0;
	/*
	int32_t counter = 0;
	if (Is_BodyPresent(entity))
	{
		counter += 1;
	}
	std_shared_ptr<EntitySearch> search = Do_SearchForChildrenWithComponent<OeTagIsExtraBody>(entity);
	counter += search->mList.size();
	return counter;
	*/
}
MirrorEffect* Get_MirrorEffect(Entity entity)
{
	return (MirrorEffect*)Get_Component(C_MirrorEffect, entity);
}
Random32* Get_Random(Entity entity)
{
	return (Random32*)Get_Component(C_Random32, entity);
}
NodeMovingKit* Get_NodeMovingKit(Entity entity)
{
	return (NodeMovingKit*)Get_Component(C_NodeMovingKit, entity);
}
HitFlashKit* Get_HitFlashKit(Entity entity)
{
	return (HitFlashKit*)Get_Component(C_HitFlashKit, entity);
}
MoveGetter* Get_MoveGetter(Entity entity)
{
	return (MoveGetter*)Get_Component(C_MoveGetter, entity);
}
float Get_Rotate(Entity entity)
{
	return Get_DrawActor(entity)->mRotation;
}
int32_t Get_AttackId(void)
{
	int32_t returnValue = _mAttackIdCounter;
	_mAttackIdCounter += 1;
	return returnValue;
}
float Get_ScaleX(Entity entity)
{
	return Get_DrawActor(entity)->mScale.X;
}
float Get_ScaleY(Entity entity)
{
	return Get_DrawActor(entity)->mScale.Y;
}
EntitySearch* Get_EntitiesInPlay(void)
{
	return EntitySearch_CreateNewAllEntitySearch();
}
double Get_CurrentCircleX(float degree, float degreeOffset, float radius)
{
	return Math_cos(Math_ToRadians(degree + degreeOffset)) * radius;
}
double Get_CurrentCircleY(float degree, float degreeOffset, float radius)
{
	return Math_sin(Math_ToRadians(degree + degreeOffset)) * radius;
}
int32_t Get_DirectionFromBool(bool value)
{
	return Get_DirectionFromBool2(value, false);
}
int32_t Get_DirectionFromBool2(bool value, bool flip)
{
	int32_t temp = 1;
	if (!value)
	{
		temp = -1;
	}
	if (flip)
	{
		temp = -temp;
	}
	return temp;
}
int32_t Get_TileSize(void)
{
	return TILE_SIZE;
}
int32_t Get_HalfTileSize(void)
{
	return HALF_TILE_SIZE;
}
float Get_InterpolatedX(Entity entity, double delta)
{
	return 0;
	//double temp = Utils_GetInterpolated(delta, Body_GetPosition(Get_Body(entity)).X, Body_GetLastRenderPosition(Get_Body(entity)).X);
	//return (float)temp;
}
float Get_InterpolatedY(Entity entity, double delta)
{
	return 0;
	//double temp = Utils_GetInterpolated(delta, Body_GetPosition(Get_Body(entity)).Y, Body_GetLastRenderPosition(Get_Body(entity)).Y);
	//return (float)temp;
}
Vector2 Get_AdjustedMouse(void)
{
	return Vector2_Zero;
	// return Input_GetCameraAdjustedMouseForRetroScreen(Get_Camera());
}
float Get_AdjustedMouseX(void)
{
	return 0;
	//return Input_GetCameraAdjustedMouseForRetroScreenX(Get_Camera());
}
float Get_AdjustedMouseY(void)
{
	return 0;
	//return Input_GetCameraAdjustedMouseForRetroScreenY(Get_Camera());
}
float Get_MouseX(void)
{
	return (float)Input_GetMouseX();
}
float Get_MouseY(void)
{
	return (float)Input_GetMouseY();
}
Color Get_Color(Entity entity)
{
	return Get_DrawActor(entity)->mTintColor;
}
CollisionEngine* Get_CollisionEngine(void)
{
	return (CollisionEngine*)Get_FirstSetComponent(C_CollisionEngine);
}
Point Get_Node(Entity entity, int32_t i)
{
	Point* arr_nodes = Get_ArrGridNodes(entity);
	if (arrlen(arr_nodes) == 0)
	{
		return Points_NegativeOne;
	}

	if (arrlen(arr_nodes) > i)
	{
		Point* node = &arr_nodes[i];
		Point gridPosition = Get_GridPosition(entity);
		int32_t positionX = (node->X + gridPosition.X) * TILE_SIZE + HALF_TILE_SIZE;
		int32_t positionY = (node->Y + gridPosition.Y) * TILE_SIZE + HALF_TILE_SIZE;
		return Point_Create(positionX, positionY);
	}
	else
	{
		return Points_NegativeOne;
	}
}
Point Get_NodeAsGrid(Entity entity, int32_t i)
{
	Point* arr_nodes = Get_ArrGridNodes(entity);
	if (arrlen(arr_nodes) == 0)
	{
		return Points_NegativeOne;
	}

	if (arrlen(arr_nodes) > i)
	{
		return arr_nodes[i];
	}
	else
	{
		return Points_NegativeOne;
	}
}
int32_t Get_IndexOfRandomNode(Entity entity, Random32* random)
{
	return Get_IndexOfRandomNode2(entity, random, -1);
}
int32_t Get_IndexOfRandomNode2(Entity entity, Random32* random, int32_t ignore)
{
	Point* arr_nodes = Get_ArrGridNodes(entity);
	if (arrlen(arr_nodes) == 0)
	{
		return -1;
	}

	int64_t count = arrlen(arr_nodes);
	int32_t index = Random32_NextInt(random, (int32_t)count);

	if (ignore != -1)
	{
		while (index == ignore)
		{
			index = Get_IndexOfRandomNode(entity, random);
		}
	}

	return index;
}
int32_t Get_AmountOfNodes(Entity entity)
{
	return (int32_t)arrlen(Get_ArrGridNodes(entity));
}
Point Get_RandomNode(Entity entity, Random32* random)
{
	return Get_RandomNode2(entity, random, -1);
}
Point Get_RandomNode2(Entity entity, Random32* random, int32_t ignore)
{
	return Get_Node(entity, Get_IndexOfRandomNode2(entity, random, ignore));
}
int32_t Get_IndexOfClosestNodeToPlayerX(Entity entity, int32_t losOffsetX, int32_t losOffsetY)
{
	if (!Is_NodesPresent(entity))
	{
		return -1;
	}

	float playerX = Get_PlayerX();
	float playerY = Get_PlayerY();
	int64_t amountOfNodes = Get_AmountOfNodes(entity);
	if (amountOfNodes > 0)
	{
		float value = FLT_MAX;
		int32_t index = -1;
		for (int32_t i = 0; i < amountOfNodes; i += 1)
		{
			Point currentNode = Get_Node(entity, i);
			float dist = Math_fabsf(currentNode.X - playerX);
			if (dist < value)
			{
				value = dist;
				index = i;
			}
			else if (dist == value) //on same X
			{
				Point indexNode = Get_Node(entity, index);
				if (!Is_LineOfSight2((float)(indexNode.X + losOffsetX), (float)(indexNode.Y + losOffsetY), playerX, playerY))
				{
					if (Is_LineOfSight2((float)(currentNode.X + losOffsetX), (float)(currentNode.Y + losOffsetY), playerX, playerY))
					{
						index = i;
					}
				}
			}
		}
		return index;
	}
	else
	{
		return -1;
	}
}
int32_t Get_IndexOfClosestNodeToThing(Entity entity, Entity target)
{
	if (!Is_NodesPresent(entity))
	{
		return -1;
	}

	float targetX = Get_X(target);
	int64_t amountOfNodes = Get_AmountOfNodes(entity);
	if (amountOfNodes > 0)
	{
		float value = FLT_MAX;
		int32_t index = -1;
		for (int32_t i = 0; i < amountOfNodes; i += 1)
		{
			float dist = Math_fabsf(Get_Node(entity, i).X - targetX);
			if (dist < value)
			{
				value = dist;
				index = i;
			}
		}
		return index;
	}
	else
	{
		return -1;
	}
}
Point Get_ClosestNodeToPlayerX(Entity entity, int32_t losOffsetX, int32_t losOffsetY)
{
	return Get_Node(entity, Get_IndexOfClosestNodeToPlayerX(entity, losOffsetX, losOffsetY));
}
Point Get_ClosestNodeToPlayerY(Entity entity)
{
	float playerY = Get_PlayerY();
	int64_t amountOfNodes = Get_AmountOfNodes(entity);
	if (amountOfNodes > 0)
	{
		float value = FLT_MAX;
		int32_t index = -1;
		for (int32_t i = 0; i < amountOfNodes; i += 1)
		{
			float dist = Math_fabsf(Get_Node(entity, i).Y - playerY);
			if (dist < value)
			{
				value = dist;
				index = i;
			}
		}
		return Get_Node(entity, index);
	}
	else
	{
		return Points_NegativeOne;
	}
}
Entity Get_ParentEntity(Entity entity)
{
	int32_t parentNumber = Get_ParentNumber(entity);
	return Get_Entity(parentNumber);
}
Entity Get_Entity(int32_t entityNumber)
{
	return GameState_GetEntityInPlay(Get_ActiveGameState(), entityNumber);
}
int32_t Get_ParentNumber(Entity entity)
{
	bool wasSuccessful;
	ParentNumber* component = (ParentNumber*)TryGet_Component(C_ParentNumber, entity, &wasSuccessful);
	if (!wasSuccessful)
	{
		return -1;
	}
	else
	{
		return component->mTag;
	}
}
void Do_SetParentNumber(Entity entity, int32_t value)
{
	ParentNumber* component = (ParentNumber*)Get_Component(C_ParentNumber, entity);
	component->mTag = value;
}
const char* Get_Name(Entity entity)
{
	bool wasSuccessful;
	Name* component = (Name*)TryGet_Component(C_Name, entity, &wasSuccessful);
	if (!wasSuccessful)
	{
		return EE_STR_NOT_SET;
	}
	else
	{
		return component->mName;
	}
}
Vector2 Get_InitialPosition(Entity entity)
{
	bool wasSuccessful;
	InitialPosition* component = (InitialPosition*)TryGet_Component(C_InitialPosition, entity, &wasSuccessful);
	if (!wasSuccessful)
	{
		return Vector2_Zero;
	}
	else
	{
		return component->mInitialPosition;
	}
}
Point Get_InitialPositionAsPoint(Entity entity)
{
	Vector2 initialPos = Get_InitialPosition(entity);
	Point tempPoint = { (int32_t)initialPos.X, (int32_t)initialPos.Y };
	return tempPoint;
}
Vector2 Get_FakePosition(Entity entity)
{
	bool wasSuccessful;
	FakePosition* component = (FakePosition*)TryGet_Component(C_FakePosition, entity, &wasSuccessful);
	if (!wasSuccessful)
	{
		return Vector2_Zero;
	}
	else
	{
		return component->mFakePosition;
	}
}
Point Get_GridPosition(Entity entity)
{
	bool wasSuccessful;
	GridPosition* component = (GridPosition*)TryGet_Component(C_GridPosition, entity, &wasSuccessful);
	if (!wasSuccessful)
	{
		return Point_Zero;
	}
	else
	{
		return component->mGridPosition;
	}
}
int32_t Get_CurrentImagePhase(Entity entity, int32_t state)
{
	return DrawActorSys_GetCurrentPhase(entity, state);
}
Camera* Get_Camera(void)
{
	return ((Camera*)Get_FirstSetComponent(C_Camera));
}
Rectangle Get_CameraRectangle(void)
{
	return Camera_GetRectangle(Get_Camera(), 1.0f);
}
Rectangle Get_CameraRectangleExtended(void)
{
	return Camera_GetRectangle(Get_Camera(), CAMERA_EXTENDED_CAMERA);
}
float Get_CenterOfCameraX(void)
{
	return Camera_GetCenterXFloat(Get_Camera());
}
float Get_CenterOfCameraY(void)
{
	return Camera_GetCenterYFloat(Get_Camera());
}
float Get_TopOfCamera(void)
{
	return Camera_GetTopFloat(Get_Camera());
}
float Get_RightOfCamera(void)
{
	return Camera_GetRightFloat(Get_Camera());
}
float Get_BottomOfCamera(void)
{
	return Camera_GetBottomFloat(Get_Camera());
}
float Get_LeftOfCamera(void)
{
	return Camera_GetLeftFloat(Get_Camera());
}
int32_t Get_EntityNumber(Entity entity)
{
	return entity;
}
int32_t Get_PlayerNumber(Entity entity)
{
	bool wasSuccessful;
	PlayerNumber* component = (PlayerNumber*)TryGet_Component(C_PlayerNumber, entity, &wasSuccessful);
	if (!wasSuccessful)
	{
		return -1;
	}
	else
	{
		return component->mTag;
	}
}
void Do_SetPlayerNumber(Entity entity, int32_t playerNumber)
{
	PlayerNumber* component = (PlayerNumber*)Get_Component(C_PlayerNumber, entity);
	component->mTag = playerNumber;
}
GameState* Get_ActiveGameState(void)
{
	return GameStateManager_GetGameState();
}
LevelData* Get_LevelData(void)
{
	return GameHelper_GetLevelData();
}
Resource* Get_LevelDataResource(void)
{
	return GameHelper_GetLevelDataResource();
}
const char* Get_LevelFileName(void)
{
	return Resource_GetFilenameWithoutExtension(Get_LevelDataResource());
}
Rectangle Get_BodyRectangle(Entity entity)
{
	return Body_GetRect(Get_Body(entity));
}
Body* Get_Body(Entity entity)
{
	return (Body*)Get_Component(C_Body, entity);
}
float Get_X(Entity entity)
{
	return Get_Position(entity).X;
}
float Get_Y(Entity entity)
{
	return Get_Position(entity).Y;
}
Vector2 Get_PositionFromBody(Entity entity)
{
	return Body_GetPosition(Get_Body(entity));
}
Vector2 Get_Position(Entity entity)
{
	bool wasSuccessful;
	Body* body = (Body*)TryGet_Component(C_Body, entity, &wasSuccessful);
	if (wasSuccessful)
	{
		return Body_GetPosition(body);
	}
	else
	{
		return Get_FakePosition(entity);
	}
}
Vector2 Get_LastRenderPosition(Entity entity)
{
	bool wasSuccessful;
	Body* body = (Body*)TryGet_Component(C_Body, entity, &wasSuccessful);
	if (wasSuccessful)
	{
		return Body_GetLastRenderPosition(body);
	}
	else
	{
		return Get_FakePosition(entity);
	}
}
float Get_InitialX(Entity entity)
{
	return Get_InitialPosition(entity).X;
}
float Get_InitialY(Entity entity)
{
	return Get_InitialPosition(entity).Y;
}
int32_t Get_FacingDirection(Entity entity)
{
	if (Is_FlipX(entity))
	{
		return -1;
	}
	else
	{
		return 1;
	}
}
int32_t Get_ValueLockedToTileSize(float value)
{
	int32_t tiles = (int32_t)(value / TILE_SIZE);
	return tiles * TILE_SIZE;
}
int32_t Get_TuningAsInt(Entity entity, const char* dataName)
{
	return 0;
}
int32_t Get_ClampedValueAsInt(int32_t value, int32_t limit)
{
	int32_t signum = Math_SignumInt(value);
	int32_t abs = Math_abs(value);
	if (abs > limit)
	{
		abs = limit;
	}
	int32_t returnValue = abs * signum;
	return returnValue;
}
float Get_ClampedValueAsFloat(float value, float limit)
{
	int32_t signum = Math_SignumFloat(value);
	float abs = Math_fabsf(value);
	if (abs > limit)
	{
		abs = limit;
	}
	float returnValue = abs * signum;
	return returnValue;
}
double Get_ClampedValueAsDouble(double value, double limit)
{
	int32_t signum = Math_SignumDouble(value);
	double abs = Math_fabs(value);
	if (abs > limit)
	{
		abs = limit;
	}
	double returnValue = abs * signum;
	return returnValue;
}
Animation* Get_Animation(Entity entity, int32_t state, int32_t phase)
{
	return DrawActorSys_GetAnimation(entity, state, phase);
}
Animation* Get_CurrentAnimation(Entity entity, int32_t state)
{
	return DrawActorSys_GetCurrentAnimation(entity, state);
}
void Get_PointDirectionFromEightWayIntegerDirection(Point* point, int32_t value)
{
	switch (value)
	{
	case 0:
		point->X = 0;
		point->Y = -1;
		break;
	case 1:
		point->X = 1;
		point->Y = -1;
		break;
	case 2:
		point->X = 1;
		point->Y = 0;
		break;
	case 3:
		point->X = 1;
		point->Y = 1;
		break;
	case 4:
		point->X = 0;
		point->Y = 1;
		break;
	case 5:
		point->X = -1;
		point->Y = 1;
		break;
	case 6:
		point->X = -1;
		point->Y = 0;
		break;
	default:
		point->X = -1;
		point->Y = -1;
		break;
	}
}
Body* Get_PlayerBody(void)
{
	return Get_Body(Get_Player());
}
Entity Get_ClosestPlayer2(Entity entity)
{
	return Get_ClosestPlayer(Get_X(entity), Get_Y(entity));
}
Vector2 Get_PlayerPosition(void)
{
	Entity player = Get_Player();
	if (player == ENTITY_NOTHING)
	{
		Logger_LogError("Player not present.");
		return Vector2_Zero;
	}

	return Get_Position(player);
}
float Get_PlayerX(void)
{
	return Get_PlayerPosition().X;
}
float Get_PlayerY(void)
{
	return Get_PlayerPosition().Y;
}
float Get_PlayerXByNumber(int32_t playerNumber)
{
	return Get_X(Get_PlayerByNumber(playerNumber));
}
float Get_PlayerYByNumber(int32_t playerNumber)
{
	return Get_Y(Get_PlayerByNumber(playerNumber));
}
float Get_ClosestPlayerX(Entity entity)
{
	return Get_X(Get_ClosestPlayer2(entity));
}
float Get_ClosestPlayerY(Entity entity)
{
	return Get_Y(Get_ClosestPlayer2(entity));
}
double Get_AngleToPlayer(Entity entity)
{
	return Get_AngleToThing(entity, Get_Player());
}
double Get_AngleToPlayerByNumber(Entity entity, int32_t playerNumber)
{
	return Get_AngleToThing(entity, Get_PlayerByNumber(playerNumber));
}
double Get_AngleToClosestPlayer(Entity entity)
{
	return Get_AngleToThing(entity, Get_ClosestPlayer2(entity));
}
double Get_AngleToThing(Entity entity, Entity target)
{
	return Get_AngleToSomewhere(entity, Get_X(target), Get_Y(target));
}
double Get_AngleToSomewhere(Entity entity, float otherX, float otherY)
{
	return Get_Angle(Get_X(entity), Get_Y(entity), otherX, otherY);
}
double Get_AngleToSomewhere2(Vector2 valueA, Vector2 valueB)
{
	return Get_Angle(valueA.X, valueA.Y, valueB.X, valueB.Y);
}
float Get_DegreeAngleFromRadianAngle(float degree)
{
	return Math_ToDegrees(degree);
}
double Get_Angle(float x1, float y1, float x2, float y2)
{
	return Math_GetAngle(x1, y1, x2, y2);
}
void Get_VectorAngle(float x1, float y1, float x2, float y2, Vector2* vec)
{
	double angle = Get_Angle(x1, y1, x2, y2);
	Get_VectorFromRadianAngle(angle, vec);
}
void Get_VectorAngleToSomewhere(Entity entity, float otherX, float otherY, Vector2* vec)
{
	Get_VectorAngle(Get_X(entity), Get_Y(entity), otherX, otherY, vec);
}
int32_t Get_Width(Entity entity)
{
	return Body_GetWidth(Get_Body(entity));
}
int32_t Get_Height(Entity entity)
{
	return Body_GetHeight(Get_Body(entity));
}
Point Get_BodyCollisionGridPosition(Entity entity)
{
	return Point_Zero;
	//Vector2 position = Body_GetPosition(Get_Body(entity));
	//return CollisionEngineSys_GetCollisionGridPosition(position.X, position.Y);
}
int32_t Get_BodyCollisionGridPositionX(Entity entity)
{
	return Get_BodyCollisionGridPosition(entity).X;
}
int32_t Get_BodyCollisionGridPositionY(Entity entity)
{
	return Get_BodyCollisionGridPosition(entity).Y;
}
int32_t Get_AmountOfMyChildrenByName(Entity entity, const char* name)
{
	EntitySearch* search = Do_SearchForChildrenWithName(entity, name);
	return search->len;
}
int32_t Get_AmountOfMyChildren(Entity entity)
{
	EntitySearch* search = Do_SearchForChildren(entity);
	return search->len;
}
const char* Get_StringSettingAsString(Entity entity, const char* key)
{
	StringPair* arr_settings = Get_ArrStringSettings(entity);
	if (arrlen(arr_settings) == 0)
	{
		return EE_STR_EMPTY;
	}

	for (int32_t i = 0; i < arrlen(arr_settings); i += 1)
	{
		StringPair* p = &arr_settings[i];
		if (Utils_StringEqualTo(p->mKey, key))
		{
			return p->mValue;
		}
	}

	return EE_STR_EMPTY;
}
StringPair* Get_ArrStringSettings(Entity entity)
{
	bool wasSuccessful;
	StringSettings* component = (StringSettings*)TryGet_Component(C_StringSettings, entity, &wasSuccessful);
	if (!wasSuccessful)
	{
		return NULL;
	}
	else
	{
		return component->arr_given_settings;
	}
}
int32_t Get_StringSettingAsInt(Entity entity, const char* setting)
{
	return Utils_ParseInt(Get_StringSettingAsString(entity, setting));
}
bool Get_StringSettingAsBooleanByChar(Entity entity, const char* setting)
{
	const char* temp = Get_StringSettingAsString(entity, setting);
	if (Utils_StringEqualTo(temp, EE_STR_EMPTY))
	{
		Logger_LogError("Missing char bool setting: ");
		Logger_LogError(setting);
		return false;
	}
	return Utils_ParseBooleanFromChar(temp);
}
float Get_StringSettingAsFloat(Entity entity, const char* setting)
{
	return Utils_ParseFloat(Get_StringSettingAsString(entity, setting));
}
bool Get_StringSettingAsBoolean(Entity entity, const char* setting)
{
	const char* temp = Get_StringSettingAsString(entity, setting);
	if (Utils_StringEqualTo(temp, EE_STR_EMPTY))
	{
		Logger_LogError("Missing bool setting: ");
		Logger_LogError(setting);
		return false;
	}
	if (Utils_StringEqualTo(temp, "T") || Utils_StringEqualTo(temp, "TRUE"))
	{
		return true;
	}
	else
	{
		return false;
	}
}
double Get_VectorFromRadianAngleX(double radianAngle)
{
	return Math_GetVectorFromRadianAngleX(radianAngle);
}
double Get_VectorFromRadianAngleY(double radianAngle)
{
	return Math_GetVectorFromRadianAngleY(radianAngle);
}
void Get_VectorFromRadianAngle(double radianAngle, Vector2* vec)
{
	vec->X = (float)Get_VectorFromRadianAngleX(radianAngle);
	vec->Y = (float)Get_VectorFromRadianAngleY(radianAngle);
}
void Get_VectorFromDegreeAngle(float degreeAngle, Vector2* vec)
{
	Get_VectorFromRadianAngle(Math_ToRadians(degreeAngle), vec);
}
double Get_VectorDirectionToPlayerX(Entity entity)
{
	double angle = Get_AngleToPlayer(entity);
	return Get_VectorFromRadianAngleX(angle);
}
double Get_VectorDirectionToPlayerY(Entity entity)
{
	double angle = Get_AngleToPlayer(entity);
	return Get_VectorFromRadianAngleY(angle);
}
double Get_VectorToAngleLegacyDoNotUse(float x, float y)
{
	return Math_atan2(x, -y);
}
double Get_VectorToAngle(float x, float y)
{
	return Math_atan2(y, x);
}
double Get_WrappedAngle(double radians)
{
	while (radians < -MATH_PI)
	{
		radians += MATH_PI * 2;
	}
	while (radians > MATH_PI)
	{
		radians -= MATH_PI * 2;
	}
	return radians;
}
float Get_WrappedDegreeAngle(float degree)
{
	while (degree < 0)
	{
		degree += 360;
	}
	while (degree > 360)
	{
		degree -= 360;
	}
	return degree;
}
double Get_LimitedAngle(double target, double current, double limit)
{
	double difference = Get_AngleDifference(target, current);

	double turnSpeed = limit;
	difference = Math_MinDouble(difference, turnSpeed);
	difference = Math_MaxDouble(difference, -turnSpeed);

	return current + difference;
}
double Get_AngleDifference(double target, double current)
{
	return Get_WrappedAngle(target - current);
}
int32_t Get_OriginalWidth(Entity entity)
{
	return Body_GetOriginalWidth(Get_Body(entity));
}
int32_t Get_OriginalHeight(Entity entity)
{
	return Body_GetOriginalHeight(Get_Body(entity));
}
Entity Get_ChildByName(Entity entity, const char* name)
{
	return Get_ChildByName2(entity, name, 0);
}
Entity Get_ChildByName2(Entity entity, const char* name, int32_t number)
{
	EntitySearch* search = Do_SearchForChildrenWithName(entity, name);
	if (number < search->len)
	{
		return search->entities[number];
	}
	else
	{
		return ENTITY_NOTHING;
	}
}
Entity Get_ChildByTag(Entity entity, int32_t value)
{
	return Get_ChildByTag2(entity, value, 0);
}
Entity Get_ChildByTag2(Entity entity, int32_t value, int32_t number)
{
	EntitySearch* search = Do_SearchForChildrenWithIntTag(entity, value);
	if (number < search->len)
	{
		return search->entities[number];
	}
	else
	{
		return ENTITY_NOTHING;
	}
}
int32_t Get_DirectionToPlayerX(Entity entity)
{
	if (!Is_PlayerPresent(true))
	{
		return 0;
	}

	return Get_DirectionHelper(Body_GetPosition(Get_Body(Get_Player())).X, Body_GetPosition(Get_Body(entity)).X);
}
int32_t Get_DirectionToPlayerY(Entity entity)
{
	if (!Is_PlayerPresent(true))
	{
		return 0;
	}

	return Get_DirectionHelper(Body_GetPosition(Get_Body(Get_Player())).Y, Body_GetPosition(Get_Body(entity)).Y);
}
int32_t Get_DirectionToSomewhereX(Entity entity, float value)
{
	return Get_DirectionHelper(value, Get_X(entity));
}
int32_t Get_DirectionToSomewhereY(Entity entity, float value)
{
	return Get_DirectionHelper(value, Get_Y(entity));
}
int32_t Get_DirectionHelper(float target, float me)
{
	if (target < me)
	{
		return -1;
	}
	else
	{
		return 1;
	}
}
bool Get_DirectionToPlayerBoolean(Entity entity)
{
	if (Get_DirectionToPlayerX(entity) == 1)
	{
		return false;
	}
	return true;
}
bool Get_Direction(int32_t value)
{
	if (value > 0)
	{
		return true;
	}
	return false;
}
float Get_DistanceToPlayerX(Entity entity)
{
	return Get_DistanceToPlayerAsVec2(entity).X;
}
float Get_DistanceToPlayerY(Entity entity)
{
	return Get_DistanceToPlayerAsVec2(entity).Y;
}
Vector2 Get_DistanceToPlayerAsVec2(Entity entity)
{
	return Get_DistanceAsVec22(Get_Position(entity), Get_PlayerPosition());
}
Vector2 Get_DistanceAsVec2(Entity entity, Vector2 pos2)
{
	return Get_DistanceAsVec22(Get_Position(entity), pos2);
}
Vector2 Get_DistanceAsVec22(Vector2 pos1, Vector2 pos2)
{
	Vector2 temp = { Get_Distance5(pos1.X, pos2.X), Get_Distance5(pos1.Y, pos2.Y) };
	return temp;
}
double Get_DistanceToPlayer(Entity entity)
{
	return Get_Distance(entity, Get_Player());
}
double Get_Distance(Entity entity, Entity target)
{
	return Get_Distance4(Get_Position(entity), Get_Position(target));
}
double Get_Distance2(Entity entity, float pos2X, float pos2Y)
{
	Vector2 position2 = { pos2X, pos2Y };
	return Get_Distance4(Get_Position(entity), position2);
}
double Get_Distance3(Entity entity, Vector2 pos2)
{
	return Get_Distance4(Get_Position(entity), pos2);
}
double Get_Distance4(Vector2 pos1, Vector2 pos2)
{
	return Get_Distance6(pos1.X, pos1.Y, pos2.X, pos2.Y);
}
float Get_Distance5(float x1, float x2)
{
	return Math_GetDistanceFloat(x1, x2);
}
double Get_Distance6(float x1, float y1, float x2, float y2)
{
	return Math_GetDistanceEuclideanFloat(x1, y1, x2, y2);
}
int32_t Get_RandomBinaryDirection(Random32* random)
{
	return Random32_NextInt(random, 2) == 0 ? 1 : -1;
}
int32_t Get_RandomDirection(Random32* random)
{
	int32_t value = Random32_NextInt(random, 3);
	if (value == 0)
	{
		return -1;
	}
	else if (value == 1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
float Get_ArcVelocityX(float targetPosX, float throwPosX, int32_t travelTime, float mul)
{
	float diffX = targetPosX - throwPosX;
	float startVelX;
	startVelX = diffX / travelTime;
	return startVelX * mul;
}
float Get_ArcVelocityY(float targetPosY, float throwPosY, float grav, int32_t travelTime, float mul)
{
	float diffY = targetPosY - throwPosY;
	float startVelY;
	startVelY = (diffY - 0.5f * grav * travelTime * travelTime) / travelTime;
	return startVelY * mul;
}
int32_t Get_LevelFrameCount(void)
{
	return ((LevelFrameCounter*)Get_FirstSetComponent(C_LevelFrameCounter))->mCounter;
}
int32_t Get_CollisionBit(float posX, float posY, int32_t tileOffsetX, int32_t tileOffsetY)
{
	float bitX = posX + (tileOffsetX * TILE_SIZE);
	float bitY = posY + (tileOffsetY * TILE_SIZE);
	return CollisionEngineSys_GetCollisionBit(Get_CollisionEngine(), bitX, bitY);
}
int32_t Get_CollisionBitSafe(float posX, float posY, int32_t tileOffsetX, int32_t tileOffsetY, int32_t returnValue)
{
	float bitX = posX + (tileOffsetX * TILE_SIZE);
	float bitY = posY + (tileOffsetY * TILE_SIZE);
	return CollisionEngineSys_GetCollisionBitSafe(Get_CollisionEngine(), bitX, bitY, returnValue);
}
int32_t Get_DirectionFromCameraSideX(Entity entity)
{
	if (Is_OnRightSideOfScreen(entity))
	{
		return 1;
	}
	else
	{
		return -1;
	}
}
int32_t Get_DirectionFromCameraSideY(Entity entity)
{
	if (Is_OnBottomSideOfScreen(entity))
	{
		return 1;
	}
	else
	{
		return -1;
	}
}
Entity Do_BuildActor(Vector2 initialPosition, ThingInstance* instanceData, const char* name)
{
	return Do_BuildActor2(initialPosition.X, initialPosition.Y, instanceData, name);
}
Entity Do_BuildActor2(float initialPositionX, float initialPositionY, ThingInstance* instanceData, const char* name)
{
	return Do_BuildActor6(0, 0, initialPositionX, initialPositionY, instanceData, name, ENTITY_NOTHING);
}
Entity Do_BuildActor3(Vector2 initialPosition, ThingInstance* instanceData, const char* name, Entity parent)
{
	return Do_BuildActor4(initialPosition.X, initialPosition.Y, instanceData, name, parent);
}
Entity Do_BuildActor4(float initialPositionX, float initialPositionY, ThingInstance* instanceData, const char* name, Entity parent)
{
	return Do_BuildActor6(0, 0, initialPositionX, initialPositionY, instanceData, name, parent);
}
Entity Do_BuildActor5(int32_t gridPositionX, int32_t gridPositionY, float initialPositionX, float initialPositionY, ThingInstance* instanceData, const char* name)
{
	return Do_BuildActor6(gridPositionX, gridPositionY, initialPositionX, initialPositionY, instanceData, name, ENTITY_NOTHING);
}
Entity Do_BuildActor6(int32_t gridPositionX, int32_t gridPositionY, float initialPositionX, float initialPositionY, ThingInstance* instanceData, const char* name, Entity parent)
{
	ThingSettings* settings = (ThingSettings*)ResourceManager_GetResourceData(ResourceManagerList_ThingSettings(), name);
	if (settings == NULL)
	{
		Logger_LogError("Unable to build entity:");
		Logger_LogError(name);
		return ENTITY_NOTHING;
	}

	Entity entity = GameState_BuildNewEntity(Get_ActiveGameState());

	Do_SetBoolTag(C_TagIsActor, entity, true);

	Do_SetName(entity, name);

	int32_t parentNumber = -1;
	if (parent != ENTITY_NOTHING)
	{
		parentNumber = Get_EntityNumber(parent);
	}
	Do_SetParentNumber(entity, parentNumber);

	Do_SetGridPosition(entity, gridPositionX, gridPositionY);

	Do_SetInitialPosition(entity, initialPositionX, initialPositionY);

	Do_SetPosition(entity, initialPositionX, initialPositionY);

	Point* arr_nodes;
	StringPair* arr_settings;
	if (instanceData != NULL)
	{
		arr_nodes = instanceData->arr_nodes;
		arr_settings = instanceData->arr_settings;
	}
	else
	{
		arr_nodes = NULL;
		arr_settings = NULL;
	}
	Do_SetArrNodes(entity, arr_nodes);
	Do_SetArrStringSettings(entity, arr_settings);

	if (settings->mHasCollision)
	{
		Do_InitBody(entity, settings->mCollisionWidth, settings->mCollisionHeight);
	}

	if (settings->mHasDrawing)
	{
		DrawActor* drawActor = (DrawActor*)Do_InitComponent(C_DrawActor, entity);
		DrawActorSys_Setup(entity, drawActor, settings->sh_graphics_data, 
			settings->mDefaultState, settings->mDefaultPhase);
	}

	if (settings->mHasAI)
	{
		GameHelper_BuildControllerComponent(settings->mRoutineId, entity);
	}

	return entity;
}
void Do_SetSeedFromInitialCoordinates(Entity entity, Random32* random)
{
	Vector2 initialPosition = Get_InitialPosition(entity);
	Point gridPosition = Get_GridPosition(entity);
	float seed = initialPosition.X + initialPosition.Y + gridPosition.X + gridPosition.Y;
	Random32_SetSeed(random, (uint32_t)(seed));
}
void Do_SendBroadcast(int32_t type)
{
	Do_SendBroadcast2(type, -1);
}
void Do_SendBroadcast2(int32_t type, int32_t packet1)
{
	Do_SendBroadcast3(type, packet1, -1);
}
void Do_SendBroadcast3(int32_t type, int32_t packet1, int32_t packet2)
{
	Do_SendBroadcast4(type, packet1, packet2, -1);
}
void Do_SendBroadcast4(int32_t type, int32_t packet1, int32_t packet2, int32_t packet3)
{
	GameState_Do_SendBroadcast(Get_ActiveGameState(), type, packet1, packet2, packet3);
}
int32_t Get_CameraHingeBottom(void)
{
	return Get_Camera()->mHingeGateBottom;
}
void Do_SaveGame(void)
{
	GameSaveManager_Save();
}
void Do_SaveUserConfig(void)
{
	Cvars_SaveUserConfig();
}

//IS REGION
bool Is_IntersectingCameraExtended(Camera* camera, int32_t posX, int32_t posY, int32_t width, int32_t height, bool isCentered)
{
	return Is_IntersectingCamera2(camera, posX, posY, width, height, isCentered, CAMERA_EXTENDED_CAMERA);
}
bool Is_IntersectingCamera(Camera* camera, int32_t posX, int32_t posY, int32_t width, int32_t height, bool isCentered)
{
	return Is_IntersectingCamera2(camera, posX, posY, width, height, isCentered, 1);
}
bool Is_IntersectingCamera2(Camera* camera, int32_t posX, int32_t posY, int32_t width, int32_t height, bool isCentered, float mul)
{
	Rectangle rect;
	rect.Width = width;
	rect.Height = height;
	if (isCentered)
	{
		rect.X = posX - (width / 2);
		rect.Y = posY - (height / 2);
	}
	else
	{
		rect.X = posX;
		rect.Y = posY;
	}
	return Camera_IntersectsCameraRectMul(camera, &rect, mul);
}
Rectangle Get_LevelBoundsRectangle(void)
{
	return LevelData_GetLevelBoundsRectangle(Get_LevelData());
}
bool Is_InLevelBounds(Entity entity)
{
	Rectangle levelBounds = Get_LevelBoundsRectangle();
	bool wasSuccessful;
	Body* body = (Body*)TryGet_Component(C_Body, entity, &wasSuccessful);
	if (!wasSuccessful)
	{
		Vector2 temp = Get_FakePosition(entity);
		float posX = temp.X - HALF_TILE_SIZE;
		float posY = temp.Y - HALF_TILE_SIZE;
		Rectangle otherRect = Rectangle_Create((int32_t)posX, (int32_t)posY, TILE_SIZE, TILE_SIZE);
		return Rectangle_Intersects(&levelBounds, &otherRect);
	}
	else
	{
		Rectangle bodyRect = Body_GetRect(body);
		return Rectangle_Intersects(&levelBounds, &bodyRect);
	}
}
bool Is_ThisBodyThePlayerBody(Body* body)
{
	if (body->mOwner == Get_Player())
	{
		return true;
	}
	return false;
}
bool Is_TouchingUp(Entity entity)
{
	return Get_Body(entity)->mTouchedUp;
}
bool Is_TouchingRight(Entity entity)
{
	return Get_Body(entity)->mTouchedRight;
}
bool Is_TouchingDown(Entity entity)
{
	return Get_Body(entity)->mTouchedDown;
}
bool Is_TouchingLeft(Entity entity)
{
	return Get_Body(entity)->mTouchedLeft;
}
bool Is_TouchingBody(Entity entity)
{
	return Get_Body(entity)->mTouchedBody;
}
bool Is_TouchingCollision(Entity entity)
{
	return Get_Body(entity)->mTouchedCollision;
}
bool Is_TouchingLeftOrRight(Entity entity)
{
	return Is_TouchingRight(entity) || Is_TouchingLeft(entity);
}
bool Is_TouchingUpOrDown(Entity entity)
{
	return Is_TouchingUp(entity) || Is_TouchingDown(entity);
}
bool Is_BodyDisabled(Entity entity)
{
	return Get_Body(entity)->mIsDisabled;
}
bool Is_BlockingLineOfSight(Entity entity)
{
	return Is_ComponentPresent(C_TagIsBlockingLineOfSight, entity);
}
bool Is_IgnoringTransitions(Entity entity)
{
	return Is_ComponentPresent(C_TagIsIgnoringTransitions, entity);
}
bool Is_PlayerPresent(bool logWarning)
{
	if (Get_Player() == ENTITY_NOTHING)
	{
		if (logWarning)
		{
			Logger_LogWarning("Player not present.");
		}
		return false;
	}
	else
	{
		return true;
	}
}
bool Is_Player(Entity entity)
{
	return Get_PlayerNumber(entity) != -1;
}
bool Is_OnRightSideOfScreen(Entity entity)
{
	return Get_X(entity) >= Camera_GetCenterX(Get_Camera());
}
bool Is_OnBottomSideOfScreen(Entity entity)
{
	return Get_Y(entity) >= Camera_GetCenterY(Get_Camera());
}
bool Is_NodesPresent(Entity entity)
{
	return Get_AmountOfNodes(entity) != 0;
}
bool DeprecatedIs_TagPresent(Entity entity, int32_t i)
{
	return false;
	/*
	if (Is_ComponentPresent(entity, i))
	{
		return true;
	}
	else
	{
		return false;
	}
	*/
}
bool Is_DrawActorPresent(Entity entity)
{
	return Is_ComponentPresent(C_DrawActor, entity);
}
bool Is_BodyPresent(Entity entity)
{
	if (Is_ComponentPresent(C_Body, entity))
	{
		return true;
	}
	return false;
}
bool Is_Child(Entity entity)
{
	return Get_ParentNumber(entity) != -1;
}
bool Is_Complete(Entity entity)
{
	return Is_ComponentPresent(C_TagIsComplete, entity);
}
bool Is_OnScreen(Entity entity)
{
	return Is_OnScreen2(entity, 1.0f);
}
bool Is_OnScreen2(Entity entity, float mul)
{
	bool wasSuccessful;
	Body* body = (Body*)TryGet_Component(C_Body, entity, &wasSuccessful);
	if (!wasSuccessful)
	{
		Vector2 temp = Get_FakePosition(entity);
		float posX = temp.X - (TILE_SIZE / 2);
		float posY = temp.Y - (TILE_SIZE / 2);
		return Camera_IntersectsCamera(Get_Camera(), (int32_t)(posX), (int32_t)(posY), TILE_SIZE, TILE_SIZE, mul);
	}
	else
	{
		Rectangle bodyRectangle = Body_GetRect(body);
		return Camera_IntersectsCameraRectMul(Get_Camera(), &bodyRectangle, mul);
	}
}
bool Is_OnScreen3(const Rectangle* rect)
{
	return Camera_IntersectsCameraRectMul(Get_Camera(), rect, 1.0f);
}
bool Is_OnScreen4(const Rectangle* rect, float mul)
{
	return Camera_IntersectsCameraRectMul(Get_Camera(), rect, mul);
}
bool Is_OnScreen5(Vector2 pos)
{
	return Is_OnScreen6(pos, 1.0f);
}
bool Is_OnScreen6(Vector2 pos, float mul)
{
	float posX = pos.X - (TILE_SIZE / 2);
	float posY = pos.Y - (TILE_SIZE / 2);
	return Camera_IntersectsCamera(Get_Camera(), (int32_t)posX, (int32_t)posY, TILE_SIZE, TILE_SIZE, mul);
}
bool Is_OnScreenExtended(Entity entity)
{
	return Is_OnScreen2(entity, CAMERA_EXTENDED_CAMERA);
}
bool Is_InitialStringSettingsMapPresent(ComponentType componentType)
{
	return false;
	/*
	if (_mControllerComponentStringSettingsMap.ContainsKey(componentType))
	{
		return true;
	}
	return false;
	*/
}
bool Is_StringSettingsPresent(Entity entity)
{
	StringPair* arr_settings = Get_ArrStringSettings(entity);
	return arrlen(arr_settings) > 0;
}
bool Is_FlipX(Entity entity)
{
	return Get_DrawActor(entity)->mIsFlipX;
}
bool Is_FlipY(Entity entity)
{
	return Get_DrawActor(entity)->mIsFlipY;
}
bool Is_NearCollisionLowerRight(Entity entity, int32_t xDirection, int32_t yDirection, const int32_t* collisionToCheck, int32_t collisionToCheckLen)
{
	Rectangle rect = Body_GetRect(Get_Body(entity));
	return CollisionEngineSys_CheckSurroundingCollision(Get_CollisionEngine(), Rectangle_Right(&rect), Rectangle_Bottom(&rect),
		xDirection, yDirection, collisionToCheck, collisionToCheckLen);
}
bool Is_NearCollisionLowerLeft(Entity entity, int32_t xDirection, int32_t yDirection, const int32_t* collisionToCheck, int32_t collisionToCheckLen)
{
	Rectangle rect = Body_GetRect(Get_Body(entity));
	return CollisionEngineSys_CheckSurroundingCollision(Get_CollisionEngine(), Rectangle_Left(&rect), Rectangle_Bottom(&rect),
		xDirection, yDirection, collisionToCheck, collisionToCheckLen);
}
bool Is_NearCollisionUpperCenter(Entity entity, int32_t xDirection, int32_t yDirection, const int32_t* collisionToCheck, int32_t collisionToCheckLen)
{
	Rectangle rect = Body_GetRect(Get_Body(entity));
	return CollisionEngineSys_CheckSurroundingCollision(Get_CollisionEngine(), Rectangle_Center(&rect).X, Rectangle_Top(&rect),
		xDirection, yDirection, collisionToCheck, collisionToCheckLen);
}
bool Is_NearCollisionLowerCenter(Entity entity, int32_t xDirection, int32_t yDirection, const int32_t* collisionToCheck, int32_t collisionToCheckLen)
{
	Rectangle rect = Body_GetRect(Get_Body(entity));
	return CollisionEngineSys_CheckSurroundingCollision(Get_CollisionEngine(), Rectangle_Center(&rect).X, Rectangle_Bottom(&rect),
		xDirection, yDirection, collisionToCheck, collisionToCheckLen);
}
bool Is_NearCollisionMiddleRight(Entity entity, int32_t xDirection, int32_t yDirection, const int32_t* collisionToCheck, int32_t collisionToCheckLen)
{
	Rectangle rect = Body_GetRect(Get_Body(entity));
	return CollisionEngineSys_CheckSurroundingCollision(Get_CollisionEngine(), Rectangle_Right(&rect), Rectangle_Center(&rect).Y,
		xDirection, yDirection, collisionToCheck, collisionToCheckLen);
}
bool Is_NearCollisionMiddleLeft(Entity entity, int32_t xDirection, int32_t yDirection, const int32_t* collisionToCheck, int32_t collisionToCheckLen)
{
	Rectangle rect = Body_GetRect(Get_Body(entity));
	return CollisionEngineSys_CheckSurroundingCollision(Get_CollisionEngine(), Rectangle_Left(&rect), Rectangle_Center(&rect).Y,
		xDirection, yDirection, collisionToCheck, collisionToCheckLen);
}
bool Is_NearCollisionUpperRight(Entity entity, int32_t xDirection, int32_t yDirection, const int32_t* collisionToCheck, int32_t collisionToCheckLen)
{
	Rectangle rect = Body_GetRect(Get_Body(entity));
	return CollisionEngineSys_CheckSurroundingCollision(Get_CollisionEngine(), Rectangle_Right(&rect), Rectangle_Top(&rect),
		xDirection, yDirection, collisionToCheck, collisionToCheckLen);
}
bool Is_NearCollisionCenter(Entity entity, int32_t xDirection, int32_t yDirection, const int32_t* collisionToCheck, int32_t collisionToCheckLen)
{
	Rectangle rect = Body_GetRect(Get_Body(entity));
	return CollisionEngineSys_CheckSurroundingCollision(Get_CollisionEngine(), Rectangle_Center(&rect).X, Rectangle_Center(&rect).Y,
		xDirection, yDirection, collisionToCheck, collisionToCheckLen);
}
bool Is_NearCollision(int32_t x, int32_t y, int32_t xDirection, int32_t yDirection, const int32_t* collisionToCheck, int32_t collisionToCheckLen)
{
	return CollisionEngineSys_CheckSurroundingCollision(Get_CollisionEngine(), x, y, xDirection, yDirection, collisionToCheck, collisionToCheckLen);
}
bool Is_NearCollisionUpperLeft(Entity entity, int32_t xDirection, int32_t yDirection, const int32_t* collisionToCheck, int32_t collisionToCheckLen)
{
	Rectangle rect = Body_GetRect(Get_Body(entity));
	return CollisionEngineSys_CheckSurroundingCollision(Get_CollisionEngine(), Rectangle_Left(&rect), Rectangle_Top(&rect), 
		xDirection, yDirection, collisionToCheck, collisionToCheckLen);
}
bool Is_LineOfSight(float x1, float y1, float x2, float y2, bool respectOneWays)
{
	return CollisionEngineSys_HasLineOfSight(Get_CollisionEngine(), x1, y1, x2, y2, respectOneWays);
}
bool Is_LineOfSight2(float x1, float y1, float x2, float y2)
{
	return Is_LineOfSight(x1, y1, x2, y2, true);
}
bool Is_InLineOfSightWith(Entity entity, float targetX, float targetY, float offsetX, float offsetY)
{
	return Is_InLineOfSightWith2(entity, targetX, targetY, offsetX, offsetY, true);
}
bool Is_InLineOfSightWith2(Entity entity, float targetX, float targetY, float offsetX, float offsetY, bool respectOneWays)
{
	return Is_LineOfSight(Get_X(entity) + offsetX, Get_Y(entity) + offsetY, targetX, targetY, respectOneWays);
}
bool Is_InLineOfSightWithPlayer(Entity entity)
{
	return Is_InLineOfSightWithPlayer3(entity, 0, 0);
}
bool Is_InLineOfSightWithPlayer2(Entity entity, float offsetX, float offsetY, int32_t limAngle)
{
	float wrappedAngle = (float)(Get_WrappedAngle(Get_AngleToClosestPlayer(entity)));
	float toDegrees = Math_ToDegrees(wrappedAngle);
	int32_t angle = (int32_t)(Math_fabsf(toDegrees));
	if ((Math_abs(180 - angle) < limAngle) || (Math_abs(0 - angle) < limAngle))
	{
		return Is_InLineOfSightWithPlayer3(entity, offsetX, offsetY);
	}
	return false;
}
bool Is_InLineOfSightWithPlayer3(Entity entity, float offsetX, float offsetY)
{
	return Is_InLineOfSightWith(entity, Get_PlayerX(), Get_PlayerY(), offsetX, offsetY);
}
bool Is_PlayerNearBody(Entity entity, int32_t buffer)
{
	if (!Is_PlayerPresent(true))
	{
		return false;
	}

	Rectangle rect1 = Body_GetRect(Get_Body(Get_Player()));
	Body* body = Get_Body(entity);
	Rectangle bodyRect = Body_GetRect(body);
	Rectangle rect2 = Rectangle_Create(Rectangle_Left(&bodyRect) - buffer, Rectangle_Top(&bodyRect) - buffer,
		Body_GetWidth(body) + buffer * 2, Body_GetHeight(body) + buffer * 2);
	return Rectangle_Intersects(&rect1, &rect2);
}
bool Is_ClosestPlayerNearBody(Entity entity, int32_t buffer)
{
	if (!Is_PlayerPresent(true))
	{
		return false;
	}

	Entity closestPlayer = Get_ClosestPlayer2(entity);
	Rectangle rect1 = Body_GetRect(Get_Body(closestPlayer));
	Body* body = Get_Body(entity);
	Rectangle bodyRect = Body_GetRect(body);
	Rectangle rect2 = Rectangle_Create(Rectangle_Left(&bodyRect) - buffer, Rectangle_Top(&bodyRect) - buffer,
		Body_GetWidth(body) + buffer * 2, Body_GetHeight(body) + buffer * 2);
	return Rectangle_Intersects(&rect1, &rect2);
}
bool Is_LeftOfCamera(Entity entity)
{
	Rectangle rect = Body_GetRect(Get_Body(entity));
	if (Rectangle_Center(&rect).X < Camera_GetLeft(Get_Camera()))
	{
		return true;
	}
	return false;
}
bool Is_RightOfCamera(Entity entity)
{
	Rectangle rect = Body_GetRect(Get_Body(entity));
	if (Rectangle_Center(&rect).X > Camera_GetRight(Get_Camera()))
	{
		return true;
	}
	return false;
}
bool Is_TopOfCamera(Entity entity)
{
	Rectangle rect = Body_GetRect(Get_Body(entity));
	if (Rectangle_Center(&rect).Y < Camera_GetTop(Get_Camera()))
	{
		return true;
	}
	return false;
}
bool Is_BottomOfCamera(Entity entity)
{
	Rectangle bodyRect = Body_GetRect(Get_Body(entity));
	if (Rectangle_Center(&bodyRect).Y > Camera_GetBottom(Get_Camera()))
	{
		return true;
	}
	return false;
}
bool Is_UnderCameraHinge(Entity entity)
{
	return Is_UnderCameraHinge2(entity, 0);
}
bool Is_UnderCameraHinge2(Entity entity, int32_t offset)
{
	Rectangle bodyRect = Body_GetRect(Get_Body(entity));
	if (Rectangle_Top(&bodyRect) > Get_CameraHingeBottom() + offset)
	{
		return true;
	}
	return false;
}
bool Is_UnderLevelBounds(Entity entity)
{
	return Is_UnderLevelBounds2(entity, 0);
}
bool Is_UnderLevelBounds2(Entity entity, int32_t offset)
{
	Rectangle bodyRect = Body_GetRect(Get_Body(entity));
	Rectangle levelBoundsRectangle = Get_LevelBoundsRectangle();
	if (Rectangle_Top(&bodyRect) > (Rectangle_Bottom(&levelBoundsRectangle) + offset))
	{
		return true;
	}
	return false;
}
bool Is_UnderCamera(Entity entity)
{
	return Is_UnderCamera2(entity, 0);
}
bool Is_UnderCamera2(Entity entity, int32_t offset)
{
	Rectangle bodyRect = Body_GetRect(Get_Body(entity));
	if (Rectangle_Top(&bodyRect) > Camera_GetBottom(Get_Camera()) + offset)
	{
		return true;
	}
	return false;
}
bool Is_NearPlayerX(Entity entity, int32_t buffer)
{
	if (Get_DistanceToPlayerX(entity) < buffer)
	{
		return true;
	}
	return false;
}
bool Is_NearPlayerY(Entity entity, int32_t buffer)
{
	if (Get_DistanceToPlayerY(entity) < buffer)
	{
		return true;
	}
	return false;
}
bool Is_OnDrawPhase(Entity entity, int32_t state, int32_t phaseToCheck)
{
	if (Get_CurrentImagePhase(entity, state) == phaseToCheck)
	{
		return true;
	}
	return false;
}
bool Is_AnimationComplete(Entity entity, int32_t state, int32_t phase)
{
	return Get_Animation(entity, state, phase)->mIsAnimationComplete;
}
bool Is_ImagePhaseTheSame(Entity entity, int32_t state, int32_t phase)
{
	if (Get_CurrentImagePhase(entity, state) == phase)
	{
		return true;
	}
	return false;
}
bool Is_LevelFrameCountModuloOverHalf(int32_t value)
{
	return Get_LevelFrameCount() % value >= value / 2;
}
bool Is_LevelFrameCountModulo(int32_t value, int32_t target)
{
	return Get_LevelFrameCount() % value == target;
}
bool Is_UpdateDisabled(Entity entity)
{
	return Is_ComponentPresent(C_TagIsUpdateDisabled, entity);
}
bool Is_TaggedAsEnemy(Entity entity)
{
	return Is_ComponentPresent(C_TagIsEnemy, entity);
}
bool Is_DrawDisabled(Entity entity)
{
	return Is_ComponentPresent(C_TagIsDrawDisabled, entity);
}
void* Get_Component(ComponentType ctype, Entity entity)
{
	return GameStateManager_Set(ctype, entity);
}
void* Do_InitComponent(ComponentType ctype, Entity entity)
{
	return GameStateManager_SetAndInit(ctype, entity);
}
Entity Get_ChildByComponent(ComponentType ctype, Entity entity)
{
	EntitySearch* search = Do_SearchForChildrenWithComponent(ctype, entity);
	if (search->len <= 0)
	{
		return ENTITY_NOTHING;
	}
	else
	{
		return search->entities[0];
	}
}
void Do_SetBoolTag(ComponentType ctype, Entity entity, bool value)
{
	if (value)
	{
		Get_Component(ctype, entity);
	}
	else
	{
		Do_RemoveComponent(ctype, entity);
	}
}
void Do_RemoveComponent(ComponentType ctype, Entity entity)
{
	GameStateManager_Unset(ctype, entity);
}
bool Is_ComponentPackPresent(ComponentType ctype)
{
	return true;
	//return GameStateManager_IsComponentPackPresent(ctype);
}
bool Is_ComponentPresent(ComponentType ctype, Entity entity)
{
	return GameStateManager_HasComponent(ctype, entity);
}
ComponentPack* Get_ComponentPack(ComponentType ctype)
{
	return GameStateManager_GetComponentPack(ctype);
}
Entity Get_FirstSetEntity(ComponentType ctype)
{
	return GameStateManager_GetFirstSetEntity(ctype);
}
void* Get_FirstSetComponent(ComponentType ctype)
{
	return GameStateManager_GetFirstSetComponent(ctype);
}
bool Is_AnyEntityInPack(ComponentType ctype)
{
	return GameStateManager_IsAnyEntityInPack(ctype);
}
void* TryGet_Component(ComponentType ctype, Entity entity, bool* wasSuccessful)
{
	return GameStateManager_TryGetComponent(ctype, entity, wasSuccessful);
}
void* TryGet_FirstSetComponent(ComponentType ctype, bool* wasSuccessful)
{
	return GameStateManager_TryGetFirstSetComponent(ctype, wasSuccessful);
}
Entity Get_DummyEntityUniqueToPack(ComponentType ctype, const char* name)
{
	return Get_DummyEntityUniqueToPack2(Get_ComponentPack(ctype), name);
}
Entity Get_DummyEntityUniqueToPack2(ComponentPack* pack, const char* name)
{
	if (!ComponentPack_IsAnyEntityInPack(pack))
	{
		Entity entity = Do_BuildNewEntity();
		Do_SetName(entity, name);
		return entity;
	}
	else
	{
		return ComponentPack_GetFirstSetEntity(pack);
	}
}
void Do_RemoveUniqueDummyEntity(ComponentType ctype)
{
	Do_RemoveUniqueDummyEntity2(Get_ComponentPack(ctype));
}
void Do_RemoveUniqueDummyEntity2(ComponentPack* pack)
{
	if (ComponentPack_IsAnyEntityInPack(pack))
	{
		GameState_RemoveEntity(Get_ActiveGameState(), ComponentPack_GetFirstSetEntity(pack));
	}
}
int32_t Get_AmountOfMyChildrenWithComponent(ComponentType ctype, Entity entity)
{
	EntitySearch* search = Do_SearchForChildrenWithComponent(ctype, entity);
	return search->len;
}
void Do_DestroyChildrenWithComponent(ComponentType ctype, Entity entity)
{
	Do_DestroyChildrenWithComponent2(ctype, entity, EE_STR_EMPTY);
}
void Do_DestroyChildrenWithComponent2(ComponentType ctype, Entity entity, const char* particle)
{
	EntitySearch* search = Do_SearchForChildrenWithComponent(ctype, entity);
	for (int32_t i = 0; i < search->len; i += 1)
	{
		Entity target = search->entities[i];
		if (!Utils_StringEqualTo(particle, EE_STR_EMPTY))
		{
			Do_AddParticle2(particle, Get_Position(target));
		}
		Do_SetComplete(target);
	}
}
void Do_UnsetAtIndexAndRemoveDummyEntityIfLast(ComponentPack* pack, int32_t index)
{
	Entity dummyEntity = pack->Entities[index];
	ComponentPack_UnsetAtIndex(pack, index);
	if (!ComponentPack_IsAnyEntityInPack(pack))
	{
		GameState_RemoveEntity(Get_ActiveGameState(), dummyEntity);
	}
}
EntitySearch* Do_SearchForEntitiesWithComponent(ComponentType ctype)
{
	return Do_SearchForEntitiesWithComponent2(ctype, false);
}
EntitySearch* Do_SearchForEntitiesWithComponent2(ComponentType ctype, bool isReverse)
{
	return EntitySearch_SearchForEntitiesWithThisComponent(ctype, EntitySearch_CreateNewAllEntitySearch(),
		EntitySearch_CreateNewBlankSearch(), isReverse);
}
EntitySearch* Do_SearchForChildrenWithComponent(ComponentType ctype, Entity entity)
{
	return Do_SearchForChildrenWithComponent2(ctype, entity, false);
}
EntitySearch* Do_SearchForChildrenWithComponent2(ComponentType ctype, Entity entity, bool isReverse)
{
	return EntitySearch_SearchForEntitiesWithThisComponent(ctype, Do_SearchForChildren2(entity, isReverse),
		EntitySearch_CreateNewBlankSearch(), isReverse);
}
void Do_DestroyEntitiesWithComponent(ComponentType ctype)
{
	Do_DestroyEntitiesWithComponent2(ctype, EE_STR_EMPTY);
}
void Do_DestroyEntitiesWithComponent2(ComponentType ctype, const char* particle)
{
	EntitySearch* search = Do_SearchForEntitiesWithComponent(ctype);
	for (int32_t i = 0; i < search->len; i += 1)
	{
		Entity target = search->entities[i];
		if (!Utils_StringEqualTo(particle, EE_STR_EMPTY))
		{
			Do_AddParticle2(particle, Get_Position(target));
		}
		Do_SetComplete(target);
	}
}
