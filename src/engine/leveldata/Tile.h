/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "DrawTile.h"
#include "ThingInstance.h"
#include "PropInstance.h"
#include "../components/Camera.h"

typedef struct LevelData LevelData;
typedef struct Texture Texture;
typedef struct BufferReader BufferReader;
typedef struct DynamicByteBuffer DynamicByteBuffer;
typedef struct SpriteBatch SpriteBatch;

#define TILE_DRAW_LAYER_LENGTH 10
#define TILE_OVERRIDE_DEPTH 190

typedef struct Tile
{
	int32_t mCollisionType;
	DrawTile mDrawTiles[TILE_DRAW_LAYER_LENGTH];
	ThingInstance* arr_instances;
	PropInstance* arr_props;
} Tile;

void Tile_Init(Tile* t);
void Tile_Read(int32_t version, Tile* t, BufferReader* br);
void Tile_Write(Tile* t, DynamicByteBuffer* dbb);
Rectangle Tile_GetCollisionRectangle(Tile* t, int32_t gridX, int32_t gridY);
void Tile_DrawProps(Tile* t, SpriteBatch* spriteBatch, const Camera* camera, int32_t gridX, int32_t gridY, bool overrideDepth);
void Tile_DrawProps2(Tile* t, SpriteBatch* spriteBatch, const Camera* camera, int32_t gridX, int32_t gridY, bool overrideDepth, bool drawInfo);