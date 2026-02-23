/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "Line.h"
#include "Tile.h"
#include "MetaMap.h"
#include "LayerData.h"
#include "LevelCameraData.h"
#include "../math/Rectangle.h"
#include "../render/DrawRectangle.h"
#include "../utils/MString.h"
#include "Tilemap.h"

typedef struct BufferReader BufferReader;
typedef struct SpriteBatch SpriteBatch;

#define LEVELDATA_STRING_DATA_LENGTH 20
#define LEVELDATA_LAYER_DATA_LENGTH 10

typedef struct LevelData
{
	Tile** arr_save_tiles;
	int32_t* _mSaveTileMap;
	bool _mIsMetaMap;
	MetaMap _mMetaMap;
	DrawRectangle* _mDynamicManyRectangles;
	char mLevelName[EE_FILENAME_MAX];
	char mTilesetName[EE_FILENAME_MAX];
	char mStringData[LEVELDATA_STRING_DATA_LENGTH][EE_FILENAME_MAX];
	LayerData mLayerData[LEVELDATA_LAYER_DATA_LENGTH];
	Tilemap tilemap;
	LevelCameraData* arr_camera_data;
	Line* arr_lines;
} LevelData;

LevelData* LevelData_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);
void LevelData_Init(LevelData* ld);
void LevelData_Dispose(LevelData* ld);
void LevelData_Read(LevelData* ld, BufferReader* reader);
void LevelData_ReadHeader(int32_t version, LevelData* ld, BufferReader* reader);
void LevelData_ReadData(int32_t version, LevelData* ld, BufferReader* reader);
void LevelData_Write(LevelData* ld, DynamicByteBuffer* dbb);
void LevelData_WriteHeader(LevelData* ld, DynamicByteBuffer* dbb);
void LevelData_WriteData(LevelData* ld, DynamicByteBuffer* dbb);
void LevelData_LoadSetupOffsets(LevelData* ld);
bool LevelData_LoadSetupTileData(LevelData* ld);
Rectangle LevelData_GetBoundary(LevelData* ld);
Rectangle LevelData_GetBoundaryInPixels(LevelData* ld);
Tilemap* LevelData_GetTilemap(LevelData* ld);
Tile* LevelData_GetTile(LevelData* ld, int32_t x, int32_t y);
int32_t* LevelData_CreateCollisionArray(LevelData* ld);
int32_t* LevelData_CreateEmptyCollisionArray(LevelData* ld);
void LevelData_ImprintToCollisionArray(LevelData* ld, int32_t x, int32_t y, int32_t* collisionArray);
bool LevelData_IsTilesetNameSet(LevelData* ld);
void LevelData_DrawTiles(LevelData* ld, SpriteBatch* spriteBatch, Camera* camera);
void LevelData_DrawTiles2(LevelData* ld, SpriteBatch* spriteBatch, Camera* camera, int32_t preferredLayer, float cameraMul);
Texture* LevelData_GetTilesetTexture(LevelData* ld);
void LevelData_DrawProps(LevelData* ld, SpriteBatch* spriteBatch, Camera* camera);
void LevelData_DrawProps2(LevelData* ld, SpriteBatch* spriteBatch, Camera* camera, bool drawInfo);
void LevelData_DrawCollision(LevelData* ld, SpriteBatch* spriteBatch, Camera* camera);