/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Animation.h"

#include "../../third_party/stb_ds.h"
#include "AnimationInfo.h"
#include "../utils/Logger.h"
#include "../utils/Utils.h"
#include "Sheet.h"
#include "../math/Math.h"
#include "../utils/MString.h"
#include "../utils/IStrings.h"

static struct { const char* key; IStringArray* value; }*sh_animation_string_map;
static struct { const char* key; const char* value; }*sh_conversion_map;
static struct { const char* key; FixedArray value; }*sh_animation_sheet_map;
static AnimationInfo* arr_animation_info;
static Sheet** _mDummyAnimationSheets;

//private
static void Animation_SetupSheets(Animation* data, int32_t flipTimer)
{
	data->mIsSuppressed = true;

	Timer_Reset(&data->mFlipTimer);
	data->mFlipTimer.mLimit = flipTimer;

	data->mInitialFlipTimerLimit = data->mFlipTimer.mLimit;

	data->mFrameTimer.mLimit = data->mSheetsLen - 1;
}
static IStringArray* Animation_GetAnimationSheetNames(const char* animation)
{
	int64_t index = shgeti(sh_animation_string_map, animation);
	if (index >= 0)
	{
		return sh_animation_string_map[index].value;
	}

	return NULL;
}
static char Animation_GetNumber(int32_t i)
{
	switch (i)
	{
	case 0:
		return '0';
	case 1:
		return '1';
	case 2:
		return '2';
	case 3:
		return '3';
	case 4:
		return '4';
	case 5:
		return '5';
	case 6:
		return '6';
	case 7:
		return '7';
	case 8:
		return '8';
	case 9:
		return '9';
	}
	return 'N';
}

//public
void Animation_Init(Animation* data, const char* spriteSheet, int32_t flipTimer)
{
	Utils_memset(data, 0, sizeof(Animation));

	if (Utils_strlen(spriteSheet) <= 0)
	{
		return;
	}

	FixedArray sheetsArray = Animation_GetAnimationSheets(spriteSheet);
	data->mSheets = (Sheet**)sheetsArray.data;
	data->mSheetsLen = sheetsArray.len;

	if (data->mSheetsLen > 0)
	{
		Animation_SetupSheets(data, flipTimer);
	}
	else
	{
		Logger_LogError("Unable to setup animation:");
		Logger_LogError(spriteSheet);
	}
}
void Animation_Init2(Animation* data, Sheet** sheets, int32_t sheetsLen, int32_t flipTimer)
{
	Utils_memset(data, 0, sizeof(Animation));

	data->mSheets = sheets;
	data->mSheetsLen = sheetsLen;
	Animation_SetupSheets(data, flipTimer);
}
const char* Animation_GetCurrentSheetName(Animation* data)
{
	return Animation_GetCurrentSheet(data)->mSheetName;
}
bool Animation_Update(Animation* data)
{
	if (data->mIsUpdateDisabled)
	{
		return true;
	}

	if (!data->mIsSuppressed)
	{
		if (Timer_Update(&data->mFlipTimer))
		{
			if (!data->mIsGoingInReverse)
			{
				data->mFrameTimer.mCurrent += 1;
			}
			else
			{
				data->mFrameTimer.mCurrent -= 1;
			}
		}
	}
	else
	{
		data->mIsSuppressed = false;
	}

	if (((data->mFrameTimer.mCurrent > data->mFrameTimer.mLimit) && !data->mIsGoingInReverse) || 
		((data->mFrameTimer.mCurrent < 0) && data->mIsGoingInReverse))
	{
		if (!data->mIsLoopingDisabled)
		{
			if (data->mAnimationLoopPoint != 0)
			{
				data->mFrameTimer.mCurrent = data->mAnimationLoopPoint;
			}
			else
			{
				if (data->mIsGoingInReverse)
				{
					data->mFrameTimer.mCurrent = data->mFrameTimer.mLimit;
				}
				else
				{
					data->mFrameTimer.mCurrent = 0;
				}
			}
		}
		else
		{
			if (data->mIsGoingInReverse)
			{
				data->mFrameTimer.mCurrent = 0;
			}
			else
			{
				data->mFrameTimer.mCurrent = data->mFrameTimer.mLimit;
			}

			data->mIsAnimationComplete = true;
		}
	}

	return data->mIsAnimationComplete;
}
void Animation_ResetAnimation(Animation* data)
{
	data->mIsReverse = false;

	data->mIsSuppressed = true;

	data->mIsAnimationComplete = false;

	data->mFrameTimer.mCurrent = 0;

	Timer_Reset(&data->mFlipTimer);
	data->mFlipTimer.mLimit = data->mInitialFlipTimerLimit;
}
Sheet* Animation_GetCurrentSheet(Animation* data)
{
	int32_t counter = data->mFrameTimer.mCurrent;
	if (data->mIsReverse)
	{
		counter = data->mFrameTimer.mLimit - counter;
	}

	int32_t sheetsSize = data->mSheetsLen;
	if ((sheetsSize <= 0) || (counter > sheetsSize - 1))
	{
		return Sheet_GetDefaultSheet();
	}

	return data->mSheets[counter];
}
int32_t Animation_DigitsAtEnd(const char* sheetName, int32_t sheetNameLength)
{
	int32_t numberOfDigits = 0;
	for (int32_t i = (sheetNameLength - 1); i >= 0; i -= 1) // See how many digits there are at the end...
	{
		if (Utils_CharIsDigit(sheetName[i]))
		{
			numberOfDigits += 1;
		}
		else
		{
			return numberOfDigits; // IMMEDIATELY stop once we detect the base.
		}
	}
	return numberOfDigits;
}
void Animation_CheckForAnimation(const char* textureName)
{
	const char* sheetName = textureName;
	int32_t nameLength = (int32_t)Utils_strlen(sheetName);

	char lastDigit = sheetName[nameLength - 1];
	if (!Utils_CharIsDigit(lastDigit))
	{
		return;
	}

	int32_t numberOfDigits = Animation_DigitsAtEnd(sheetName, nameLength);
	if (numberOfDigits == nameLength)
	{
		return;
	}
	
	MString* baseName = NULL;
	MString_AssignSubString(&baseName, sheetName, 0, nameLength - numberOfDigits); //We have our base name, time to find out how many frames there are.

	bool isStart = (lastDigit == '0');
	ptrdiff_t animationInfoLen = arrlen(arr_animation_info);
	for (int32_t i = 0; i < animationInfoLen; i += 1)
	{
		AnimationInfo* existingInfo = &arr_animation_info[i];
		if (Utils_StringEqualTo(existingInfo->mBaseName, MString_Text(baseName)))
		{
			if (isStart)
			{
				existingInfo->mHasStart = true;
			}
			existingInfo->mFrames += 1;
			MString_Dispose(&baseName);
			return;
		}
	}

	AnimationInfo newInfo = { 0 };
	Utils_strlcpy(newInfo.mSheetName, sheetName, EE_FILENAME_MAX);

	Utils_strlcpy(newInfo.mBaseName, MString_Text(baseName), EE_FILENAME_MAX);
	newInfo.mNumberOfDigits = numberOfDigits;
	newInfo.mFrames += 1;
	if (isStart)
	{
		newInfo.mHasStart = true;
	}
	arrput(arr_animation_info, newInfo);

	MString_Dispose(&baseName);
}
void Animation_BuildAnimations(void)
{
	shfree(sh_animation_string_map);
	shfree(sh_conversion_map);
	shfree(sh_animation_sheet_map);
	arrsetlen(arr_animation_info, 0);

	sh_new_arena(sh_animation_string_map);
	sh_new_arena(sh_conversion_map);
	sh_new_arena(sh_animation_sheet_map);

	IStringArray* sheets = Sheet_CreateListOfSheetNames();
	int64_t sheetsLen = IStringArray_Length(sheets);
	for (int32_t i = 0; i < sheetsLen; i += 1)
	{
		const char* sheetName = IStringArray_Get(sheets, i);
		Animation_CheckForAnimation(sheetName);
	}
	IStringArray_Dispose(sheets);

	int64_t animationInfoLen = arrlen(arr_animation_info);
	for (int32_t i = 0; i < animationInfoLen; i += 1)
	{
		AnimationInfo* info = &arr_animation_info[i];
		if (info->mHasStart)
		{
			Animation_AddToAnimationMap(info->mBaseName, info->mSheetName, info->mFrames, info->mNumberOfDigits);
		}
		else
		{
			Logger_LogWarning("Animation is incorrectly setup:");
			Logger_LogWarning(info->mSheetName);
		}
	}
}
void Animation_AddToAnimationMap(const char* baseName, const char* sheetName, int32_t frames, int32_t digits)
{
	IStringArray* sheetNames = IStringArray_Create();
	Animation_CreateAnimationStringArray(sheetNames, baseName, frames, digits);
	shput(sh_conversion_map, baseName, sheetName);
	shput(sh_animation_string_map, sheetName, sheetNames);
}
void Animation_CreateAnimationStringArray(IStringArray* listToAddTo, const char* baseName, int32_t frames, int32_t digits)
{
	MString* stringBuilder = NULL;
	MString_AssignString(&stringBuilder, baseName);

	for (int32_t i = 0; i < digits; i += 1)
	{
		MString_AddAssignChar(&stringBuilder, '0');
	}

	for (int32_t i = 0; i < frames; i += 1)
	{
		int32_t currentNumber = i;

		double placeAsDouble = Math_pow(10, digits - 1);
		int32_t place = (int32_t)placeAsDouble;
		do
		{
			for (int32_t j = 0; j < digits; j += 1)
			{
				int32_t number = currentNumber / place;
				char* stringBuilderText = MString_Text(stringBuilder);
				int32_t stringBuilderLength = MString_Length(stringBuilder);
				stringBuilderText[stringBuilderLength - digits + j] = Animation_GetNumber(number);
				currentNumber -= number * place;
				place /= 10;
			}
		} while (place != 0);
		IStringArray_Add(listToAddTo, MString_Text(stringBuilder));
	}

	MString_Dispose(&stringBuilder);
}
const char* Animation_GetConvertedName(const char* baseName)
{
	return shget(sh_conversion_map, baseName);
}
FixedArray Animation_GetAnimationSheets(const char* animation)
{
	FixedArray fixedArray = { 0 };

	int64_t locInSheetMap = shgeti(sh_animation_sheet_map, animation);
	if (locInSheetMap >= 0)
	{
		return sh_animation_sheet_map[locInSheetMap].value;
	}

	IStringArray* sheetNames = Animation_GetAnimationSheetNames(animation);
	if (sheetNames == NULL)
	{
		return fixedArray;
	}

	int64_t sheetNamesLen = IStringArray_Length(sheetNames);
	if (sheetNamesLen > 0)
	{
		Sheet** sheets = (Sheet**)Utils_calloc(sheetNamesLen, sizeof(Sheet*));
		for (int32_t i = 0; i < sheetNamesLen; i += 1)
		{
			const char* currentSheetName = IStringArray_Get(sheetNames, i);
			Sheet* sheetToSet = Sheet_GetSheet(currentSheetName);
			sheets[i] = sheetToSet;
		}
		fixedArray.data = (uint8_t*)sheets;
		fixedArray.len = (int32_t)sheetNamesLen;
		shput(sh_animation_sheet_map, animation, fixedArray);
		return fixedArray;
	}

	return fixedArray;
}

const Sheet** Animation_GetSheets(const Animation* anim)
{
	if (anim->mSheets == NULL)
	{
		return (const Sheet**)_mDummyAnimationSheets;
	}

	return (const Sheet**)anim->mSheets;
}
