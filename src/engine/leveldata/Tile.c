/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Tile.h"

#include "../utils/Macros.h"
#include "../components/Camera.h"
#include "../utils/Cvars.h"
#include "../render/DrawTool.h"
#include "../core/GameHelper.h"
#include "../io/BufferReader.h"
#include "../utils/Utils.h"
#include "../../third_party/stb_ds.h"
#include "../io/DynamicByteBuffer.h"

#define TILE_SIZE GLOBAL_DEF_TILE_SIZE
#define HALF_TILE_SIZE GLOBAL_DEF_HALF_TILE_SIZE

void Tile_Init(Tile* t)
{
	Utils_memset(t, 0, sizeof(Tile));
	for (int32_t i = 0; i < TILE_DRAW_LAYER_LENGTH; i += 1)
	{
		DrawTile_Init(&t->mDrawTiles[i]);
	}
}
void Tile_Read(int32_t version, Tile* t, BufferReader* br)
{
	//BufferReader_ReadI32(br); //
	//BufferReader_ReadI32(br); //Unused - Multi Array Collision
	t->mCollisionType = BufferReader_ReadI32(br);

	int32_t arrayDrawTilesSize = BufferReader_ReadI32(br);
	for (int32_t i = 0; i < arrayDrawTilesSize; i += 1)
	{
		DrawTile_Read(version, &t->mDrawTiles[i], br);
	}

	int32_t arrayThingDataSize = BufferReader_ReadI32(br);
	for (int32_t i = 0; i < arrayThingDataSize; i += 1)
	{
		ThingInstance instance = { 0 };
		ThingInstance_Read(version, &instance, br);
		arrput(t->arr_instances, instance);
	}

	int32_t arrayPropDataSize = BufferReader_ReadI32(br);
	for (int32_t i = 0; i < arrayPropDataSize; i += 1)
	{
		PropInstance instance = { 0 };
		PropInstance_Init(&instance);
		PropInstance_Read(version, &instance, br);
		arrput(t->arr_props, instance);
	}
}

void Tile_Write(Tile* t, DynamicByteBuffer* dbb)
{
	DynamicByteBuffer_WriteI32(dbb, t->mCollisionType);

	DynamicByteBuffer_WriteNewline(dbb);

	DynamicByteBuffer_WriteI32(dbb, TILE_DRAW_LAYER_LENGTH);
	for (int32_t i = 0; i < TILE_DRAW_LAYER_LENGTH; i += 1)
	{
		DynamicByteBuffer_WriteNewline(dbb);
		DrawTile_Write(&t->mDrawTiles[i], dbb);
	}

	DynamicByteBuffer_WriteNewline(dbb);

	DynamicByteBuffer_WriteI32(dbb, (int32_t)arrlen(t->arr_instances));
	for (int32_t i = 0; i < arrlen(t->arr_instances); i += 1)
	{
		DynamicByteBuffer_WriteNewline(dbb);
		ThingInstance_Write(&t->arr_instances[i], dbb);
	}

	DynamicByteBuffer_WriteNewline(dbb);

	DynamicByteBuffer_WriteI32(dbb, (int32_t)arrlen(t->arr_props));
	for (int32_t i = 0; i < arrlen(t->arr_props); i += 1)
	{
		DynamicByteBuffer_WriteNewline(dbb);
		PropInstance_Write(&t->arr_props[i], dbb);
	}
}

Rectangle Tile_GetCollisionRectangle(Tile* t, int32_t gridX, int32_t gridY)
{
	int32_t rectX = gridX * TILE_SIZE;
	int32_t rectY = gridY * TILE_SIZE;
	int32_t rectWidth = TILE_SIZE;
	int32_t rectHeight = TILE_SIZE;
	if ((t->mCollisionType == GAMEHELPER_PLATFORM_UP) || (t->mCollisionType == GAMEHELPER_PLATFORM_DOWN))
	{
		rectHeight /= 2;
		if (t->mCollisionType == GAMEHELPER_PLATFORM_DOWN)
		{
			rectY += HALF_TILE_SIZE;
		}
	}
	if ((t->mCollisionType == GAMEHELPER_PLATFORM_LEFT) || (t->mCollisionType == GAMEHELPER_PLATFORM_RIGHT))
	{
		rectWidth /= 2;
		if (t->mCollisionType == GAMEHELPER_PLATFORM_RIGHT)
		{
			rectX += HALF_TILE_SIZE;
		}
	}
	return Rectangle_Create(rectX, rectY, rectWidth, rectHeight);
}
void Tile_DrawProps(Tile* t, SpriteBatch* spriteBatch, const Camera* camera, int32_t gridX, int32_t gridY, bool overrideDepth)
{
	Tile_DrawProps2(t, spriteBatch, camera, gridX, gridY, overrideDepth, false);
}
void Tile_DrawProps2(Tile* t, SpriteBatch* spriteBatch, const  Camera* camera, int32_t gridX, int32_t gridY, bool overrideDepth, bool drawInfo)
{
	if (t->arr_props == NULL)
	{
		return;
	}

	int64_t arr_props_len = arrlen(t->arr_props);
	if (arr_props_len == 0)
	{
		return;
	}

	Rectangle cameraRect = Camera_GetRectangle(camera, 1.0f);
	Vector2 position = Vector2_Create((float)(gridX * TILE_SIZE), (float)(gridY * TILE_SIZE));
	for (int32_t i = 0; i < arr_props_len; i += 1)
	{
		PropInstance* propInstance = &t->arr_props[i];

		/*Rectangle cameraCheck = PropInstance_GetRectangle(propInstance, position);
		//To compensate for possible rotation around its upper left, which is how it is done for some reason.
		if (cameraCheck.Width < cameraCheck.Height)
		{
			cameraCheck.Width = cameraCheck.Height;
		}
		else
		{
			cameraCheck.Height = cameraCheck.Width;
		}
		Rectangle_Inflate(&cameraCheck, cameraCheck.Width + HALF_TILE_SIZE, cameraCheck.Height + HALF_TILE_SIZE);
		//
		if (!Rectangle_Intersects(&cameraRect, &cameraCheck))
		{
			continue;
		}*/

		if (!overrideDepth)
		{
			PropInstance_Draw2(propInstance, spriteBatch, position, drawInfo);
		}
		else
		{
			PropInstance_Draw4(propInstance, spriteBatch, TILE_OVERRIDE_DEPTH, position, drawInfo);
		}

		//Debug stuff to draw the camera check rectangle
		//Color color = Color_Red;
		//color.A = 122;
		//OeDrawTool_DrawRectangle(spriteBatch, color, 100, cameraCheck, 0, false);
	}
}