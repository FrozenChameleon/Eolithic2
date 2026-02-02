/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "SpriteBatch.h"

#include "../render/DrawTool.h"
#include "../utils/Logger.h"
#include "Renderer.h"
#include "../globals/Align.h"
#include "../font/BmFont.h"
#include "../resources/ResourceManagerList.h"
#include "../utils/MString.h"
#include "../../third_party/stb_ds.h"
#include "../render/Texture.h"

static RenderCommandString _mDummyRenderCommandString;

static MString** arr_pinned_strings;

#define MINIMUM_DEPTH 0
#define MAXIMUM_DEPTH 200

static void ClampDepth(int32_t* depth)
{
	if (*depth > MAXIMUM_DEPTH)
	{
		Logger_LogWarning("Given depth over maximum depth");
		*depth = MAXIMUM_DEPTH;
	}
	if (*depth < MINIMUM_DEPTH)
	{
		Logger_LogWarning("Given depth under minimum depth");
		*depth = MINIMUM_DEPTH;
	}
}

void SpriteBatch_DisposePinnedStrings(void)
{
	for (int32_t i = 0; i < arrlen(arr_pinned_strings); i += 1)
	{
		MString* pinnedString = arr_pinned_strings[i];
		MString_Dispose(&pinnedString);
	}
	arrsetlen(arr_pinned_strings, 0);
}

void SpriteBatch_Init(SpriteBatch* sb)
{
	Utils_memset(sb, 0, sizeof(SpriteBatch));

	for (int32_t i = 0; i < SPRITEBATCH_RENDER_STREAMS_LEN; i += 1)
	{
		sb->_mRenderStreams[i]._mBuffer = DynamicByteBuffer_Create();
	}
}

void SpriteBatch_Clear(SpriteBatch* sb)
{
	for (int32_t i = 0; i < SPRITEBATCH_RENDER_STREAMS_LEN; i += 1)
	{
		RenderStream_Clear(&sb->_mRenderStreams[i]);
	}
}

static Texture* GetDefaultTextureData(void)
{
	return (Texture*)ResourceManager_GetDefaultResourceData(ResourceManagerList_Texture());
}

RenderCommandSheet* SpriteBatch_Draw(SpriteBatch* sb, Texture* texture, Color color, int32_t depth, ShaderProgram* program, Vector2 position, Rectangle sourceRectangle, 
	Vector2 scale, float rotation, bool flipX, bool flipY, Vector2 origin)
{
	ClampDepth(&depth);

	if (texture == NULL)
	{
		Logger_LogInformation("MISSING TEXTURE FOR DRAW");
		texture = GetDefaultTextureData();
	}

	RenderCommandSheet* command = RenderStream_GetRenderCommandSheetUninitialized(&sb->_mRenderStreams[depth]);
	command->mType = RENDERER_TYPE_SHEET;
	command->mDepth = (uint8_t)depth;
	command->mFlipX = flipX;
	command->mFlipY = flipY;
	command->mIsRectangle = false;
	command->mIsInterpolated = false;
	command->mBlendState = 0;
	command->mExtraPasses = 0;
	command->mColor = color;
	command->mPosition = position;
	command->mLastPosition = Vector2_Zero;
	command->mOrigin = origin;
	command->mScale = scale;
	command->mRotation = rotation;
	command->mSourceRectangle = sourceRectangle;
	command->mDestinationRectangle = Rectangle_Empty;
	command->mTexture = texture;
	command->mShaderProgram = program;

	return command;
}

RenderCommandSheet* SpriteBatch_DrawInterpolated(SpriteBatch* sb, Texture* texture, Color color, int32_t depth, ShaderProgram* program, Vector2 position, Vector2 lastPosition, Rectangle sourceRectangle, Vector2 scale, float rotation, bool flipX, bool flipY, Vector2 origin)
{
	ClampDepth(&depth);

	if (texture == NULL)
	{
		Logger_LogInformation("MISSING TEXTURE FOR DRAW");
		texture = GetDefaultTextureData();
	}

	RenderCommandSheet* command = RenderStream_GetRenderCommandSheetUninitialized(&sb->_mRenderStreams[depth]);
	command->mType = RENDERER_TYPE_SHEET;
	command->mDepth = (uint8_t)depth;
	command->mFlipX = flipX;
	command->mFlipY = flipY;
	command->mIsRectangle = false;
	command->mIsInterpolated = true;
	command->mBlendState = 0;
	command->mExtraPasses = 0;
	command->mColor = color;
	command->mPosition = position;
	command->mLastPosition = lastPosition;
	command->mOrigin = origin;
	command->mScale = scale;
	command->mRotation = rotation;
	command->mSourceRectangle = sourceRectangle;
	command->mDestinationRectangle = Rectangle_Empty;
	command->mTexture = texture;
	command->mShaderProgram = program;

	return command;
}

RenderCommandManyRectangle* SpriteBatch_DrawManyRectangle(SpriteBatch* sb, int32_t depth, DrawRectangle* manyRectangles)
{
	ClampDepth(&depth);

	RenderCommandManyRectangle* command = RenderStream_GetRenderCommandManyRectangleUninitialized(&sb->_mRenderStreams[depth]);
	command->mType = RENDERER_TYPE_MANY_RECTANGLE;
	command->mDepth = (uint8_t)depth;
	command->mManyRectangles = manyRectangles;
	command->mTexture = DrawTool_GetSinglePixel();

	return command;
}
RenderCommandTileLayer* SpriteBatch_DrawLayer(SpriteBatch* sb, Texture* tileset, Color color, Tile** tileData, Rectangle tileDataBounds, int32_t depth, int32_t layer, int32_t x1, int32_t x2, int32_t y1, int32_t y2)
{
	ClampDepth(&depth);

	if (tileset == NULL)
	{
		Logger_LogInformation("MISSING TILESET FOR DRAW");
		tileset = GetDefaultTextureData();
	}

	RenderCommandTileLayer* command = RenderStream_GetRenderCommandLayerUninitialized(&sb->_mRenderStreams[depth]);
	command->mType = RENDERER_TYPE_TILE_LAYER;
	command->mDepth = (uint8_t)depth;
	command->mLayer = (uint8_t)layer;
	command->mX1 = x1;
	command->mX2 = x2;
	command->mY1 = y1;
	command->mY2 = y2;
	command->mColor = color;
	command->mTileDataBounds = tileDataBounds;
	command->mTileData = tileData;
	command->mTexture = tileset;

	return command;
}

RenderCommandString* SpriteBatch_DrawString(SpriteBatch* sb, const char* font, const char* str, Color color, int32_t depth, Vector2 position)
{
	return SpriteBatch_DrawString2(sb, font, str, color, depth, position, ALIGN_LEFT, ALIGN_TOP);
}
RenderCommandString* SpriteBatch_DrawString2(SpriteBatch* sb, const char* font, const char* str, Color color, int32_t depth, Vector2 position, int32_t alignmentX, int32_t alignmentY)
{
	return SpriteBatch_DrawString3(sb, font, str, color, depth, position, alignmentX, alignmentY, true);
}
RenderCommandString* SpriteBatch_DrawString3(SpriteBatch* sb, const char* font, const char* str, Color color, int32_t depth, Vector2 position, int32_t alignmentX, int32_t alignmentY, bool isLockedToInt)
{
	ClampDepth(&depth);

	BmFont* bitmapFont = (BmFont*)ResourceManager_GetResourceData(ResourceManagerList_Font(), font);
	if (bitmapFont == NULL)
	{
		Logger_LogError("MISSING FONT FOR DRAW: ");
		Logger_LogError(font);
		return &_mDummyRenderCommandString;
	}

	RenderCommandString* command = RenderStream_GetRenderCommandStringUninitialized(&sb->_mRenderStreams[depth]);
	command->mType = RENDERER_TYPE_STRING;
	command->mDepth = (uint8_t)depth;
	command->mIsInterpolated = false;
	command->mIsLockedToInt = isLockedToInt;
	command->mDoNotReplaceFont = false;
	command->mAlignmentX = (int16_t)alignmentX;
	command->mAlignmentY = (int16_t)alignmentY;
	command->mPosition = position;
	command->mLastPosition = Vector2_Zero;
	command->mColor = color;
	command->mFont = bitmapFont;
	MString* tempString = NULL;
	MString_AssignString(&tempString, str);
	command->mString = MString_Text(tempString);
	arrput(arr_pinned_strings, tempString);

	return command;
}

RenderCommandSheet* SpriteBatch_DrawRectangle(SpriteBatch* sb, Texture* texture, Color color, int32_t depth, ShaderProgram* program,
	Rectangle destinationRectangle, Rectangle sourceRectangle, float rotation, bool flipX, bool flipY, Vector2 origin)
{
	ClampDepth(&depth);

	if (texture == NULL)
	{
		Logger_LogInformation("MISSING TEXTURE FOR DRAW");
		texture = GetDefaultTextureData();
	}

	RenderCommandSheet* command = RenderStream_GetRenderCommandSheetUninitialized(&sb->_mRenderStreams[depth]);
	command->mType = RENDERER_TYPE_SHEET;
	command->mDepth = (uint8_t)depth;
	command->mFlipX = flipX;
	command->mFlipY = flipY;
	command->mIsRectangle = true;
	command->mIsInterpolated = false;
	command->mBlendState = 0;
	command->mExtraPasses = 0;
	command->mColor = color;
	command->mPosition = Vector2_Zero;
	command->mLastPosition = Vector2_Zero;
	command->mOrigin = origin;
	command->mScale = Vector2_Zero;
	command->mRotation = rotation;
	command->mSourceRectangle = sourceRectangle;
	command->mDestinationRectangle = destinationRectangle;
	command->mTexture = texture;
	command->mShaderProgram = program;

	return command;
}