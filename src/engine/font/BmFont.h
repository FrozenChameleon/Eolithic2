/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "BmFontData.h"
#include "../utils/MString.h"
#include "../font/FontMapData.h"

typedef struct SpriteBatch SpriteBatch;
typedef struct Texture Texture;
typedef struct TTFont TTFont;
typedef struct BufferReader BufferReader;

typedef struct BmFont
{
	char _mFontName[EE_FILENAME_MAX];
	BmFontData _mFontData;
	Texture* _mFontTexture;
	struct { int32_t key; BmFontDataChar value; }*hm_font_char_map;
} BmFont;

void BmFont_Init(BmFont* bmf, BufferReader* br, const char* fontName);
BmFont* BmFont_GetBmFont(BmFont* bmf, const char* font);
const FontMapData* BmFont_GetReplacement(BmFont* bmf);
const char* BmFont_GetFontName(BmFont* bmf);
Texture* BmFont_GetFontTexture(BmFont* bmf);
bool BmFont_UpdateFontTexture(BmFont* bmf);
int32_t BmFont_GetBaseHeight(BmFont* bmf);
int32_t BmFont_GetLineHeight(BmFont* bmf);
Rectangle BmFont_GetBounds(BmFont* bmf, const char* text, bool doNotReplaceFont);
// const std::string& GetFontExtension(); //UNUSED
// const std::vector<std::string>& GetFontDirectories(); //UNUSED
BmFont* BmFont_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);
void BmFont_Dispose(BmFont* bmf);