/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "TextureOffsetInfo.h"

#include "../io/BufferReader.h"

typedef struct TextureOffset
{
	TextureOffsetInfo* arr_offsets;
} TextureOffset;

TextureOffset* TextureOffset_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);
void TextureOffset_Dispose(TextureOffset* to);