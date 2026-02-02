/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "MovieImage.h"

#include "../utils/Utils.h"
#include "../render/Texture.h"
#include "../render/Color.h"
#include "../render/Sheet.h"
#include "../resources/ResourceManagerList.h"

static Sheet* CreateNewSheetForMovieImage(const char* image)
{
	ResourceManager* movieTextureMan = ResourceManagerList_MovieTexture();
	const char* internedKey = ResourceManager_GetKey(movieTextureMan, image);
	if (internedKey == NULL)
	{
		return NULL;
	}

	Sheet* temp = (Sheet*)Utils_CallocArena(1, sizeof(Sheet), UTILS_ALLOCATION_ARENA_MOVIE_PLAYER);
	temp->mTextureResource = ResourceManager_GetResource(movieTextureMan, internedKey);
	Utils_strlcpy(temp->mSheetName, internedKey, EE_FILENAME_MAX);
	Utils_strlcpy(temp->mUnderlyingTextureName, internedKey, EE_FILENAME_MAX);
	temp->mRectangle = ((Texture*)temp->mTextureResource->mData)->mBounds;
	return temp;
}
void MovieImage_Init(MovieImage* mi, int32_t scale, const char* image)
{
	Utils_memset(mi, 0, sizeof(MovieImage));

	mi->mScale = scale;

	Utils_strlcpy(mi->mImage, image, EE_FILENAME_MAX);

	mi->mSheetForImage = CreateNewSheetForMovieImage(image);
}
void MovieImage_Init2(MovieImage* mi, int32_t scale, const char* baseImage, int32_t frames, int32_t flip)
{
	Utils_memset(mi, 0, sizeof(MovieImage));

	mi->mScale = scale;

	{
		IStringArray* images = IStringArray_Create();

		Animation_CreateAnimationStringArray(images, baseImage, frames, Utils_GetAmountOfDigits(frames));

		mi->mSheetsForAnimationLen = frames;
		mi->mSheetsForAnimation = (Sheet**)Utils_CallocArena(mi->mSheetsForAnimationLen, sizeof(Sheet*), UTILS_ALLOCATION_ARENA_MOVIE_PLAYER);

		for (int32_t i = 0; i < frames; i += 1)
		{
			const char* currentImage = IStringArray_Get(images, i);
			Resource* resource = ResourceManager_GetResource(ResourceManagerList_MovieTexture(), currentImage);
			if (resource == NULL)
			{
				mi->mSheetsForAnimation[i] = Sheet_GetDefaultSheet();
			}
			else
			{
				mi->mSheetsForAnimation[i] = CreateNewSheetForMovieImage(currentImage);
			}
		}

		IStringArray_Dispose(images);
	}

	Animation_Init2(&mi->mAnimation, mi->mSheetsForAnimation, mi->mSheetsForAnimationLen, flip);

	mi->mIsAnimationSet = true;
}

void MovieImage_SetIsLoopingDisabled(MovieImage* mi, bool value)
{
	if (mi->mIsAnimationSet)
	{
		mi->mAnimation.mIsLoopingDisabled = value;
	}
}
void MovieImage_SetLoopPoint(MovieImage* mi, int32_t value)
{
	mi->mAnimation.mAnimationLoopPoint = value;
}
void MovieImage_Update(MovieImage* mi)
{
	if (mi->mIsAnimationSet)
	{
		Animation_Update(&mi->mAnimation);
	}
}
void MovieImage_SetIsPermanentImage(MovieImage* mi, bool value)
{
	mi->mIsPermanent = value;
}
void MovieImage_DrawHud(MovieImage* mi, SpriteBatch* spriteBatch)
{
	Sheet* sheet = NULL;
	if (mi->mIsAnimationSet)
	{
		sheet = Animation_GetCurrentSheet(&mi->mAnimation);
	}
	else
	{
		sheet = mi->mSheetForImage;
	}

	if (sheet == NULL)
	{
		return;
	}

	Sheet_Draw5(sheet, spriteBatch, COLOR_WHITE, mi->mDepth, false, false, NULL, Vector2_Add(mi->mPosition, mi->mShake), 
		Vector2_Create((float)mi->mScale, (float)mi->mScale), 0, false, false, Vector2_Zero);
}
