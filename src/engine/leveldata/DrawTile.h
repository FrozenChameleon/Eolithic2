/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "../math/Point.h"
#include "../render/Color.h"
#include "../utils/MString.h"
#include "../render/SpriteEffects.h"

typedef struct Texture Texture;
typedef struct SpriteBatch SpriteBatch;
typedef struct BufferWriter BufferWriter;
typedef struct BufferReader BufferReader;

enum
{
	DRAWTILE_DRAWING_MODE_NOTHING = 0,
	DRAWTILE_DRAWING_MODE_NOT_ANIMATED = 1,
	DRAWTILE_DRAWING_MODE_ANIMATED = 2
};

typedef struct DrawTile
{
	char mAnimation[EE_FILENAME_MAX];
	Point mPoint;
	bool mFlipX;
	bool mFlipY;
	float mRotation;
	Point mOffsetPoint;
} DrawTile;

void DrawTile_Init(DrawTile* drawTile);
void DrawTile_Read(DrawTile* drawTile, int32_t version, BufferReader* reader);
void DrawTile_LoadSheet(DrawTile* drawTile, int32_t x, int32_t y);
void DrawTile_LoadAnimation(DrawTile* drawTile, const char* animation);
void DrawTile_Draw(DrawTile* drawTile, SpriteBatch* spriteBatch, Texture* texture, Color color, int32_t depth, int32_t x, int32_t y);
Point DrawTile_GetCorrectPoint(DrawTile* drawTile);
bool DrawTile_IsZero(DrawTile* drawTile);
bool DrawTile_IsAnimation(DrawTile* drawTile);
bool DrawTile_IsSheet(DrawTile* drawTile);
bool DrawTile_IsEqualTo(DrawTile* drawTile, DrawTile* otherDrawTile);
