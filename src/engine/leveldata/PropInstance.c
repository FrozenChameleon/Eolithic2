/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "PropInstance.h"

#include "../utils/Macros.h"
#include "../utils/Utils.h"
#include "../render/SpriteBatch.h"
#include "../io/BufferReader.h"
#include "../render/Sheet.h"
#include "../math/Points.h"
#include "../render/Color.h"
#include "../render/DrawTool.h"
#include "../resources/Resource.h"
#include "../resources/ResourceManagerList.h"

#define TILE_SIZE GLOBAL_DEF_TILE_SIZE

static Color SELECTBOX_COLOR = { 0, 0, 255, 76 };
static Color SELECTBOX_COLOR_OFF = { 0, 255, 0, 153 };

static Prop* GetThePropData(PropInstance* prop)
{
	if (prop->INTERNAL_mCachedPropData == NULL)
	{
		prop->INTERNAL_mCachedPropData = (Prop*)ResourceManager_GetResourceData(ResourceManagerList_Prop(), prop->mName);
	}

	return prop->INTERNAL_mCachedPropData;
}

void PropInstance_Init(PropInstance* prop)
{
	Utils_memset(prop, 0, sizeof(PropInstance));

	prop->mDepth = 70;
	prop->mScale = 1;
	prop->mDrawOffset = Vector2_Zero;
}
void PropInstance_Write(PropInstance* prop, DynamicByteBuffer* dbb)
{
	DynamicByteBuffer_WriteFloat(dbb, prop->mOffset.X);
	DynamicByteBuffer_WriteFloat(dbb, prop->mOffset.Y);
	DynamicByteBuffer_WriteFloat(dbb, prop->mScale);
	DynamicByteBuffer_WriteI32(dbb, prop->mDepth);
	DynamicByteBuffer_WriteFloat(dbb, prop->mRotation);
	DynamicByteBuffer_WriteFloat(dbb, prop->mDrawOffset.X);
	DynamicByteBuffer_WriteFloat(dbb, prop->mDrawOffset.Y);
	DynamicByteBuffer_WriteBoolean(dbb, prop->mFlipX);
	DynamicByteBuffer_WriteBoolean(dbb, prop->mFlipY);
	DynamicByteBuffer_WriteString(dbb, prop->mName, EE_FILENAME_MAX);
}
void PropInstance_Read(int32_t version, PropInstance* prop, BufferReader* reader)
{
	prop->mOffset.X = BufferReader_ReadFloat(reader);
	prop->mOffset.Y = BufferReader_ReadFloat(reader);
	prop->mScale = BufferReader_ReadFloat(reader);
	prop->mDepth = BufferReader_ReadI32(reader);
	prop->mRotation = BufferReader_ReadFloat(reader);
	prop->mDrawOffset.X = BufferReader_ReadFloat(reader);
	prop->mDrawOffset.Y = BufferReader_ReadFloat(reader);
	prop->mFlipX = BufferReader_ReadBoolean(reader);
	prop->mFlipY = BufferReader_ReadBoolean(reader);
	BufferReader_ReadString(reader, prop->mName, EE_FILENAME_MAX);
}
void PropInstance_Draw(PropInstance* prop, SpriteBatch* spriteBatch, Vector2 position)
{
	PropInstance_Draw3(prop, spriteBatch, prop->mDepth, position);
}
void PropInstance_Draw2(PropInstance* prop, SpriteBatch* spriteBatch, Vector2 position, bool drawInfo)
{
	PropInstance_Draw4(prop, spriteBatch, prop->mDepth, position, drawInfo);
}
void PropInstance_Draw3(PropInstance* prop, SpriteBatch* spriteBatch, int32_t depth, Vector2 position)
{
	PropInstance_Draw4(prop, spriteBatch, depth, position, false);
}
void PropInstance_Draw4(PropInstance* prop, SpriteBatch* spriteBatch, int32_t depth, Vector2 position, bool drawInfo)
{
	Prop* propData = GetThePropData(prop);

	if (propData == NULL)
	{
		DrawTool_DrawRectangle2(spriteBatch, COLOR_BLUE, 100, Rectangle_Create((int32_t)position.X, (int32_t)position.Y, TILE_SIZE, TILE_SIZE), 0, false);
	}
	else
	{
		Vector2 temp;
		if (!drawInfo)
		{
			temp = Vector2_Add(Vector2_Add(position, prop->mOffset), prop->mDrawOffset);
		}
		else
		{
			temp = Vector2_Add(position, prop->mOffset);
		}
		Prop_Draw(propData, spriteBatch, depth, Points_ToPointFromVector2(temp), prop->mScale, prop->mRotation, prop->mFlipX, prop->mFlipY, drawInfo);
	}
}
Prop* PropInstance_GetPropData(PropInstance* prop)
{
	return GetThePropData(prop);
}
bool PropInstance_IsPropValid(PropInstance* prop)
{
	if (prop->mName == NULL)
	{
		return false;
	}

	if (GetThePropData(prop) == NULL)
	{
		return false;
	}
	return true;
}

Rectangle PropInstance_GetRectangle(PropInstance* prop, Vector2 position)
{
	Prop* propData = GetThePropData(prop);

	Sheet* propSheet;
	if (propData != NULL)
	{
		propSheet = Prop_GetSheet(propData);
	}
	else
	{
		propSheet = NULL;
	}

	int32_t width;
	int32_t height;
	if (propSheet != NULL)
	{
		const Rectangle* rect = &propSheet->mRectangle;
		width = (int32_t)(rect->Width * propData->mScaler * prop->mScale);
		height = (int32_t)(rect->Height * propData->mScaler * prop->mScale);
	}
	else
	{
		width = TILE_SIZE;
		height = TILE_SIZE;
	}

	return Rectangle_Create((int32_t)(position.X + prop->mOffset.X),
		(int32_t)(position.Y + prop->mOffset.Y),
		width, height);
}
bool PropInstance_IsPropActuallyTouched(PropInstance* prop, Point relativeMouse)
{
	return false;

	/*OeProp* propData = GetThePropData(prop); //UNUSED
	if (propData != nullptr)
	{
		//WILLNOTDO 05152023
		//OeSheet* sheet = ref OeSheet.GetSheet(propData.mTextureName);
		//OeTexture tex = sheet.mTextureResource.GetData();
		//Texture2D texture = tex.GetTexture2D();
		//Color[] colors1d = new Color[texture.Width * texture.Height];
		//texture.GetData(colors1d);
		//Color[, ] colors2d = new Color[texture.Width, texture.Height];
		//for (int32_t i = 0; i < texture.Width; i += 1)
		//{
		//	for (int32_t j = 0; j < texture.Height; j += 1)
		//	{
		//		colors2d[i, j] = colors1d[i + j * texture.Width];
		//	}
		//}
		//int32_t targetX = relativeMouse.X / propData.mScaler;
		//int32_t targetY = relativeMouse.Y / propData.mScaler;
		//if (prop->mFlipX)
		//{
		//	targetX = texture.Width - targetX;
		//}
		//if (prop->mFlipY)
		//{
		//	targetY = texture.Height - targetY;
		//}
		//targetX = OeMath.Min(targetX, texture.Width - 1);
		//targetX = OeMath.Max(targetX, 0);
		//targetY = OeMath.Min(targetY, texture.Height - 1);
		//targetY = OeMath.Max(targetY, 0);
		//if (colors2d[targetX, targetY].A != 0)
		//{
		//	return true;
		//}
	}
	return false;*/
}
bool PropInstance_IsEqualTo(PropInstance* pi, PropInstance* instance)
{
	if (Vector2_EqualTo(instance->mOffset, pi->mOffset) && (instance->mScale == pi->mScale) && (instance->mDepth == pi->mDepth) &&
		(instance->mRotation == pi->mRotation) && Vector2_EqualTo(instance->mDrawOffset, pi->mDrawOffset) && (instance->mFlipX == pi->mFlipX) &&
		(instance->mFlipY == pi->mFlipY) && (Utils_StringEqualTo(instance->mName, pi->mName)))
	{
		return true;
	}

	return false;
}