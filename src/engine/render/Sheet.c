/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Sheet.h"

#include "../utils/Macros.h"
#include "../utils/Utils.h"
#include "../../third_party/stb_ds.h"
#include "../resources/ResourceManagerList.h"
#include "../globals/Globals.h"
#include "SpriteBatch.h"
#include "../render/TextureOffset.h"

#define DEBUG_SHEET_NAME "DEBUG_ERROR_777"

static Sheet* _mDummy;
static struct { const char* key; Sheet* value; } *sh_sheet_map;
static Sheet** arr_sheet_list;
static bool _mHasInit;

static void Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	sh_new_arena(sh_sheet_map);

	_mHasInit = true;
}
static void InitSheet(Sheet* sheet)
{
	Utils_memset(sheet, 0, sizeof(Sheet));

	Utils_strlcpy(sheet->mSheetName, EE_STR_EMPTY, EE_FILENAME_MAX);
	Utils_strlcpy(sheet->mUnderlyingTextureName, EE_STR_EMPTY, EE_FILENAME_MAX);
	sheet->mRectangle = Rectangle_Empty;
	sheet->mTextureResource = NULL;
}

Sheet* Sheet_GetDefaultSheet(void)
{
	Init();

	int64_t index = shgeti(sh_sheet_map, "DEBUG_ERROR_777");
	if (index >= 0)
	{
		return sh_sheet_map[index].value;
	}
	else
	{
		return _mDummy;
	}
}
Sheet* Sheet_GetSheet(const char* name)
{
	Init();

	int64_t index = shgeti(sh_sheet_map, name);
	if (index >= 0)
	{
		return sh_sheet_map[index].value;
	}
	else
	{
		return Sheet_GetDefaultSheet();
	}
}
void Sheet_BuildSheets(void)
{
	arrsetlen(arr_sheet_list, 0);
	shfree(sh_sheet_map);

	Init();

	ResourceManager* textureMan = ResourceManagerList_Texture();
	for (int32_t i = 0; i < ResourceManager_Length(textureMan); i += 1)
	{
		Resource* resource = ResourceManager_GetResourceByIndex(textureMan, i);
		if (resource->mData == NULL)
		{
			continue;
		}

		Sheet* sheet = (Sheet*)Utils_calloc(1, sizeof(Sheet));
		InitSheet(sheet);
		Utils_strlcpy(sheet->mUnderlyingTextureName, resource->mFileNameWithoutExtension, EE_FILENAME_MAX);
		Utils_strlcpy(sheet->mSheetName, sheet->mUnderlyingTextureName, EE_FILENAME_MAX);
		sheet->mTextureResource = resource;
		sheet->mRectangle = ((Texture*)sheet->mTextureResource->mData)->mBounds;
		arrput(arr_sheet_list, sheet);
		shput(sh_sheet_map, resource->mPath, sheet);
	}

	if (!Globals_IsDebugFileMode() || GLOBALS_DEBUG_ENGINE_FORCE_LOAD_DATS)
	{
		ResourceManager* textureOffsetMan = ResourceManagerList_TextureOffset();
		for (int32_t i = 0; i < ResourceManager_Length(textureOffsetMan); i += 1)
		{
			Resource* texOffsetResource = ResourceManager_GetResourceByIndex(textureOffsetMan, i);
			if (texOffsetResource->mData == NULL)
			{
				continue;
			}

			TextureOffset* texOffset = (TextureOffset*)texOffsetResource->mData;
			int64_t infoLen = arrlen(texOffset->arr_offsets);
			for (int32_t j = 0; j < infoLen; j += 1)
			{
				TextureOffsetInfo* info = &texOffset->arr_offsets[j];
				Sheet* sheet = (Sheet*)Utils_calloc(1, sizeof(Sheet));
				InitSheet(sheet);
				Utils_strlcpy(sheet->mSheetName, info->mVirtualName, EE_FILENAME_MAX);
				Utils_strlcpy(sheet->mUnderlyingTextureName, info->mFilenameWithoutExtension, EE_FILENAME_MAX);
				sheet->mRectangle = info->mRect;
				sheet->mTextureResource = ResourceManager_GetResource(textureMan, info->mFilenameWithoutExtension);
				arrput(arr_sheet_list, sheet);
				shput(sh_sheet_map, sheet->mSheetName, sheet);
			}

		}
	}
}
bool Sheet_HasSheet(const char* name)
{
	Init();

	ptrdiff_t index = shgeti(sh_sheet_map, name);
	if (index < 0)
	{
		return false;
	}
	return true;
}
IStringArray* Sheet_CreateListOfSheetNames(void)
{
	IStringArray* sa = IStringArray_Create();
	ptrdiff_t len = shlen(sh_sheet_map);
	for (int32_t i = 0; i < len; i += 1)
	{
		const char* sheetName = sh_sheet_map[i].value->mSheetName;
		IStringArray_Add(sa, sheetName);
	}
	return sa;
}
Resource* Sheet_GetTextureResource(Sheet* sheet)
{
	return sheet->mTextureResource;
}
Texture* Sheet_GetTexture(Sheet* sheet)
{
	Resource* texResource = Sheet_GetTextureResource(sheet);
	if (texResource == NULL)
	{
		return NULL;
	}
	else
	{
		return (Texture*)texResource->mData;
	}
}

RenderCommandSheet* Sheet_Draw(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY, 
	ShaderProgram* program, Vector2 position)
{
	return Sheet_Draw2(sheet, spriteBatch, color, depth, isCenterX, isCenterY, program, position, Vector2_One);
}
RenderCommandSheet* Sheet_Draw2(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY, 
	ShaderProgram* program, Vector2 position, Vector2 scale)
{
	return Sheet_Draw3(sheet, spriteBatch, color, depth, isCenterX, isCenterY, program, position, scale, 0);
}
RenderCommandSheet* Sheet_Draw3(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY, 
	ShaderProgram* program, Vector2 position, Vector2 scale, float rotation)
{
	return Sheet_Draw4(sheet, spriteBatch, color, depth, isCenterX, isCenterY, program, position, scale, rotation, false, false);
}
RenderCommandSheet* Sheet_Draw4(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY,
	ShaderProgram* program, Vector2 position, Vector2 scale, float rotation, bool flipX, bool flipY)
{
	Rectangle rect = sheet->mRectangle;
	return Sheet_Draw5(sheet, spriteBatch, color, depth, isCenterX, isCenterY, program, position, scale, rotation, flipX, flipY,
		Vector2_DivFloat(Vector2_Create((float)rect.Width, (float)rect.Height), 2.0f));
}
RenderCommandSheet* Sheet_Draw5(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY, 
	ShaderProgram* program, Vector2 position, Vector2 scale, float rotation, bool flipX, bool flipY, Vector2 origin)
{
	Rectangle rect = sheet->mRectangle;
	float halfWidth = rect.Width / 2.0f;
	float halfHeight = rect.Height / 2.0f;
	float newX = isCenterX ? position.X - (int32_t)halfWidth : position.X;
	float newY = isCenterY ? position.Y - (int32_t)halfHeight : position.Y;
	return SpriteBatch_Draw(spriteBatch, Sheet_GetTexture(sheet), color, depth, program, Vector2_Create(newX, newY),
		rect, scale, rotation, flipX, flipY, origin);
}

RenderCommandSheet* Sheet_DrawInterpolated(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY,
	ShaderProgram* program, Vector2 position, Vector2 lastPosition)
{
	return Sheet_DrawInterpolated2(sheet, spriteBatch, color, depth, isCenterX, isCenterY, program, position, lastPosition, Vector2_One);
}
RenderCommandSheet* Sheet_DrawInterpolated2(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY,
	ShaderProgram* program, Vector2 position, Vector2 lastPosition, Vector2 scale)
{
	return Sheet_DrawInterpolated3(sheet, spriteBatch, color, depth, isCenterX, isCenterY, program, position, lastPosition, scale, 0);
}
RenderCommandSheet* Sheet_DrawInterpolated3(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY,
	ShaderProgram* program, Vector2 position, Vector2 lastPosition, Vector2 scale, float rotation)
{
	return Sheet_DrawInterpolated4(sheet, spriteBatch, color, depth, isCenterX, isCenterY, program, position, lastPosition, scale, rotation, false, false);
}
RenderCommandSheet* Sheet_DrawInterpolated4(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY,
	ShaderProgram* program, Vector2 position, Vector2 lastPosition, Vector2 scale, float rotation, bool flipX, bool flipY)
{
	Rectangle rect = sheet->mRectangle;
	Vector2 origin = Vector2_DivFloat(Vector2_Create((float)rect.Width, (float)rect.Height), 2.0f);
	return Sheet_DrawInterpolated5(sheet, spriteBatch, color, depth, isCenterX, isCenterY, program, position, lastPosition, scale, rotation, flipX, flipY,
		origin);
}
RenderCommandSheet* Sheet_DrawInterpolated5(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY,
	ShaderProgram* program, Vector2 position, Vector2 lastPosition, Vector2 scale, float rotation, bool flipX, bool flipY, Vector2 origin)
{
	Rectangle rect = sheet->mRectangle;
	float halfWidth = rect.Width / 2.0f;
	float halfHeight = rect.Height / 2.0f;
	float currentX = isCenterX ? (position.X - (int32_t)halfWidth) : position.X;
	float currentY = isCenterY ? (position.Y - (int32_t)halfHeight) : position.Y;
	float lastX = isCenterX ? (lastPosition.X - (int32_t)halfWidth) : lastPosition.X;
	float lastY = isCenterY ? (lastPosition.Y - (int32_t)halfHeight) : lastPosition.Y;
	return SpriteBatch_DrawInterpolated(spriteBatch, Sheet_GetTexture(sheet), color, depth, program, Vector2_Create(currentX, currentY), Vector2_Create(lastX, lastY),
		rect, scale, rotation, flipX, flipY, origin);
}
RenderCommandSheet* Sheet_DrawDestinationRect(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenter, 
	ShaderProgram* program, Rectangle destinationRect, float rotation, bool flipX, bool flipY, Vector2 origin)
{
	Rectangle rect = sheet->mRectangle;
	int32_t newX = destinationRect.X;
	int32_t newY = destinationRect.Y;
	if (isCenter)
	{
		newX -= destinationRect.Width / 2;
		newY -= destinationRect.Height / 2;
	}
	return SpriteBatch_DrawRectangle(spriteBatch, Sheet_GetTexture(sheet), color, depth, program, Rectangle_Create(newX, newY, destinationRect.Width, destinationRect.Height),
		rect, rotation, flipX, flipY, origin);
}
RenderCommandSheet* Sheet_DrawSourceRect(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenter, 
	ShaderProgram* program, Vector2 position, Rectangle sourceRectangle, float scale, float rotation, bool flipX, bool flipY)
{
	float halfWidth = sourceRectangle.Width / 2.0f;
	float halfHeight = sourceRectangle.Height / 2.0f;
	float newX = isCenter ? position.X - (int32_t)halfWidth : position.X;
	float newY = isCenter ? position.Y - (int32_t)halfHeight : position.Y;
	return SpriteBatch_Draw(spriteBatch, Sheet_GetTexture(sheet), color, depth, program, Vector2_Create(newX, newY), sourceRectangle, Vector2_Create2(scale),
		rotation, flipX, flipY, Vector2_Create((float)(sourceRectangle.Width / 2), (float)(sourceRectangle.Height / 2)));
}
RenderCommandSheet* Sheet_DrawSourceRectInterpolated(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenter,
	ShaderProgram* program, Vector2 position, Vector2 lastPosition, Rectangle sourceRectangle, float scale, float rotation, bool flipX, bool flipY)
{
	float halfWidth = sourceRectangle.Width / 2.0f;
	float halfHeight = sourceRectangle.Height / 2.0f;
	float newX = isCenter ? position.X - (int32_t)halfWidth : position.X;
	float newY = isCenter ? position.Y - (int32_t)halfHeight : position.Y;
	float lastX = isCenter ? lastPosition.X - (int32_t)halfWidth : lastPosition.X;
	float lastY = isCenter ? lastPosition.Y - (int32_t)halfHeight : lastPosition.Y;
	return SpriteBatch_DrawInterpolated(spriteBatch, Sheet_GetTexture(sheet), color, depth, program, Vector2_Create(newX, newY), Vector2_Create(lastX, lastY),
		sourceRectangle, Vector2_Create2(scale), rotation, flipX, flipY, Vector2_Create((float)(sourceRectangle.Width / 2), (float)(sourceRectangle.Height / 2)));
}