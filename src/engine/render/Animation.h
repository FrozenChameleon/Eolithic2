/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "../utils/Timer.h"
#include "../render/Sheet.h"
#include "../utils/FixedArray.h"

typedef struct Animation
{
	int32_t mAnimationLoopPoint;
	int32_t mInitialFlipTimerLimit;
	int32_t mSheetsLen;
	bool mIsSuppressed;
	bool mIsAnimationComplete;
	bool mIsLoopingDisabled;
	bool mIsReverse;
	bool mIsGoingInReverse;
	bool mIsUpdateDisabled;
	Timer mFrameTimer;
	Timer mFlipTimer;
	Sheet** mSheets;
} Animation;

void Animation_Init(Animation* data, const char* spriteSheet, int32_t flipTimer);
void Animation_Init2(Animation* data, Sheet** sheets, int32_t sheetsLen, int32_t flipTimer);

const char* Animation_GetCurrentSheetName(Animation* data);
bool Animation_Update(Animation* data);
void Animation_ResetAnimation(Animation* data);
Sheet* Animation_GetCurrentSheet(Animation* data);
int32_t Animation_DigitsAtEnd(const char* sheetName, int32_t sheetNameLength);
void Animation_CheckForAnimation(const char* textureName);
void Animation_BuildAnimations(void);
void Animation_AddToAnimationMap(const char* baseName, const char* sheetName, int32_t frames, int32_t digits);
void Animation_CreateAnimationStringArray(IStringArray* listToAddTo, const char* baseName, int32_t frames, int32_t digits);
const char* Animation_GetConvertedName(const char* baseName);
FixedArray Animation_GetAnimationSheets(const char* animation);
const Sheet** Animation_GetSheets(const Animation* anim);