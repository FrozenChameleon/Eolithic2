/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "LevelData.h"

#include "../utils/Macros.h"
#include "../utils/Utils.h"
#include "../io/BufferReader.h"
#include "../globals/Globals.h"
#include "../math/Points.h"
#include "../../third_party/stb_ds.h"
#include "../utils/Cvars.h"
#include "../render/SpriteBatch.h"
#include "../resources/ResourceList.h"
#include "../utils/MString.h"
#include "../render/TilesetOffset.h"
#include "../utils/Utils.h"
#include "../../third_party/stb_ds.h"
#include "../utils/Logger.h"

#define VERSION_LEVELDATA 12

static DrawRectangle* arr_many_rectangles;

static int32_t LevelData_GetTilePos1D(LevelData* ld, int32_t i, int32_t j)
{
	return Utils_Get1DArrayPosFor2DArray(i, j, ld->tilemap.boundary.Width);
}

LevelData* LevelData_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	LevelData* levelData = LevelData_CreateNew();
	LevelData_Read(levelData, br);
	return levelData;
}
LevelData* LevelData_CreateNew()
{
	LevelData* ld = (LevelData*)Utils_calloc(1, sizeof(LevelData));
	LevelData_Init(ld);
	return ld;
}
void LevelData_Init(LevelData* ld)
{
	Utils_strlcpy(ld->mLevelName, EE_STR_NOT_SET, EE_FILENAME_MAX);
	Utils_strlcpy(ld->mTilesetName, EE_STR_NOT_SET, EE_FILENAME_MAX);

	for (int i = 0; i < LEVELDATA_STRING_DATA_LENGTH; i += 1)
	{
		Utils_strlcpy(ld->mStringData[i], EE_STR_NOT_SET, EE_FILENAME_MAX);
	}

	for (int i = 0; i < LEVELDATA_LAYER_DATA_LENGTH; i += 1)
	{
		ld->mLayerData[i].mDepth = (int32_t)(100 - (i * (100.0f / LEVELDATA_LAYER_DATA_LENGTH)));
	}

	int32_t defaultWidth = Cvars_GetAsInt(CVARS_ENGINE_DEFAULT_CHUNK_WIDTH);
	int32_t defaultHeight = Cvars_GetAsInt(CVARS_ENGINE_DEFAULT_CHUNK_HEIGHT);
	Rectangle tilemapBoundary = Rectangle_Create(0, 0, defaultWidth, defaultHeight);
	Tilemap_Init(&ld->tilemap, tilemapBoundary);
	Tilemap_FillWithNewTiles(&ld->tilemap);
}
void LevelData_Dispose(LevelData* ld)
{
	Utils_free(ld);
}
void LevelData_Read(LevelData* ld, BufferReader* reader)
{
	if (!BufferReader_ReadMagicNumber(reader))
	{
		return;
	}
	int16_t versionNumber = BufferReader_ReadVersionNumber(reader);

	LevelData_ReadHeader(versionNumber, ld, reader);

	/*if (Globals_IsDebugFileMode())// UNUSED
	{
		//if (mStringData.Length < STRING_DATA_LENGTH) //if debug then correct custom data if wrong size...
		//{
		//	OeLogger_Log(LOGGER_INFORMATION, "Correcting custom data amount...");

		//	string[] newCustom = new string[CUSTOM_DATA_AMOUNT];
		//	for (int32_t i = 0; i < newCustom.Length; i += 1)
		//	{
		//		newCustom[i] = OeUtils_NOT_SET;
		//	}
		//	for (int32_t i = 0; i < mStringData.Length; i += 1)
		//	{
		//		newCustom[i] = mStringData[i];
		//	}

		//	mStringData = newCustom;
		//}
	}*/

	if (Utils_StringEqualTo(ld->mLevelName, EE_STR_EMPTY))
	{
		Utils_strlcpy(ld->mLevelName, EE_STR_NOT_SET, EE_FILENAME_MAX);
	}

	/*if (ld->_mIsMetaMap) //UNUSED
	{
		SetupAsMetaMap();
		_mMetaMap.Read(reader);
		_mMetaMap.TestAndRemoveInvalidChunks();
		if (OeGlobals_IsDebugFileMode())
		{
			_mMetaMap.DebugAlignLevelIni();
		}
		mTileData = _mMetaMap.CreateTileData();
		mCameraData.clear();
		_mMetaMap.HandleCameraStuff(mCameraData);
		return;
	}*/

	LevelData_ReadData(versionNumber, ld, reader);
	LevelData_LoadSetupOffsets(ld);
	LevelData_LoadSetupTileData(ld);
}
static void CreateSaveTiles(LevelData* ld)
{
	ld->_mSaveTileMap = (int32_t*)Utils_calloc(ld->tilemap.boundary.Width * ld->tilemap.boundary.Height, sizeof(int32_t)); //LEAKS, DONT CARE!

	arrsetlen(ld->arr_save_tiles, 0);

	for (int i = 0; i < ld->tilemap.boundary.Width; i += 1) // <-
	{
		for (int j = 0; j < ld->tilemap.boundary.Height; j += 1) // <- TODO 20260302 REVERSE THESE ONCE ABLE TO CONFIRM LEVEL DATA INTEGRITY
		{
			Tile* currentTile = LevelData_GetTile(ld, i, j);

			int placement = -1;

			for (int k = 0; k < arrlen(ld->arr_save_tiles); k += 1)
			{
				if (Tile_EqualTo(ld->arr_save_tiles[k], currentTile))
				{
					placement = k;
					break;
				}
			}

			int32_t arrayIndex = Utils_Get1DArrayPosFor2DArray(i, j, ld->tilemap.boundary.Width);
			if (placement == -1)
			{
				ld->_mSaveTileMap[arrayIndex] = (int32_t)arrlen(ld->arr_save_tiles);
				arrput(ld->arr_save_tiles, currentTile);
			}
			else
			{
				ld->_mSaveTileMap[arrayIndex] = placement;
			}
		}
	}
}
void LevelData_Write(LevelData* ld, DynamicByteBuffer* dbb)
{
	CreateSaveTiles(ld);

	DynamicByteBuffer_WriteMagicNumber(dbb);
	DynamicByteBuffer_WriteVersionNumber(dbb, VERSION_LEVELDATA);

	LevelData_WriteHeader(ld, dbb);

	DynamicByteBuffer_WriteNewline(dbb);

	/*if (IsMetaMap())
	{
		_mMetaMap.Write(writer);
		if (OeGlobals.DebugIsMetaMapEditTileModeAtMapLoad())
		{
			_mMetaMap.CopyAllChunkLevelDataFromTileData(mTileData);
			_mMetaMap.SaveAllChunkLevelData();
		}
		return;
	}*/

	LevelData_WriteData(ld, dbb);
}
void LevelData_ReadHeader(int32_t version, LevelData* ld, BufferReader* reader)
{
	//version = BufferReader_ReadI32(reader); // <- //UNSUED

	ld->_mIsMetaMap = BufferReader_ReadBoolean(reader);

	BufferReader_ReadString(reader, ld->mLevelName, EE_FILENAME_MAX);
	BufferReader_ReadString(reader, ld->mTilesetName, EE_FILENAME_MAX);

	int32_t customDataSize = BufferReader_ReadI32(reader);
	for (int32_t i = 0; i < customDataSize; i += 1)
	{
		BufferReader_ReadString(reader, ld->mStringData[i], EE_FILENAME_MAX);
	}

	//BufferReader_ReadFloat(reader); //Unused - Player Position X
	//BufferReader_ReadFloat(reader); //Unused - Player Position Y

	int32_t layerDataSize = BufferReader_ReadI32(reader);
	for (int32_t i = 0; i < layerDataSize; i += 1)
	{
		LayerData_Read(&ld->mLayerData[i], reader);
	}
}
void LevelData_WriteHeader(LevelData* ld, DynamicByteBuffer* dbb)
{
	DynamicByteBuffer_WriteBoolean(dbb, ld->_mIsMetaMap);

	DynamicByteBuffer_WriteString(dbb, ld->mLevelName, EE_FILENAME_MAX);
	DynamicByteBuffer_WriteString(dbb, ld->mTilesetName, EE_FILENAME_MAX);

	DynamicByteBuffer_WriteNewline(dbb);

	DynamicByteBuffer_WriteI32(dbb, LEVELDATA_STRING_DATA_LENGTH);
	for (int32_t i = 0; i < LEVELDATA_STRING_DATA_LENGTH; i += 1)
	{
		DynamicByteBuffer_WriteString(dbb, ld->mStringData[i], EE_FILENAME_MAX);
	}

	DynamicByteBuffer_WriteNewline(dbb);

	DynamicByteBuffer_WriteI32(dbb, LEVELDATA_LAYER_DATA_LENGTH);
	for (int32_t i = 0; i < LEVELDATA_LAYER_DATA_LENGTH; i += 1)
	{
		LayerData_Write(&ld->mLayerData[i], dbb);
	}
}
void LevelData_ReadData(int32_t version, LevelData* ld, BufferReader* reader)
{
	//version = BufferReader_ReadI32(reader); //UNSUED

	if (version >= 3)
	{
		//BufferReader_ReadI32(reader); //Unused - Thing Instance ID Counter
	}

	int32_t saveTileSize = BufferReader_ReadI32(reader);
	for (int32_t i = 0; i < saveTileSize; i += 1)
	{
		Tile* tile = (Tile*)Utils_calloc(1, sizeof(Tile));
		Tile_Init(tile);
		Tile_Read(version, tile, reader);
		arrput(ld->arr_save_tiles, tile);
	}

	ld->tilemap.boundary.Width = BufferReader_ReadI32(reader);
	ld->tilemap.boundary.Height = BufferReader_ReadI32(reader);
	ld->_mSaveTileMap = BufferReader_ReadIntArray2D(reader, ld->tilemap.boundary.Width, ld->tilemap.boundary.Height);

	int32_t cameraDataSize = BufferReader_ReadI32(reader);
	for (int32_t i = 0; i < cameraDataSize; i += 1)
	{
		LevelCameraData cameraData = { 0 };
		LevelCameraData_Read(version, &cameraData, reader);
		arrput(ld->arr_camera_data, cameraData);
	}

	int32_t lineInfoSize = BufferReader_ReadI32(reader);
	for (int32_t i = 0; i < lineInfoSize; i += 1)
	{
		Line line = { 0 };
		Line_Read(version, &line, reader);
		arrput(ld->arr_lines, line);
	}
}
void LevelData_WriteData(LevelData* ld, DynamicByteBuffer* dbb)
{
	DynamicByteBuffer_WriteI32(dbb, (int32_t)arrlen(ld->arr_save_tiles));
	for (int32_t i = 0; i < arrlen(ld->arr_save_tiles); i += 1)
	{
		DynamicByteBuffer_WriteNewline(dbb);
		Tile_Write(ld->arr_save_tiles[i], dbb);
	}

	DynamicByteBuffer_WriteNewline(dbb);

	DynamicByteBuffer_WriteI32(dbb, ld->tilemap.boundary.Width);
	DynamicByteBuffer_WriteI32(dbb, ld->tilemap.boundary.Height);
	DynamicByteBuffer_WriteIntArray2D(dbb, ld->_mSaveTileMap, ld->tilemap.boundary.Width, ld->tilemap.boundary.Height);

	DynamicByteBuffer_WriteNewline(dbb);

	DynamicByteBuffer_WriteI32(dbb, (int32_t)arrlen(ld->arr_camera_data));
	for (int32_t i = 0; i < arrlen(ld->arr_camera_data); i += 1)
	{
		DynamicByteBuffer_WriteNewline(dbb);
		LevelCameraData_Write(&ld->arr_camera_data[i], dbb);
	}

	DynamicByteBuffer_WriteNewline(dbb);

	DynamicByteBuffer_WriteI32(dbb, (int32_t)arrlen(ld->arr_lines));
	for (int32_t i = 0; i < arrlen(ld->arr_lines); i += 1)
	{
		DynamicByteBuffer_WriteNewline(dbb);
		Line_Write(&ld->arr_lines[i], dbb);
	}
}
void LevelData_LoadSetupOffsets(LevelData* ld)
{
	/*
	if (Globals_IsDebugFileMode() && !GLOBALS_DEBUG_ENGINE_FORCE_LOAD_DATS) //DEBUG ONLY FOR NOW TODO
	{
		return;
	}

	for (int32_t i = 0; i < arrlen(ld->arr_save_tiles); i += 1)
	{
		Tile* t = ld->arr_save_tiles[i];
		for (int32_t j = 0; j < LEVELDATA_LAYER_DATA_LENGTH; j += 1)
		{
			TilesetOffset_LoadOffsetPoint(&t->mDrawTiles[j], ld->mTilesetName);
		}
	}*/
}
bool LevelData_LoadSetupTileData(LevelData* ld)
{
	Tile** tiles = ld->arr_save_tiles;
	int32_t* map = ld->_mSaveTileMap;
	int32_t gridWidth = LevelData_GetBoundary(ld).Width;
	int32_t gridHeight = LevelData_GetBoundary(ld).Height;
	Tilemap_Init(&ld->tilemap, Rectangle_Create(0, 0, gridWidth, gridHeight));
	for (int32_t i = 0; i < gridWidth; i += 1)
	{
		for (int32_t j = 0; j < gridHeight; j += 1)
		{
			int32_t gridPos = LevelData_GetTilePos1D(ld, i, j);
			int32_t tileLocInSaveTiles = map[gridPos];
			Tile* tileToSet = tiles[tileLocInSaveTiles];
			bool isDebugFileMode = true;
			//if (OeGlobals_IsDebugFileMode()) //UNUSED, DEBUG//TODO
			if (isDebugFileMode)
			{
				ld->tilemap.tiles[gridPos] = Tile_Clone(tileToSet);
				/* //TODO
				if (_mIsMetaMap && OeGlobals_DEBUG_IS_META_MAP_EDIT_TILE_MODE_AT_MAP_LOAD || !_mIsMetaMap) //if debug and editing, we use clones of tiles so they can be manipulated...
				{
					mTileData[gridPos] = tiles[map[gridPos]].GetClone();
				}
				else //otherwise we just use a reference...
				{
					mTileData[gridPos] = &tiles[map[gridPos]];
				}*/
			}
			else
			{
				ld->tilemap.tiles[gridPos] = tileToSet;
			}
		}
	}

	return true;
}
Rectangle LevelData_GetBoundary(LevelData* ld)
{
	return ld->tilemap.boundary;
}
Rectangle LevelData_GetBoundaryInPixels(LevelData* ld)
{
	return ld->tilemap.boundaryInPixels;
}
Tilemap* LevelData_GetTilemap(LevelData* ld)
{
	return &ld->tilemap;
}
Tile* LevelData_GetTile(LevelData* ld, int32_t x, int32_t y)
{
	return Tilemap_GetTile(&ld->tilemap, x, y);
}
int32_t* LevelData_CreateCollisionArray(LevelData* ld)
{
	int32_t* collisionArray = LevelData_CreateEmptyCollisionArray(ld);
	LevelData_ImprintToCollisionArray(ld, 0, 0, collisionArray);
	return collisionArray;
}
int32_t* LevelData_CreateEmptyCollisionArray(LevelData* ld)
{
	return (int32_t*)Utils_CallocArena(ld->tilemap.boundary.Width * ld->tilemap.boundary.Height, sizeof(int32_t), UTILS_ALLOCATION_ARENA_JUST_THIS_LEVEL);
}
void LevelData_ImprintToCollisionArray(LevelData* ld, int32_t x, int32_t y, int32_t* collisionArray)
{
	int32_t width = ld->tilemap.boundary.Width;
	int32_t height = ld->tilemap.boundary.Height;
	for (int32_t i = 0; i < width; i += 1)
	{
		for (int32_t j = 0; j < height; j += 1)
		{
			collisionArray[LevelData_GetTilePos1D(ld, x + i, y + j)] = ld->tilemap.tiles[LevelData_GetTilePos1D(ld, i, j)]->mCollisionType;
		}
	}
}
bool LevelData_IsTilesetNameSet(LevelData* ld)
{
	if (Utils_StringEqualTo(ld->mTilesetName, EE_STR_NOT_SET))
	{
		return false;
	}
	return true;
}
void LevelData_DrawTiles(LevelData* ld, SpriteBatch* spriteBatch, Camera* camera)
{
	LevelData_DrawTiles2(ld, spriteBatch, camera, -1, 1);
}
void LevelData_DrawTiles2(LevelData* ld, SpriteBatch* spriteBatch, Camera* camera, int32_t preferredLayer, float cameraMul)
{
#ifdef EDITOR_MODE
	if (!Cvars_GetAsBool(CVARS_EDITOR_SHOW_TILES))
	{
		return;
	}
#endif

	if (!LevelData_IsTilesetNameSet(ld))
	{
		return;
	}

	Texture* texture = LevelData_GetTilesetTexture(ld);
	if (texture == NULL)
	{
		return;
	}
	if (Texture_GetTexture2D(texture) == NULL)
	{
		return;
	}

	int32_t x1 = Camera_GetX1Mul(camera, cameraMul);
	int32_t x2 = Camera_GetX2Mul(camera, ld->tilemap.boundary.Width, cameraMul);
	int32_t y1 = Camera_GetY1Mul(camera, cameraMul);
	int32_t y2 = Camera_GetY2Mul(camera, ld->tilemap.boundary.Height, cameraMul);

	for (int32_t i = 0; i < LEVELDATA_LAYER_DATA_LENGTH; i += 1)
	{
		Color color = COLOR_WHITE;
		int32_t depth = ld->mLayerData[i].mDepth;
		if (preferredLayer >= 0)
		{
			if (preferredLayer == i)
			{
				depth = 100;
			}
			else
			{
				color = COLOR_GRAY;
			}
		}

		bool showLayer = true;
#ifdef EDITOR_MODE
		//showLayer = Cvars_GetAsBool(Cvars_GetEditorShowLayer(i)); //UNUSED
#endif
		if (showLayer)
		{
			SpriteBatch_DrawLayer(spriteBatch, texture, color, ld->tilemap.tiles, ld->tilemap.boundary, depth, i, x1, x2, y1, y2);
		}
	}

#ifdef EDITOR_MODE
	//DrawTileInfo(spriteBatch, preferredLayer, x1, x2, y1, y2); //UNUSED
#endif
}
Texture* LevelData_GetTilesetTexture(LevelData* ld)
{
	void* resourceData = ResourceMan_GetResourceData(ResourceList_Texture(), ld->mTilesetName);
	if (resourceData == NULL)
	{
		return NULL;
	}

	return (Texture*)resourceData;
}
void LevelData_DrawProps(LevelData* ld, SpriteBatch* spriteBatch, Camera* camera)
{
	LevelData_DrawProps2(ld, spriteBatch, camera, false);
}
void LevelData_DrawProps2(LevelData* ld, SpriteBatch* spriteBatch, Camera* camera, bool drawInfo)
{
#ifdef EDITOR_MODE
	if (!Cvars_GetAsBool(CVARS_EDITOR_SHOW_PROPS))
	{
		return;
	}
#endif

	int32_t dist = Cvars_GetAsInt(CVARS_ENGINE_PROP_DISTANCE);

	Rectangle boundary = LevelData_GetBoundary(ld);
	int32_t x1 = Camera_GetX1Mul(camera, (float)dist);
	int32_t x2 = Camera_GetX2Mul(camera, boundary.Width, (float)dist);
	int32_t y1 = Camera_GetY1Mul(camera, (float)dist);
	int32_t y2 = Camera_GetY2Mul(camera, boundary.Height, (float)dist);

	for (int32_t i = x1; i < x2; i += 1)
	{
		for (int32_t j = y1; j < y2; j += 1)
		{
			Tile* t = ld->tilemap.tiles[LevelData_GetTilePos1D(ld, i, j)];
			Tile_DrawProps2(t, spriteBatch, camera, i, j, false, drawInfo);
		}
	}
}
void LevelData_DrawCollision(LevelData* ld, SpriteBatch* spriteBatch, Camera* camera)
{
	arrsetlen(arr_many_rectangles, 0);

	if (!Cvars_GetAsBool(CVARS_EDITOR_SHOW_COLLISION) || !Globals_IsEditorActive())
	{
		return;
	}

	Rectangle boundary = LevelData_GetBoundary(ld);
	int x1 = Camera_GetX1(camera);
	int x2 = Camera_GetX2(camera, boundary.Width);
	int y1 = Camera_GetY1(camera);
	int y2 = Camera_GetY2(camera, boundary.Height);

	Rectangle rect = Rectangle_Empty;
	int lastType = -1;
	Tile** tileData = ld->tilemap.tiles;
	for (int j = y1; j < y2; j += 1)
	{
		for (int i = x1; i < x2; i += 1)
		{
			Tile* tile = tileData[LevelData_GetTilePos1D(ld, i, j)];
			int type = tile->mCollisionType;
			if ((type != lastType) || type == 0)
			{
				if (!Rectangle_IsEmpty(rect))
				{
					Color color = Utils_GetCollisionColor(lastType);
					arrput(arr_many_rectangles, DrawRectangle_Create(color, rect));
					rect = Rectangle_Empty;
				}
			}
			if (type != 0)
			{
				if (type == lastType)
				{
					rect.Width += TILE_SIZE;
				}
				else
				{
					rect = Tile_GetCollisionRectangle(tile, i, j);
				}
			}
			lastType = type;
		}
		if (!Rectangle_IsEmpty(rect))
		{
			Color color = Utils_GetCollisionColor(lastType);
			arrput(arr_many_rectangles, DrawRectangle_Create(color, rect));
		}
		rect = Rectangle_Empty;
		lastType = -1;
	}

	SpriteBatch_DrawManyRectangle(spriteBatch, 150, arr_many_rectangles);
}
void LevelData_DrawThings(LevelData* ld, SpriteBatch* spriteBatch, Camera* camera)
{
#if EDITOR_MODE
	if (!Cvars_GetAsBool(CVARS_EDITOR_SHOW_THINGS))
	{
		return;
	}
#endif

	Rectangle levelBoundary = LevelData_GetBoundary(ld);
	int x1 = Camera_GetX1(camera);
	int x2 = Camera_GetX2(camera, levelBoundary.Width);
	int y1 = Camera_GetY1(camera);
	int y2 = Camera_GetY2(camera, levelBoundary.Height);

	for (int i = x1; i < x2; i += 1)
	{
		for (int j = y1; j < y2; j += 1)
		{
			Tile* tile = LevelData_GetTile(ld, i, j);
			if (arrlen(tile->arr_instances) != 0)
			{
				Tile_DrawThings(tile, spriteBatch, camera, i, j, false);
			}
		}
	}
}
static void LevelData_OffsetStuff(LevelData* ld, bool isColumn, int target, int direction)
{
	for (int i = 0; i < arrlen(ld->arr_camera_data); i += 1)
	{
		LevelCameraData* data = &ld->arr_camera_data[i];
		LevelData_OffsetPoint(ld, &data->mVolumeTrigger.mPointOne, TILE_SIZE, isColumn, target, direction);
		LevelData_OffsetPoint(ld, &data->mVolumeTrigger.mPointTwo, TILE_SIZE, isColumn, target, direction);
		LevelData_OffsetPoint(ld, &data->mVolumeBounds.mPointOne, TILE_SIZE, isColumn, target, direction);
		LevelData_OffsetPoint(ld, &data->mVolumeBounds.mPointTwo, TILE_SIZE, isColumn, target, direction);
	}

	for (int i = 0; i < arrlen(ld->arr_lines); i += 1)
	{
		Line* line = &ld->arr_lines[i];
		LevelData_OffsetPoint(ld, &line->mBegin, 1, isColumn, target, direction);
		LevelData_OffsetPoint(ld, &line->mEnd, 1, isColumn, target, direction);
	}
}
void LevelData_DeleteColumn(LevelData* ld, int loc)
{
	if (ld->_mIsMetaMap)
	{
		return;
	}

	if (ld->tilemap.boundary.Width <= 1)
	{
		return;
	}

	Rectangle newBoundary = Rectangle_Create(0, 0, ld->tilemap.boundary.Width - 1, ld->tilemap.boundary.Height);
	Tilemap newData;
	Tilemap_Init(&newData, newBoundary);

	for (int i = 0; i < newData.boundary.Width; i += 1)
	{
		for (int j = 0; j < newData.boundary.Height; j += 1)
		{
			if (i < loc)
			{
				Tilemap_SetTile(&newData, i, j, LevelData_GetTile(ld, i, j));
			}
			else if (i >= loc)
			{
				Tilemap_SetTile(&newData, i, j, LevelData_GetTile(ld, i + 1, j));
			}
		}
	}

	ld->tilemap = newData;

	LevelData_OffsetStuff(ld, true, loc, -1);

	Logger_Log(LOGGER_INFORMATION, "Deleted column at " + loc);
}
void LevelData_DeleteRow(LevelData* ld, int loc)
{
	if (ld->_mIsMetaMap)
	{
		return;
	}

	if (ld->tilemap.boundary.Height <= 1)
	{
		return;
	}

	Rectangle newBoundary = Rectangle_Create(0, 0, ld->tilemap.boundary.Width, ld->tilemap.boundary.Height - 1);
	Tilemap newData;
	Tilemap_Init(&newData, newBoundary);

	for (int i = 0; i < newData.boundary.Width; i += 1)
	{
		for (int j = 0; j < newData.boundary.Height; j += 1)
		{
			if (j < loc)
			{
				Tilemap_SetTile(&newData, i, j, LevelData_GetTile(ld, i, j));
			}
			else if (j >= loc)
			{
				Tilemap_SetTile(&newData, i, j, LevelData_GetTile(ld, i, j + 1));
			}
		}
	}

	ld->tilemap = newData;

	LevelData_OffsetStuff(ld, false, loc, -1);

	Logger_Log(LOGGER_INFORMATION, "Deleted row at " + loc);
}
void LevelData_OffsetPoint(LevelData* ld, Point* point, int tileSizeMul, bool isColumn, int target, int direction)
{
	if (isColumn)
	{
		if (point->X >= target * tileSizeMul)
		{
			point->X += direction * tileSizeMul;
		}
	}
	else
	{
		if (point->Y >= target * tileSizeMul)
		{
			point->Y += direction * tileSizeMul;
		}
	}
}
void LevelData_AddColumn(LevelData* ld, int loc)
{
	if (ld->_mIsMetaMap)
	{
		return;
	}

	Rectangle newBoundary = Rectangle_Create(0, 0, ld->tilemap.boundary.Width + 1, ld->tilemap.boundary.Height);
	Tilemap newData;
	Tilemap_Init(&newData, newBoundary);

	for (int i = 0; i < newData.boundary.Width; i += 1)
	{
		for (int j = 0; j < newData.boundary.Height; j += 1)
		{
			if (i < loc)
			{
				Tilemap_SetTile(&newData, i, j, LevelData_GetTile(ld, i, j));
			}
			else if (i > loc)
			{
				Tilemap_SetTile(&newData, i, j, LevelData_GetTile(ld, i - 1, j));
			}
			else
			{
				Tilemap_SetTile(&newData, i, j, Tile_Create());
			}
		}
	}

	ld->tilemap = newData;

	LevelData_OffsetStuff(ld, true, loc, 1);

	Logger_Log(LOGGER_INFORMATION, "Added column at " + loc);
}
void LevelData_AddRow(LevelData* ld, int loc)
{
	if (ld->_mIsMetaMap)
	{
		return;
	}

	Rectangle newBoundary = Rectangle_Create(0, 0, ld->tilemap.boundary.Width, ld->tilemap.boundary.Height + 1);
	Tilemap newData;
	Tilemap_Init(&newData, newBoundary);

	for (int i = 0; i < newData.boundary.Width; i += 1)
	{
		for (int j = 0; j < newData.boundary.Height; j += 1)
		{
			if (j < loc)
			{
				Tilemap_SetTile(&newData, i, j, LevelData_GetTile(ld, i, j));
			}
			else if (j > loc)
			{
				Tilemap_SetTile(&newData, i, j, LevelData_GetTile(ld, i, j - 1));
			}
			else
			{
				Tilemap_SetTile(&newData, i, j, Tile_Create());
			}
		}
	}

	ld->tilemap = newData;

	LevelData_OffsetStuff(ld, false, loc, 1);

	Logger_Log(LOGGER_INFORMATION, "Added row at " + loc);
}
