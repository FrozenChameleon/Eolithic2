/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "ImageData.h"
#include "../render/Sheet.h"
#include "../render/Animation.h"
#include "../render/SpriteBatch.h"

typedef struct ImageDataInstance
{
	ImageData* mData;
	Sheet* mSheet;
	Animation mAnimation;
	bool mIsOriginSet;
} ImageDataInstance;

void ImageDataInstance_Init(ImageDataInstance* idi);
void ImageDataInstance_Init2(ImageDataInstance* idi, ImageData* data);

RenderCommandSheet* ImageDataInstance_DrawInterpolated(ImageDataInstance* render, SpriteBatch* spriteBatch, Color color, ShaderProgram* program, Vector2 position, Vector2 lastPosition,
	Vector2 scale, float rotation, bool flipX,
	bool flipY, int32_t overrideDepth);
RenderCommandSheet* ImageDataInstance_DrawInterpolated2(ImageDataInstance* render, SpriteBatch* spriteBatch, Color color, ShaderProgram* program, Vector2 position, Vector2 lastPosition,
	Vector2 scale, float rotation, bool flipX,
	bool flipY, int32_t overrideDepth, Vector2 tempOffset);
Sheet* ImageDataInstance_GetCurrentSheet(ImageDataInstance* render);
Sheet* ImageDataInstance_GetSheet(ImageDataInstance* render, int32_t i);