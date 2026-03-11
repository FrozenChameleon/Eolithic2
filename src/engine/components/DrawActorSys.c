/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "DrawActorSys.h"

#include "../utils/Macros.h"
#include "../core/Func.h"
#include "../leveldata/ImageData.h"
#include "../render/DrawRenderInfo.h"
#include "../render/DrawStateInfo.h"
#include "../render/Color.h"
#include "../globals/OeState.h"
#include "../globals/OePhase.h"
#include "../components/Camera.h"
#include "../utils/Utils.h"
#include "../components/TagIsUpdateDisabled.h"
#include "../components/FakePosition.h"
#include "../../third_party/stb_ds.h"
#include "../utils/Logger.h"
#include "../render/BlendState.h"

#define PHASE_NOTHING EE_STR_EMPTY

static Animation EmptyAnimation;
static ImageDataInstance EmptyRender;

static ComponentPack* GetDrawStateInfos(void)
{
	return Get_ComponentPack(C_DrawStateInfo);
}
static ComponentPack* GetDrawRenderInfos(void)
{
	return Get_ComponentPack(C_DrawRenderInfo);
}

void DrawActorSys_Setup(Entity owner, DrawActor* data, ThingGraphicsData* sh_graphics_data, const char* defaultDrawState, const char* defaultDrawPhase)
{
	ComponentPack* stateInfos = GetDrawStateInfos(); //DrawStateInfo
	ComponentPack* renderInfos = GetDrawRenderInfos(); //OeDrawRenderInfo

	Utils_strlcpy(data->mDefaultDrawState, defaultDrawState, EE_FILENAME_MAX);
	Utils_strlcpy(data->mDefaultDrawPhase, defaultDrawPhase, EE_FILENAME_MAX);

	for (int32_t i = 0; i < shlen(sh_graphics_data); i += 1)
	{
		const char* stateString = sh_graphics_data[i].key;

		DrawStateInfo* stateInfo = (DrawStateInfo*)ComponentPack_Set2(stateInfos, owner, true);
		Utils_strlcpy(stateInfo->mState, stateString, EE_FILENAME_MAX);
		Utils_strlcpy(stateInfo->mCurrentPhase, PHASE_NOTHING, EE_FILENAME_MAX);
		stateInfo->mDepth = -1;

		ThingGraphicsEntry* sh_thing_graphics_entries = sh_graphics_data[i].value;
		for (int32_t j = 0; j < shlen(sh_thing_graphics_entries); j += 1)
		{
			const char* phaseString = sh_thing_graphics_entries[j].key;

			ImageData* arr_images = sh_thing_graphics_entries[j].value;
			for (int32_t k = 0; k < arrlen(arr_images); k += 1)
			{
				DrawRenderInfo* renderInfo = (DrawRenderInfo*)ComponentPack_Set2(renderInfos, owner, true);
				Utils_strlcpy(renderInfo->mState, stateString, EE_FILENAME_MAX);
				Utils_strlcpy(renderInfo->mPhase, phaseString, EE_FILENAME_MAX);
				ImageData* imageData = &arr_images[k];
				ImageDataInstance_Init(&renderInfo->mRender, imageData);
			}
		}
	}

	DrawActorSys_SetImageState(owner, data, data->mDefaultDrawState, data->mDefaultDrawPhase);
}
void DrawActorSys_InitRoutine(Entity owner, DrawActor* data)
{
	data->mUniversalDepthOverride = -1;
	data->mScale.X = 1;
	data->mScale.Y = 1;
	data->mTintColor = COLOR_WHITE;
	data->mOnScreenCheckMul = CAMERA_EXTENDED_CAMERA;
}
void DrawActorSys_UpdateLastRenderPositionRoutine(DrawActor* data)
{
	data->mLastRenderPosition = data->mRenderPosition;
}
static void RealUpdateRoutine(Entity owner, DrawActor* data, Camera* camera,
	ComponentPack* bodyPack, ComponentPack* isUpdateDisabledPack, ComponentPack* fakePositionPack,
	ComponentPack* stateInfos, ComponentPack* renderInfos)
{
	bool wasSuccessful;
	Body* body = (Body*)ComponentPack_TryGetComponent(bodyPack, owner, &wasSuccessful);
	if (wasSuccessful)
	{
		Rectangle bodyRect = Body_GetRect(body);
		data->mIsOnScreen = Camera_IntersectsCameraRectMul(camera, &bodyRect, data->mOnScreenCheckMul);
		data->mRenderPosition = Body_GetPosition(body);
		data->mLastRenderPosition = Body_GetLastRenderPosition(body);
	}
	else
	{
		FakePosition* fakePos = (FakePosition*)ComponentPack_TryGetComponent(fakePositionPack, owner, &wasSuccessful);
		Vector2 temp;
		if (wasSuccessful)
		{
			temp = fakePos->mFakePosition;
		}
		else
		{
			temp = Vector2_Zero;
		}
		float posX = temp.X - (TILE_SIZE / 2);
		float posY = temp.Y - (TILE_SIZE / 2);
		data->mIsOnScreen = Camera_IntersectsCamera(camera, (int32_t)(posX), (int32_t)(posY), TILE_SIZE, TILE_SIZE, data->mOnScreenCheckMul);
		data->mRenderPosition = temp;
		data->mLastRenderPosition = temp;
	}

	if (!data->mIsUpdateDisabled)
	{
		if (!ComponentPack_HasComponent(isUpdateDisabledPack, owner))
		{
			DrawActorSys_UpdateRenders(owner, data, stateInfos, renderInfos);
		}
	}
}
void DrawActorSys_UpdateRoutine(Entity owner, DrawActor* data)
{
	RealUpdateRoutine(owner, data, Get_Camera(), Get_ComponentPack(C_Body),
		Get_ComponentPack(C_TagIsUpdateDisabled), Get_ComponentPack(C_FakePosition), GetDrawStateInfos(), GetDrawRenderInfos());
}
void DrawActorSys_BeforeUpdateRoutine(void)
{
	ComponentPack* drawActorPack = Get_ComponentPack(C_DrawActor);
	if (!ComponentPack_IsAnyEntityInPack(drawActorPack))
	{
		return;
	}

	Camera* camera = Get_Camera();
	ComponentPack* bodyPack = Get_ComponentPack(C_Body);
	ComponentPack* isUpdateDisabledPack = Get_ComponentPack(C_TagIsUpdateDisabled);
	ComponentPack* fakePositionPack = Get_ComponentPack(C_FakePosition);
	ComponentPack* stateInfos = GetDrawStateInfos();
	ComponentPack* renderInfos = GetDrawRenderInfos();
	PackIterator iter = PackIterator_Begin;
	while (ComponentPack_Next(drawActorPack, &iter))
	{
		DrawActor* component = (DrawActor*)iter.mComponent;
		RealUpdateRoutine(iter.mEntity, component, camera, bodyPack, isUpdateDisabledPack, fakePositionPack, stateInfos, renderInfos);
	}
}
DrawStateInfo* DrawActorSys_GetStateInfo(Entity owner, const char* state)
{
	ComponentPack* drawStateInfos = GetDrawStateInfos();
	for (int32_t i = 0; i < ComponentPack_Length(drawStateInfos); i += 1)
	{
		if (drawStateInfos->Entities[i] == owner)
		{
			DrawStateInfo* component = (DrawStateInfo*)ComponentPack_GetComponentAtIndex(drawStateInfos, i);
			if (Utils_StringEqualTo(component->mState, state))
			{
				return component;
			}
		}
	}

	{
		MString* tempString = NULL;
		MString_Combine4(&tempString, "State Not Available: ", state, " on ", Get_Name(owner));
		Logger_Log(LOGGER_ERROR, MString_Text(tempString));
		MString_Dispose(&tempString);
	}

	return (DrawStateInfo*)ComponentPack_GetComponentAtIndex(drawStateInfos, 0);
}
void DrawActorSys_SetDepthOverride(Entity owner, const char* state, int32_t value)
{
	DrawActorSys_GetStateInfo(owner, state)->mDepth = value;
}
void DrawActorSys_SetImageDataDepth(Entity owner, const char* state, const char* phase, int32_t depth)
{
	ComponentPack* drawRenderInfos = GetDrawRenderInfos();
	for (int32_t i = 0; i < ComponentPack_Length(drawRenderInfos); i += 1)
	{
		if (drawRenderInfos->Entities[i] == owner)
		{
			DrawRenderInfo* component = (DrawRenderInfo*)ComponentPack_GetComponentAtIndex(drawRenderInfos, i);
			if ((Utils_StringEqualTo(component->mState, state)) && (Utils_StringEqualTo(component->mPhase, phase)))
			{
				component->mRender.mData->mDepth = depth;
			}
		}
	}
}
void DrawActorSys_SetImageState(Entity owner, DrawActor* data, const char* state, const char* phase)
{
	DrawActorSys_SetImageState2(owner, data, state, phase, false);
}
void DrawActorSys_SetImageState2(Entity owner, DrawActor* data, const char* state, const char* newPhase, bool carryAnimationState)
{
	const char* oldPhase = DrawActorSys_GetCurrentPhase(owner, state);
	if ((Utils_StringEqualTo(oldPhase, PHASE_NOTHING)) && (Utils_StringEqualTo(newPhase, PHASE_NOTHING)))
	{
		return;
	}

	Timer oldFrameTimer = Timer_Zero;
	Timer oldFlipTimer = Timer_Zero;
	bool oldIsReverse = false;

	bool isFirst = true;
	ComponentPack* drawRenderInfos = GetDrawRenderInfos();
	for (int32_t i = 0; i < ComponentPack_Length(drawRenderInfos); i += 1)
	{
		if (drawRenderInfos->Entities[i] == owner)
		{
			DrawRenderInfo* component = (DrawRenderInfo*)ComponentPack_GetComponentAtIndex(drawRenderInfos, i);
			if ((Utils_StringEqualTo(component->mState, state)) && (Utils_StringEqualTo(component->mPhase, oldPhase)))
			{
				ImageDataInstance* oldRender = &component->mRender;
				if (oldRender->mData->mCanAnimate)
				{
					if (isFirst)
					{
						if (carryAnimationState) //setup carry
						{
							Animation* oldAnimation = &oldRender->mAnimation;
							oldFrameTimer = oldAnimation->mFrameTimer;
							oldFlipTimer = oldAnimation->mFlipTimer;
							oldIsReverse = oldAnimation->mIsReverse;
						}
						isFirst = false;
					}
					Animation_ResetAnimation(&oldRender->mAnimation);
				}
			}
		}
	}

	if (carryAnimationState)
	{
		isFirst = true;
		for (int32_t i = 0; i < ComponentPack_Length(drawRenderInfos); i += 1)
		{
			if (drawRenderInfos->Entities[i] == owner)
			{
				DrawRenderInfo* component = (DrawRenderInfo*)ComponentPack_GetComponentAtIndex(drawRenderInfos, i);
				if ((Utils_StringEqualTo(component->mState, state)) && (Utils_StringEqualTo(component->mPhase, newPhase)))
				{
					if (isFirst)
					{
						ImageDataInstance* newRender = &component->mRender;
						Animation* newAnimation = &newRender->mAnimation;
						Timer* newFrameTimer = &newAnimation->mFrameTimer;
						Timer* newFlipTimer = &newAnimation->mFlipTimer;
						newFrameTimer->mCurrent = oldFrameTimer.mCurrent;
						if (oldFlipTimer.mLimit == newFlipTimer->mLimit)
						{
							newFlipTimer->mCurrent = oldFlipTimer.mCurrent;
						}
						else
						{
							float tempTime = oldFlipTimer.mCurrent * ((float)(newFlipTimer->mLimit) / oldFlipTimer.mLimit);
							newFlipTimer->mCurrent = (int32_t)(tempTime);
						}
						newAnimation->mIsReverse = oldIsReverse;
						isFirst = false;
					}
				}
			}
		}
	}

	Utils_strlcpy(DrawActorSys_GetStateInfo(owner, state)->mCurrentPhase, newPhase, EE_FILENAME_MAX); //set current phase
}
void DrawActorSys_SetStateRotation(Entity owner, const char* state, float rotation)
{
	DrawActorSys_GetStateInfo(owner, state)->mRotation = rotation;
}
const char* DrawActorSys_GetCurrentPhase(Entity owner, const char* state)
{
	return DrawActorSys_GetStateInfo(owner, state)->mCurrentPhase;
}
Vector2 DrawActorSys_GetNudge(Entity owner, const char* state)
{
	return DrawActorSys_GetStateInfo(owner, state)->mNudge;
}
void DrawActorSys_SetNudge(Entity owner, const char* state, float x, float y)
{
	DrawActorSys_GetStateInfo(owner, state)->mNudge = Vector2_Create(x, y);
}
ImageDataInstance* DrawActorSys_GetRender(Entity owner, const char* state, const char* phase, int32_t index)
{
	int32_t counter = 0;
	ComponentPack* drawRenderInfos = GetDrawRenderInfos();
	for (int32_t i = 0; i < ComponentPack_Length(drawRenderInfos); i += 1)
	{
		if (drawRenderInfos->Entities[i] == owner)
		{
			DrawRenderInfo* component = (DrawRenderInfo*)ComponentPack_GetComponentAtIndex(drawRenderInfos, i);
			if ((Utils_StringEqualTo(component->mState, state)) && (Utils_StringEqualTo(component->mPhase, phase)))
			{
				if (index == counter)
				{
					return &component->mRender;
				}
				else
				{
					counter += 1;
				}
			}
		}
	}
	return &EmptyRender;
}
int32_t DrawActorSys_GetAnimationPosition(Entity owner, const char* state, const char* phase)
{
	return DrawActorSys_GetAnimation(owner, state, phase)->mFrameTimer.mCurrent;
}
Animation* DrawActorSys_GetCurrentAnimation(Entity owner, const char* state)
{
	return DrawActorSys_GetAnimation(owner, state, DrawActorSys_GetCurrentPhase(owner, state));
}
Animation* DrawActorSys_GetAnimation(Entity owner, const char* state, const char* phase)
{
	ComponentPack* drawRenderInfos = GetDrawRenderInfos();
	for (int32_t i = 0; i < ComponentPack_Length(drawRenderInfos); i += 1)
	{
		if (drawRenderInfos->Entities[i] == owner)
		{
			DrawRenderInfo* component = (DrawRenderInfo*)ComponentPack_GetComponentAtIndex(drawRenderInfos, i);
			if ((Utils_StringEqualTo(component->mState, state)) && (Utils_StringEqualTo(component->mPhase, phase)))
			{
				if (component->mRender.mData->mCanAnimate)
				{
					return &component->mRender.mAnimation;
				}
			}
		}
	}

	{
		MString* tempString = MString_CreateForJustThisFrame();
		MString_Combine5(&tempString, "Unable to get Animation: ", state, ",", phase, "!");
		Logger_Log(LOGGER_ERROR, MString_Text(tempString));
	}

	return &EmptyAnimation;
}
ImageDataInstance* DrawActorSys_GetCurrentImageDataRender(Entity owner, const char* state, const char* phase)
{
	ComponentPack* drawRenderInfos = GetDrawRenderInfos();
	for (int32_t i = 0; i < ComponentPack_Length(drawRenderInfos); i += 1)
	{
		if (drawRenderInfos->Entities[i] == owner)
		{
			DrawRenderInfo* component = (DrawRenderInfo*)ComponentPack_GetComponentAtIndex(drawRenderInfos, i);
			if (Utils_StringEqualTo(component->mState, state) && Utils_StringEqualTo(component->mPhase, phase))
			{
				return &component->mRender;
			}
		}
	}

	{
		MString* tempString = MString_CreateForJustThisFrame();
		MString_Combine5(&tempString, "Unable to get Image Data Render: ", state,",", phase, "!");
		Logger_Log(LOGGER_ERROR, MString_Text(tempString));
	}

	return &EmptyRender;
}
void DrawActorSys_ResetCurrentAnimation(Entity owner, const char* state)
{
	const char* currentPhase = DrawActorSys_GetCurrentPhase(owner, state);
	ComponentPack* drawRenderInfos = GetDrawRenderInfos();
	for (int32_t i = 0; i < ComponentPack_Length(drawRenderInfos); i += 1)
	{
		if (drawRenderInfos->Entities[i] == owner)
		{
			DrawRenderInfo* component = (DrawRenderInfo*)ComponentPack_GetComponentAtIndex(drawRenderInfos, i);
			if ((Utils_StringEqualTo(component->mState, state)) && (Utils_StringEqualTo(component->mPhase, currentPhase)))
			{
				if (component->mRender.mData->mCanAnimate)
				{
					Animation_ResetAnimation(&component->mRender.mAnimation);
				}
			}
		}
	}
}
void DrawActorSys_SetCurrentAnimationPosition(Entity owner, const char* state, int32_t position)
{
	const char* currentPhase = DrawActorSys_GetCurrentPhase(owner, state);
	ComponentPack* drawRenderInfos = GetDrawRenderInfos();
	for (int32_t i = 0; i < ComponentPack_Length(drawRenderInfos); i += 1)
	{
		if (drawRenderInfos->Entities[i] == owner)
		{
			DrawRenderInfo* component = (DrawRenderInfo*)ComponentPack_GetComponentAtIndex(drawRenderInfos, i);
			if ((Utils_StringEqualTo(component->mState, state)) && (Utils_StringEqualTo(component->mPhase, currentPhase)))
			{
				if (component->mRender.mData->mCanAnimate)
				{
					component->mRender.mAnimation.mFrameTimer.mCurrent = position;
				}
			}
		}
	}
}
void DrawActorSys_SetAnimationPosition(Entity owner, const char* state, const char* phase, int32_t position)
{
	ComponentPack* drawRenderInfos = GetDrawRenderInfos();
	for (int32_t i = 0; i < ComponentPack_Length(drawRenderInfos); i += 1)
	{
		if (drawRenderInfos->Entities[i] == owner)
		{
			DrawRenderInfo* component = (DrawRenderInfo*)ComponentPack_GetComponentAtIndex(drawRenderInfos, i);
			if ((Utils_StringEqualTo(component->mState, state)) && (Utils_StringEqualTo(component->mPhase, phase)))
			{
				if (component->mRender.mData->mCanAnimate)
				{
					component->mRender.mAnimation.mFrameTimer.mCurrent = position;
					return;
				}
			}
		}
	}
}
void DrawActorSys_SetShaderProgram(Entity owner, const char* state, ShaderProgram* shaderProgram)
{
	DrawActorSys_GetStateInfo(owner, state)->mShader = shaderProgram;
}
void DrawActorSys_UpdateRenders(Entity owner, DrawActor* data,
	ComponentPack* drawStateInfos, ComponentPack* drawRenderInfos)
{
	int32_t stateInfosLen = ComponentPack_Length(drawStateInfos);
	for (int32_t i = 0; i < stateInfosLen; i += 1)
	{
		if (drawStateInfos->Entities[i] != owner)
		{
			continue;
		}

		DrawStateInfo* stateInfo = (DrawStateInfo*)ComponentPack_GetComponentAtIndex(drawStateInfos, i);
		const char* phase = stateInfo->mCurrentPhase;
		if (Utils_StringEqualTo(phase, PHASE_NOTHING))
		{
			continue;
		}
		const char* state = stateInfo->mState;

		int32_t rendersLen = ComponentPack_Length(drawRenderInfos);
		for (int32_t j = 0; j < rendersLen; j += 1)
		{
			if (drawRenderInfos->Entities[j] != owner)
			{
				continue;
			}

			DrawRenderInfo* renderInfo = (DrawRenderInfo*)ComponentPack_GetComponentAtIndex(drawRenderInfos, j);
			if ((Utils_StringEqualTo(renderInfo->mState, state)) && (Utils_StringEqualTo(renderInfo->mPhase, phase)))
			{
				ImageDataInstance* render = &renderInfo->mRender;
				if (render->mData->mCanAnimate)
				{
					Animation_Update(&render->mAnimation);
					if (render->mData->mAnimationBlanks && render->mAnimation.mIsAnimationComplete)
					{
						DrawActorSys_SetImageState(owner, data, state, PHASE_NOTHING);
					}
				}
			}
		}
	}
}
void DrawActorSys_DrawRoutine(Entity owner, DrawActor* data, SpriteBatch* spriteBatch)
{
	DrawActorSys_DrawInterpolated(owner, data, spriteBatch, data->mTintColor, data->mRenderPosition, data->mLastRenderPosition, data->mRotation,
		data->mScale, data->mUniversalDepthOverride);
}
void DrawActorSys_DrawInterpolated(Entity owner, DrawActor* data, SpriteBatch* spriteBatch,
	Color color, Vector2 currentPosition, Vector2 lastPosition, float rotation, Vector2 scale, int32_t givenDepth)
{
	if (!data->mIsOnScreen)
	{
		return;
	}

	ComponentPack* drawStateInfos = GetDrawStateInfos();
	for (int32_t i = 0; i < ComponentPack_Length(drawStateInfos); i += 1)
	{
		if (drawStateInfos->Entities[i] == owner)
		{
			DrawStateInfo* stateInfo = (DrawStateInfo*)ComponentPack_GetComponentAtIndex(drawStateInfos, i);
			const char* state = stateInfo->mState;
			const char* phase = stateInfo->mCurrentPhase;

			if (Utils_StringEqualTo(phase, PHASE_NOTHING))
			{
				continue;
			}

			ComponentPack* drawRenderInfos = GetDrawRenderInfos();
			for (int32_t j = 0; j < ComponentPack_Length(drawRenderInfos); j += 1)
			{
				if (drawRenderInfos->Entities[j] == owner)
				{
					DrawRenderInfo* component = (DrawRenderInfo*)ComponentPack_GetComponentAtIndex(drawRenderInfos, j);
					if ((Utils_StringEqualTo(component->mState, state)) && (Utils_StringEqualTo(component->mPhase, phase)))
					{
						ImageDataInstance* render = &component->mRender;

						int32_t depthOverride = givenDepth;
						if (depthOverride == -1)
						{
							depthOverride = stateInfo->mDepth;
						}

						Vector2 nudge = Vector2_Add(stateInfo->mNudge, data->mUniversalNudge);
						Vector2 oldPos = Vector2_Add(lastPosition, nudge);
						Vector2 newPos = Vector2_Add(currentPosition, nudge);

						float stateRotation = stateInfo->mRotation;

						ShaderProgram* shaderProgram = data->mShaderProgram;
						if (shaderProgram == NULL)
						{
							shaderProgram = stateInfo->mShader;
						}

						RenderCommandSheet* drawInstance = ImageDataInstance_DrawInterpolated2(render, spriteBatch, color, shaderProgram,
							newPos, oldPos, scale, rotation + stateRotation, data->mIsFlipX,
							data->mIsFlipY, depthOverride, data->mOffset);
						if (data->mExtraPasses != 0)
						{
							drawInstance->mExtraPasses = data->mExtraPasses;
						}
						if (data->mIsBlendStateAdditive)
						{
							drawInstance->mBlendState = BLENDSTATE_ADDITIVE;
						}
					}
				}
			}
		}
	}
}

System* DrawActorSys_CreateSystem(void)
{
	SystemSimple* ss = SystemSimple_Create(C_DrawActor);
	ss->_mInitRoutine = (SystemSimple_InitFunc)DrawActorSys_InitRoutine;
	ss->_mUpdateLastRenderPositionRoutine = (SystemSimple_UpdateLastRenderPositionFunc)DrawActorSys_UpdateLastRenderPositionRoutine;
	ss->_mBeforeUpdateRoutine = (SystemSimple_BeforeUpdateFunc)DrawActorSys_BeforeUpdateRoutine;
	ss->_mDrawRoutine = (SystemSimple_DrawFunc)DrawActorSys_DrawRoutine;
	return SystemSimple_CreateSystem(ss);
}
