/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "DrawTile.h"

#include "../utils/Macros.h"
#include "../math/Points.h"
#include "../globals/Globals.h"
#include "../render/SpriteBatch.h"
#include "../io/BufferReader.h"
#include "../io/File.h"
#include "AnimTile.h"
#include "../resources/Resource.h"
#include "../render/DrawTool.h"
#include "../render/Color.h"
#include "../utils/Utils.h"

#define TILE_SIZE GLOBAL_DEF_TILE_SIZE

void DrawTile_Init(DrawTile* drawTile)
{
	Utils_memset(drawTile, 0, sizeof(DrawTile));
	Utils_strlcpy(drawTile->mAnimation, EE_STR_EMPTY, EE_FILENAME_MAX);
	drawTile->mPoint = Points_NegativeOne;
	drawTile->mOffsetPoint = Points_NegativeOne;
}
void DrawTile_Read(int32_t version, DrawTile* drawTile, BufferReader* reader)
{
	BufferReader_ReadString(reader, drawTile->mAnimation, EE_FILENAME_MAX);
	Points_Read(&drawTile->mPoint, reader);
	drawTile->mFlipX = BufferReader_ReadBoolean(reader);
	drawTile->mFlipY = BufferReader_ReadBoolean(reader);
	drawTile->mRotation = BufferReader_ReadFloat(reader);
}
void DrawTile_Write(DrawTile* drawTile, DynamicByteBuffer* dbb)
{
	DynamicByteBuffer_WriteString(dbb, drawTile->mAnimation, EE_FILENAME_MAX);
	Points_Write(&drawTile->mPoint, dbb);
	DynamicByteBuffer_WriteBoolean(dbb, drawTile->mFlipX);
	DynamicByteBuffer_WriteBoolean(dbb, drawTile->mFlipY);
	DynamicByteBuffer_WriteFloat(dbb, drawTile->mRotation);
}
void DrawTile_LoadSheet(DrawTile* drawTile, int32_t x, int32_t y)
{
	drawTile->mPoint.X = x;
	drawTile->mPoint.Y = y;

	Utils_strlcpy(drawTile->mAnimation, EE_STR_EMPTY, EE_FILENAME_MAX);
}
void DrawTile_LoadAnimation(DrawTile* drawTile, const char* animation)
{
	Utils_strlcpy(drawTile->mAnimation, animation, EE_FILENAME_MAX);
	drawTile->mPoint = Points_NegativeOne;
}
void DrawTile_Draw(DrawTile* drawTile, SpriteBatch* spriteBatch, Texture* texture, Color color, int32_t depth, int32_t x, int32_t y)
{
	/*if (drawTile->mAnimation.size() != 0) //UNUSED
	{
		OeAnimTile* tile = OeResourceManagers_AnimTileManager.GetResourceData(drawTile->mAnimation);

		if (tile != nullptr)
		{
			tile->Draw(spriteBatch, color, depth, x, y, drawTile->mRotation, drawTile->mFlipX, drawTile->mFlipY);
		}
		else
		{
			OeDrawTool_DrawRectangle(spriteBatch, OeColors_RED, 100, Rectangle(x, y, TILE_SIZE, TILE_SIZE), 0, false);
		}
	}
	else if (drawTile->mPoint.X != -1 || drawTile->mPoint.Y != -1)
	{
		Point point = GetCorrectPoint(drawTile);

		spriteBatch->Draw(texture, color, depth, nullptr, Vector2(x, y), Rectangle(point.X, point.Y, TILE_SIZE, TILE_SIZE),
			Vector2_One, drawTile->mRotation, drawTile->mFlipX, drawTile->mFlipY, Vector2(TILE_SIZE / 2));
	}*/
}
Point DrawTile_GetCorrectPoint(DrawTile* drawTile)
{
	return drawTile->mPoint;
	/* //DEBUG ONLY FOR NOW TODO
	if (!Globals_IsDebugFileMode() || GLOBALS_DEBUG_ENGINE_FORCE_LOAD_DATS)
	{
		return drawTile->mOffsetPoint;
	}
	else
	{
		return drawTile->mPoint;
	}*/
}
bool DrawTile_IsZero(DrawTile* drawTile)
{
	if (Utils_StringEqualTo(drawTile->mAnimation, EE_STR_EMPTY) &&
		(drawTile->mPoint.X == -1) &&
		(drawTile->mPoint.Y == -1))
	{
		return true;
	}
	return false;
}
bool DrawTile_IsAnimation(DrawTile* drawTile)
{
	if (Utils_StringEqualTo(drawTile->mAnimation, EE_STR_EMPTY))
	{
		return false;
	}
	return true;
}
bool DrawTile_IsSheet(DrawTile* drawTile)
{
	if ((drawTile->mPoint.X == -1) &&
		(drawTile->mPoint.Y == -1))
	{
		return false;
	}
	return true;
}
bool DrawTile_IsEqualTo(DrawTile* drawTile, DrawTile* otherDrawTile)
{
	return (Utils_memcmp(drawTile, otherDrawTile, sizeof(DrawTile)) == 0);
}