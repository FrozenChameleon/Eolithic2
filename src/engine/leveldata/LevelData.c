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
#include "../resources/ResourceManagerList.h"
#include "../utils/MString.h"
#include "../render/TilesetOffset.h"
#include "../utils/Utils.h"

#define VERSION_LEVELDATA 12

#define TILE_SIZE GLOBAL_DEF_TILE_SIZE

LevelData* LevelData_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	LevelData* levelData = (LevelData*)Utils_calloc(1, sizeof(LevelData));
	LevelData_Read(levelData, br);
	return levelData;
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
		//	OeLogger_LogInformation("Correcting custom data amount...");

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
void LevelData_Write(LevelData* ld, DynamicByteBuffer* dbb)
{
	DynamicByteBuffer_WriteMagicNumber(dbb);
	DynamicByteBuffer_WriteVersionNumber(dbb, VERSION_LEVELDATA);

	LevelData_WriteHeader(ld, dbb);

	DynamicByteBuffer_WriteNewline(dbb);

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

	ld->_mGridSize.Width = BufferReader_ReadI32(reader);
	ld->_mGridSize.Height = BufferReader_ReadI32(reader);
	ld->_mSaveTileMap = BufferReader_ReadIntArray2D(reader, ld->_mGridSize.Width, ld->_mGridSize.Height);

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

	DynamicByteBuffer_WriteI32(dbb, ld->_mGridSize.Width);
	DynamicByteBuffer_WriteI32(dbb, ld->_mGridSize.Height);
	DynamicByteBuffer_WriteIntArray2D(dbb, ld->_mSaveTileMap, ld->_mGridSize.Width, ld->_mGridSize.Height);

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
	if (Globals_IsDebugFileMode() && !GLOBALS_DEBUG_ENGINE_FORCE_LOAD_DATS)
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
	}
}
bool LevelData_LoadSetupTileData(LevelData* ld)
{
	Tile** tiles = ld->arr_save_tiles;
	int32_t* map = ld->_mSaveTileMap;
	int32_t gridWidth = LevelData_GetGridSizeWidth(ld);
	int32_t gridHeight = LevelData_GetGridSizeHeight(ld);
	int32_t tileDataLen = gridWidth * gridHeight;
	ld->mTileData = (Tile**)Utils_calloc(tileDataLen, sizeof(Tile*));
	for (int32_t i = 0; i < gridWidth; i += 1)
	{
		for (int32_t j = 0; j < gridHeight; j += 1)
		{
			int32_t gridPos = LevelData_GetTilePos1D(ld, i, j);
			/*if (OeGlobals_IsDebugFileMode()) //UNUSED, DEBUG
			{
				if (_mIsMetaMap && OeGlobals_DEBUG_IS_META_MAP_EDIT_TILE_MODE_AT_MAP_LOAD || !_mIsMetaMap) //if debug and editing, we use clones of tiles so they can be manipulated...
				{
					 mTileData[gridPos] = tiles[map[gridPos]].GetClone();
				}
				else //otherwise we just use a reference...
				{
					mTileData[gridPos] = &tiles[map[gridPos]];
				}
			}
			else
			{*/
			int32_t tileLocInSaveTiles = map[gridPos];
			Tile* tileToSet = tiles[tileLocInSaveTiles];
			ld->mTileData[gridPos] = tileToSet;
			//}
		}
	}

	return true;
}
int32_t LevelData_GetGridSizeWidth(LevelData* ld)
{
	return ld->_mGridSize.Width;
}
int32_t LevelData_GetGridSizeHeight(LevelData* ld)
{
	return ld->_mGridSize.Height;
}
int32_t LevelData_GetGridSizeX(LevelData* ld)
{
	return ld->_mGridSize.Width;
}
int32_t LevelData_GetGridSizeY(LevelData* ld)
{
	return ld->_mGridSize.Height;
}
int32_t LevelData_GetRealSizeWidth(LevelData* ld)
{
	return ld->_mGridSize.Width * TILE_SIZE;
}
int32_t LevelData_GetRealSizeHeight(LevelData* ld)
{
	return ld->_mGridSize.Height * TILE_SIZE;
}
int32_t LevelData_GetRealSizeX(LevelData* ld)
{
	return ld->_mGridSize.Width * TILE_SIZE;
}
int32_t LevelData_GetRealSizeY(LevelData* ld)
{
	return ld->_mGridSize.Height * TILE_SIZE;
}
int32_t LevelData_GetTilePos1D(LevelData* ld, int32_t i, int32_t j)
{
	return i + (j * ld->_mGridSize.Width);
}
Tile* LevelData_GetTile(LevelData* ld, int32_t x, int32_t y)
{
	if (!LevelData_IsSafe(ld, x, y))
	{
		return NULL;
	}

	return ld->mTileData[LevelData_GetTilePos1D(ld, x, y)];
}
Tile* LevelData_GetTilePoint(LevelData* ld, Point p)
{
	return LevelData_GetTile(ld, p.X, p.Y);
}
bool LevelData_IsSafe(LevelData* ld, int32_t x, int32_t y)
{
	if ((x < 0) || (y < 0) || (x >= ld->_mGridSize.Width) || (y >= ld->_mGridSize.Height))
	{
		return false;
	}
	return true;
}
int32_t* LevelData_CreateCollisionArray(LevelData* ld)
{
	int32_t* collisionArray = LevelData_CreateEmptyCollisionArray(ld);
	LevelData_ImprintToCollisionArray(ld, 0, 0, collisionArray);
	return collisionArray;
}
int32_t* LevelData_CreateEmptyCollisionArray(LevelData* ld)
{
	return (int32_t*)Utils_CallocArena(ld->_mGridSize.Width * ld->_mGridSize.Height, sizeof(int32_t), UTILS_ALLOCATION_ARENA_JUST_THIS_LEVEL);
}
void LevelData_ImprintToCollisionArray(LevelData* ld, int32_t x, int32_t y, int32_t* collisionArray)
{
	int32_t width = ld->_mGridSize.Width;
	int32_t height = ld->_mGridSize.Height;
	for (int32_t i = 0; i < width; i += 1)
	{
		for (int32_t j = 0; j < height; j += 1)
		{
			collisionArray[LevelData_GetTilePos1D(ld, x + i, y + j)] = ld->mTileData[LevelData_GetTilePos1D(ld, i, j)]->mCollisionType;
		}
	}
}
Rectangle LevelData_GetLevelBoundsRectangle(LevelData* ld)
{
	return Rectangle_Create(0, 0, LevelData_GetRealSizeX(ld), LevelData_GetRealSizeY(ld));
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
	int32_t preferredLayer = -1;
	float mul = 1;

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

	int32_t x1 = Camera_GetX1Mul(camera, mul);
	int32_t x2 = Camera_GetX2Mul(camera, ld->_mGridSize.Width, mul);
	int32_t y1 = Camera_GetY1Mul(camera, mul);
	int32_t y2 = Camera_GetY2Mul(camera, ld->_mGridSize.Height, mul);

	for (int32_t i = 0; i < LEVELDATA_LAYER_DATA_LENGTH; i += 1)
	{
		Color color = COLOR_WHITE;
		int32_t depth = ld->mLayerData[i].mDepth;
		if (preferredLayer != -1)
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
			SpriteBatch_DrawLayer(spriteBatch, texture, color, ld->mTileData, ld->_mGridSize, depth, i, x1, x2, y1, y2);
		}
	}

#ifdef EDITOR_MODE
	//DrawTileInfo(spriteBatch, preferredLayer, x1, x2, y1, y2); //UNUSED
#endif
}
Texture* LevelData_GetTilesetTexture(LevelData* ld)
{
	Resource* resource = ResourceManager_GetResource(ResourceManagerList_Texture(), ld->mTilesetName);
	if ((resource == NULL) || (resource->mData == NULL))
	{
		return NULL;
	}
	return (Texture*)resource->mData;
}
void LevelData_DrawProps(LevelData* ld, SpriteBatch* spriteBatch, Camera* camera)
{
	bool drawInfo = false;

#ifdef EDITOR_MODE
	if (!Cvars_GetAsBool(CVARS_EDITOR_SHOW_PROPS))
	{
		return;
	}
#endif

	int32_t dist = Cvars_GetAsInt(CVARS_ENGINE_PROP_DISTANCE);

	int32_t x1 = Camera_GetX1Mul(camera, (float)dist);
	int32_t x2 = Camera_GetX2Mul(camera, LevelData_GetGridSizeWidth(ld), (float)dist);
	int32_t y1 = Camera_GetY1Mul(camera, (float)dist);
	int32_t y2 = Camera_GetY2Mul(camera, LevelData_GetGridSizeHeight(ld), (float)dist);

	for (int32_t i = x1; i < x2; i += 1)
	{
		for (int32_t j = y1; j < y2; j += 1)
		{
			Tile* t = ld->mTileData[LevelData_GetTilePos1D(ld, i, j)];
			Tile_DrawProps2(t, spriteBatch, camera, i, j, false, drawInfo);
		}
	}
}
