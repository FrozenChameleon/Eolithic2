/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Vector2.h"
#include "../render/Animation.h"

typedef struct SpriteBatch SpriteBatch;

typedef struct MovieImage
{
	int32_t mScale;
	int32_t mDepth;
	bool mIsAnimationSet;
	bool mIsPermanent;
	Vector2 mPosition;
	Vector2 mShake;
	Animation mAnimation;
	char mImage[EE_FILENAME_MAX];
	Sheet** mSheetsForAnimation;
	int32_t mSheetsForAnimationLen;
	Sheet* mSheetForImage;
} MovieImage;

void MovieImage_Init(MovieImage* mi, int32_t scale, const char* image);
void MovieImage_Init2(MovieImage* mi, int32_t scale, const char* baseImage, int32_t frames, int32_t flip);
void MovieImage_SetIsLoopingDisabled(MovieImage* mi, bool value);
void MovieImage_SetLoopPoint(MovieImage* mi, int32_t value);
void MovieImage_Update(MovieImage* mi);
void MovieImage_SetIsPermanentImage(MovieImage* mi, bool value);
void MovieImage_DrawHud(MovieImage* mi, SpriteBatch* spriteBatch);