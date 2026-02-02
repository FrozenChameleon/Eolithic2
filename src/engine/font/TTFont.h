/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"

typedef struct BufferReader BufferReader;

typedef struct TTFont
{
	uint8_t dummy;
} TTFont;

TTFont* TTFont_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);
void TTFont_Dispose(TTFont* ttf);