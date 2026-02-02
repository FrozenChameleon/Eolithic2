/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"

typedef struct DrawTile DrawTile;
typedef struct BufferReader BufferReader;

typedef struct TilesetOffset
{
	uint8_t mDummy;
} TilesetOffset;

TilesetOffset* TilesetOffset_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);
void TilesetOffset_Dispose(TilesetOffset* to);
void TilesetOffset_LoadOffsetPoint(DrawTile* drawTile, const char* tilesetName);