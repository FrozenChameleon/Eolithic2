/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "ImageDataInstance.h"
#include "../render/BlendState.h"
#include "../utils/Utils.h"

void ImageDataInstance_Init(ImageDataInstance* idi)
{
	Utils_memset(idi, 0, sizeof(ImageDataInstance));
}
void ImageDataInstance_Init2(ImageDataInstance* idi, ImageData* data)
{
	Utils_memset(idi, 0, sizeof(ImageDataInstance));

	idi->mData = data;

	if (data->mCanAnimate)
	{
		idi->mSheet = Sheet_GetDefaultSheet();
		Animation_Init(&idi->mAnimation, data->mImage, data->mFlipSpeed);
		idi->mAnimation.mIsLoopingDisabled = data->mAnimationStops;
		if (data->mAnimationBlanks)
		{
			idi->mAnimation.mIsLoopingDisabled = true;
		}
		idi->mAnimation.mAnimationLoopPoint = data->mAnimationLoopPoint;
	}
	else
	{
		idi->mSheet = Sheet_GetSheet(data->mImage);
		Utils_memset(&idi->mAnimation, 0, sizeof(Animation));
	}

	idi->mIsOriginSet = ImageData_IsOriginSet(data);
}

RenderCommandSheet* ImageDataInstance_DrawInterpolated(ImageDataInstance* render, SpriteBatch* spriteBatch, Color color, ShaderProgram* program,
	Vector2 position, Vector2 lastPosition, Vector2 scale, float rotation, bool flipX, bool flipY, int32_t overrideDepth)
{
	return ImageDataInstance_DrawInterpolated2(render, spriteBatch, color, program, position, lastPosition, scale, rotation, flipX, flipY, overrideDepth, Vector2_Zero);
}
RenderCommandSheet* ImageDataInstance_DrawInterpolated2(ImageDataInstance* render, SpriteBatch* spriteBatch, Color color, ShaderProgram* program,
	Vector2 position, Vector2 lastPosition, Vector2 scale, float rotation, bool flipX, bool flipY, int32_t overrideDepth, Vector2 tempOffset)
{
	Sheet* sheet = ImageDataInstance_GetCurrentSheet(render);

	Vector2 newOffset = Vector2_Create(render->mData->mOffset.X + tempOffset.X, render->mData->mOffset.Y + tempOffset.Y);
	if (flipX == true)
	{
		newOffset.X = -newOffset.X;
	}
	if (flipY == true)
	{
		newOffset.Y = -newOffset.Y;
	}
	Vector2_MulAssign(&newOffset, scale);

	float newRotation = render->mData->mInitialRotation + rotation;

	int32_t newDepth = render->mData->mDepth;
	if (overrideDepth != -1)
	{
		newDepth = overrideDepth;
	}

	Vector2 newPosition = Vector2_Add(position, newOffset);
	Vector2 newLastPosition = Vector2_Add(lastPosition, newOffset);
	Vector2 newScale = Vector2_MulInt(scale, render->mData->mScaler);

	if (!render->mIsOriginSet)
	{
		RenderCommandSheet* draw = Sheet_DrawInterpolated4(sheet, spriteBatch, color, newDepth, true, true,
			program, newPosition, newLastPosition, newScale, newRotation, flipX, flipY);
		if (render->mData->mIsAdditive)
		{
			draw->mBlendState = BLENDSTATE_ADDITIVE;
		}
		return draw;
	}
	else
	{
		RenderCommandSheet* draw = Sheet_DrawInterpolated5(sheet, spriteBatch, color, newDepth, true, true, program, newPosition, newLastPosition, newScale,
			newRotation, flipX, flipY, render->mData->mOrigin);
		if (render->mData->mIsAdditive)
		{
			draw->mBlendState = BLENDSTATE_ADDITIVE;
		}
		return draw;
	}
}
Sheet* ImageDataInstance_GetCurrentSheet(ImageDataInstance* render)
{
	if (render->mData->mCanAnimate)
	{
		return Animation_GetCurrentSheet(&render->mAnimation);
	}
	else
	{
		return render->mSheet;
	}
}
Sheet* ImageDataInstance_GetSheet(ImageDataInstance* render, int32_t i)
{
	if (render->mData->mCanAnimate)
	{
		Sheet** sheets = (Sheet**)Animation_GetSheets(&render->mAnimation);
		Sheet* sheetToReturn = sheets[i];
		return sheetToReturn;
	}
	else
	{
		return render->mSheet;
	}
}