/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "SpecialMcFinalBossEffectSys.h"

#include "../leveldata/ParticleInstance.h"
#include "../core/Func.h"
#include "../globals/Globals.h"
#include "../render/Color.h"
#include "../render/DrawTool.h"
#include "../render/Sheet.h"
#include "../input/Input.h"
#include "../input/InputPlayer.h"
#include "../utils/Utils.h"

#define BLOOD_SPRAY_DENSITY 100
#define FINAL_BOSS_DEATH 777

static void InitAfterInit(SpecialMcFinalBossEffect* data)
{
	Animation_Init(&data->mAnimation, data->mAnimationName, 5);
	Animation_ResetAnimation(&data->mAnimation);

	data->mTimerLine.mLimit = 5;
	data->mTimerBleed.mLimit = 4;
}
static void DrawRoutine(Entity owner, SpecialMcFinalBossEffect* data, SpriteBatch* spriteBatch)
{
	Sheet* sheet = Animation_GetCurrentSheet(&data->mAnimation);

	Rectangle rect = sheet->mRectangle;

	int32_t width = rect.Width;
	int32_t height = rect.Height;

	if (height >= data->mLineCounter)
	{
		Sheet_DrawSourceRect(sheet, spriteBatch, COLOR_WHITE, 80, true, NULL,
			Vector2_Create(data->mPosition.X - (width / 4) + data->mRandomOffset.X, data->mPosition.Y - data->mLineCounter + data->mRandomOffset.Y),
			Rectangle_Create(rect.X, rect.Y, width, height - data->mLineCounter), 2, 0, false, false); //Old depth was 95
	}
}
static void UpdateRoutine(Entity owner, SpecialMcFinalBossEffect* data)
{
	if (!data->mDisableShaking)
	{
		Random32* sharedRandom = Globals_GetSharedRandom();
		data->mRandomOffset.X = Random32_NextInt(sharedRandom, 6) * Get_RandomBinaryDirection(sharedRandom);
		data->mRandomOffset.Y = Random32_NextInt(sharedRandom, 6) * Get_RandomBinaryDirection(sharedRandom);
	}

	Rectangle rect = Animation_GetCurrentSheet(&data->mAnimation)->mRectangle;
	int32_t height = rect.Height;
	int32_t cutoff = data->mBloodCutoff;

	if (height - cutoff > data->mLineCounter)
	{
		if (Timer_Update(&data->mTimerBleed))
		{
			for (int32_t i = 0; i < BLOOD_SPRAY_DENSITY; i += 1)
			{
				ParticleInstance* instance = Do_AddParticle4("FINALBLOODSPLOSION", 
					data->mPosition.X + data->mBloodOffset.X, data->mPosition.Y + data->mBloodOffset.Y);
				instance->mInfluencedDepth = 100;
				if (data->mForcedDirection == 1)
				{
					instance->mInfluencedDirection = Point_Create(-1, 1);
				}
				else if (data->mForcedDirection == -1)
				{
					instance->mInfluencedDirection = Point_Create(1, 1);
				}
				else
				{
					instance->mInfluencedDirection = Point_Create(Get_RandomBinaryDirection(Globals_GetSharedRandom()), 1);
				}
			}
		}
	}

	Animation_Update(&data->mAnimation);

	if (Timer_Update(&data->mTimerLine))
	{
		data->mLineCounter += 2;
	}

	if (height < data->mLineCounter)
	{
		Do_SendBroadcast(FINAL_BOSS_DEATH); //Level Clear Routine
	}
	else
	{
		float percent;
		if (height - cutoff > data->mLineCounter)
		{
			percent = data->mLineCounter / (float)height;
			InputPlayer_Vibrate(Input_GetPlayerOne(), 999, 2, percent, percent);
		}
		else
		{
			int32_t temp = height - data->mLineCounter;
			percent = temp / (float)cutoff;
		}
		InputPlayer_Vibrate(Input_GetPlayerOne(), 999, 2, percent, percent);
	}

	if (!data->mDisableShaking)
	{
		Do_ShakeCamera(1, 2, 1, 2);
	}
}
static void DrawHudRoutine(Entity owner, SpecialMcFinalBossEffect* data, SpriteBatch* spriteBatch)
{
	if (data->mLineCounter == 0)
	{
		DrawTool_DrawRectangle2(spriteBatch, COLOR_WHITE, 100, Rectangle_Create(0, 0, 1280, 720), 0, false);
	}
}

void SpecialMcFinalBossEffectSys_Create(Vector2 position, const char* animationName, Vector2 bloodOffset, bool disableShaking, 
	int32_t bloodCutoff, int32_t forcedDirection)
{
	Entity entity = Do_BuildNewEntityWithName("MC Final Boss Effect");
	Do_SetPosition(entity, position.X, position.Y);
	SpecialMcFinalBossEffect* component = (SpecialMcFinalBossEffect*)Do_InitComponent(C_SpecialMcFinalBossEffect, entity);
	component->mPosition = position;
	Utils_strlcpy(component->mAnimationName, animationName, EE_FILENAME_MAX);
	component->mBloodOffset = bloodOffset;
	component->mDisableShaking = disableShaking;
	component->mBloodCutoff = bloodCutoff;
	component->mForcedDirection = forcedDirection;
	InitAfterInit(component);
}

System* SpecialMcFinalBossEffectSys_CreateSystem(void)
{
	SystemSimple* ss = SystemSimple_Create(C_SpecialMcFinalBossEffect);
	ss->_mUpdateRoutine = (SystemSimple_UpdateFunc)UpdateRoutine;
	ss->_mDrawRoutine = (SystemSimple_DrawFunc)DrawRoutine;
	ss->_mDrawHudRoutine = (SystemSimple_DrawFunc)DrawHudRoutine;
	return SystemSimple_CreateSystem(ss);
}
