/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "TilesetOffset.h"

#include "../utils/Macros.h"
#include "../io/File.h"
#include "../math/Point.h"
#include "../leveldata/DrawTile.h"
#include "../utils/Logger.h"
#include "../utils/Utils.h"
#include "../../third_party/stb_ds.h"
#include "../utils/IStringArray.h"

#define TILE_SIZE GLOBAL_DEF_TILE_SIZE

typedef struct HmInnerMap
{
	int32_t key;
	Point value;
} HmInnerMap;

static struct { char* key; HmInnerMap* value; }*sh_draw_tile_offset;
static struct { char* key; Point value; }*sh_draw_tile_original;

static bool _mHasInit;
static void Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	sh_new_arena(sh_draw_tile_offset);
	sh_new_arena(sh_draw_tile_original);

	_mHasInit = true;
}


TilesetOffset* TilesetOffset_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	Init();

	const char* tilesetName = filenameWithoutExtension;

	if (shgeti(sh_draw_tile_offset, tilesetName) >= 0)
	{
		return (TilesetOffset*)Utils_calloc(1, sizeof(TilesetOffset));
	}

	IStringArray* strings = File_ReadAllToStrings(br);

	shput(sh_draw_tile_offset, tilesetName, NULL);

	int32_t originalWidth = Utils_ParseInt(IStringArray_Get(strings, 0));
	int32_t originalHeight = Utils_ParseInt(IStringArray_Get(strings, 1));
	shput(sh_draw_tile_original, tilesetName, Point_Create(originalWidth, originalHeight));

	int64_t stringsLen = IStringArray_Length(strings);
	for (int32_t i = 2; i < stringsLen; i += 1)
	{
		const char* nextLine = IStringArray_Get(strings, i);
		if (Utils_StringEqualTo(nextLine, EE_STR_EMPTY))
		{
			continue;
		}

		IStringArray* addToHere = IStringArray_Create();
		Utils_SplitString(nextLine, Utils_strlen(nextLine), ',', addToHere);
		if (IStringArray_Length(addToHere) != 3)
		{
			IStringArray_Dispose(addToHere);
			continue;
		}

		int32_t index = Utils_ParseInt(IStringArray_Get(addToHere, 0));
		int32_t x = Utils_ParseInt(IStringArray_Get(addToHere, 1));
		int32_t y = Utils_ParseInt(IStringArray_Get(addToHere, 2));
		int64_t sh_index = shgeti(sh_draw_tile_offset, tilesetName);
		hmput(sh_draw_tile_offset[sh_index].value, index, Point_Create(x, y));
		IStringArray_Dispose(addToHere);
	}

	IStringArray_Dispose(strings);

	return (TilesetOffset*)Utils_calloc(1, sizeof(TilesetOffset));
}
void TilesetOffset_Dispose(TilesetOffset* to)
{
	Utils_free(to);
}

void TilesetOffset_LoadOffsetPoint(DrawTile* drawTile, const char* tilesetName)
{
	drawTile->mOffsetPoint = Point_Zero;

	if (Utils_StringEqualTo(tilesetName, EE_STR_EMPTY))
	{
		Logger_LogInformation("Offset point map not found for tileset (tileset null)");
		return;
	}

	if (Utils_StringEqualTo(tilesetName, EE_STR_NOT_SET))
	{
		return;
	}

	int64_t offset_index = shgeti(sh_draw_tile_offset, tilesetName);
	if (offset_index < 0)
	{
		Logger_LogInformation("Offset point map not found for tileset (tileset not found)");
		return;
	}

	int64_t original_index = shgeti(sh_draw_tile_original, tilesetName);
	if (original_index < 0)
	{
		Logger_LogInformation("Original point map not found for tileset (tileset not found)");
		return;
	}

	HmInnerMap* hm_offset_map_ref = sh_draw_tile_offset[offset_index].value;
	int64_t hm_offset_map_ref_len = hmlen(hm_offset_map_ref);

	int32_t offset = sh_draw_tile_original[original_index].value.X / TILE_SIZE;
	int32_t loc = (drawTile->mPoint.X / TILE_SIZE) + ((drawTile->mPoint.Y / TILE_SIZE) * offset);
	if (loc > (hm_offset_map_ref_len - 1))
	{
		Logger_LogInformation("Offset point location mismatch.");
		return;
	}

	Point pointToReturn = hm_offset_map_ref[loc].value;
	drawTile->mOffsetPoint = pointToReturn;
}