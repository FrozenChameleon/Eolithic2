/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "../render/Sheet.h"
#include "../utils/MString.h"

typedef bool(*Achievement_ConditionFunc);

typedef struct Achievement
{
	bool mIsCertified;
	bool mIsHidden;
	bool mIsUnlocked;
	char mName[EE_FILENAME_MAX];
	char mDescription[EE_FILENAME_MAX];
	char mLockedDescription[EE_FILENAME_MAX];
	bool (*mCondition)(void);
	Sheet* mSheet;
} Achievement;

void Achievement_Init(Achievement* achievement, bool (*condition)(void), const char* name, const char* description, const char* lockedDescription, const char* image, bool isHidden);
void Achievement_Update(bool doNotCertify, Achievement* achievement, int32_t index);
void Achievement_UnlockAchievement(bool doNotCertify, Achievement* achievement, int32_t index);
void Achievement_CertifyAchievement(Achievement* achievement, int32_t index);