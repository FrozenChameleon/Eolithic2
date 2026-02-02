/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"
#include "Achievement.h"

void AchievementHandler_SetIsUpdateSuppressed(bool value);
void AchievementHandler_DebugUnlockNextAchievement(void);
void AchievementHandler_SetToUseMask(void);
void AchievementHandler_Update(void);
Achievement* AchievementHandler_GetAchievements(void);
int32_t AchievementHandler_GetAchievementsLength(void);
Achievement* AchievementHandler_GetAchievement(int32_t i);
int32_t AchievementHandler_GetAmountOfAchievements(void);
void AchievementHandler_AddAchievement(bool isHidden, const char* name, const char* description, const char* lockedDescription, const char* image, bool (*condition)(void));
void AchievementHandler_Reset(void);
int32_t AchievementHandler_GetAmountOfAchievementsComplete(void);
int32_t AchievementHandler_GetIntegerPercentage(void);
