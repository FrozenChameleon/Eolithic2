/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "../utils/IStringArray.h"
#include "../math/Vector2.h"

typedef struct FontMapData
{
	char mNote[EE_FILENAME_MAX];
	char mFontName[EE_FILENAME_MAX];
	int32_t mFontSize;
	char mReplace[EE_FILENAME_MAX];
	Vector2 mOffset;
	bool mIsBitmap;
	int32_t mLineSpacing;
	int32_t mEffect;
	int32_t mEffectAmount;
	int32_t mCharacterSpacing;
	int32_t mScaleFactor;
	IStringArray* mLanguages;
	int32_t mOverrideBoundsHeight;
} FontMapData;

void FontMapData_Init(FontMapData* fmd);