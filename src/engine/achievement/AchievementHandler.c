/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "AchievementHandler.h"

#include "../../third_party/stb_ds.h"
#include "../utils/Utils.h"

static int32_t _mCertifyCrawlingCounter;
static bool _mIsUsingMask;
static bool _mIsNotFirstUpdate;
static bool _mIsUpdateSuppressed;
static Achievement* arr_achievements;

void AchievementHandler_SetIsUpdateSuppressed(bool value)
{
	_mIsUpdateSuppressed = value;
}
void AchievementHandler_DebugUnlockNextAchievement(void)
{
	int32_t len = AchievementHandler_GetAchievementsLength();
	for (int32_t i = 0; i < len; i += 1)
	{
		Achievement* achievement = &arr_achievements[i];
		if (!achievement->mIsUnlocked)
		{
			Achievement_UnlockAchievement(false, achievement, i);
			return;
		}
	}
}
void AchievementHandler_SetToUseMask(void)
{
	_mIsUsingMask = true;
}
void AchievementHandler_Update(void)
{

}
Achievement* AchievementHandler_GetAchievements(void)
{
	return arr_achievements;
}
Achievement* AchievementHandler_GetAchievement(int32_t i)
{
	return &arr_achievements[i];
}
int32_t AchievementHandler_GetAchievementsLength(void)
{
	return (int32_t)arrlen(arr_achievements);
}
int32_t AchievementHandler_GetAmountOfAchievements(void)
{
	return AchievementHandler_GetAchievementsLength();
}
void AchievementHandler_AddAchievement(bool isHidden, const char* name, const char* description, const char* lockedDescription, const char* image, bool (*condition)(void))
{
	int32_t current = AchievementHandler_GetAchievementsLength();
	MString* nameToUse = NULL;
	MString_AssignString(&nameToUse, name);
	MString* descriptionToUse = NULL;
	MString_AssignString(&descriptionToUse, description);
	description;
	if (_mIsUsingMask)
	{
		MString_Combine3(&nameToUse, "ACHIEVEMENT_", Utils_IntToStringGlobalBuffer(current), "_NAME");
		MString_Combine3(&descriptionToUse, "ACHIEVEMENT_", Utils_IntToStringGlobalBuffer(current), "_DESCRIPTION");
	}
	Achievement achievement;
	Achievement_Init(&achievement, condition, name, description, lockedDescription, image, isHidden);
	arrput(arr_achievements, achievement);
	MString_Dispose(&nameToUse);
	MString_Dispose(&descriptionToUse);
}
void AchievementHandler_Reset(void)
{
	int32_t len = AchievementHandler_GetAchievementsLength();
	for (int32_t i = 0; i < len; i += 1)
	{
		arr_achievements[i].mIsUnlocked = false;
	}
}
int32_t AchievementHandler_GetAmountOfAchievementsComplete(void)
{
	int32_t currentAchievementsDone = 0;

	int32_t len = AchievementHandler_GetAchievementsLength();
	for (int32_t i = 0; i < len; i += 1)
	{
		if (arr_achievements[i].mIsUnlocked)
		{
			currentAchievementsDone += 1;
		}
	}

	return currentAchievementsDone;
}
int32_t AchievementHandler_GetIntegerPercentage(void)
{
	float temp = (float)AchievementHandler_GetAmountOfAchievementsComplete() / AchievementHandler_GetAmountOfAchievements();
	float value = temp * 100.0f;
	return (int32_t)value;
}