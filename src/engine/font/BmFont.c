/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "BmFont.h"

#include "FontMap.h"
#include "../render/Texture.h"
#include "../render/RenderTTFont.h"
#include "../render/Renderer.h"
#include "../../third_party/stb_ds.h"
#include "../resources/ResourceManagerList.h"
#include "../font/TTFont.h"

#define FONT_EXTENSION ".fnt"

int32_t BmFont_GetBaseHeight(BmFont* bmf)
{
	return bmf->_mFontData.mCommon.Base;
}
int32_t BmFont_GetLineHeight(BmFont* bmf)
{
	return bmf->_mFontData.mCommon.LineHeight;
}
const char* BmFont_GetFontName(BmFont* bmf)
{
	return bmf->_mFontName;
}
Texture* BmFont_GetFontTexture(BmFont* bmf)
{
	BmFont_UpdateFontTexture(bmf);

	return bmf->_mFontTexture;
}
bool BmFont_UpdateFontTexture(BmFont* bmf)
{
	if (bmf->_mFontTexture == NULL)
	{
		Texture* tex = (Texture*)ResourceManager_GetResourceData(ResourceManagerList_FontTexture(), bmf->_mFontName);
		if (tex == NULL)
		{
			bmf->_mFontTexture = NULL;
			return true;
		}
		bmf->_mFontTexture = tex;
	}
	return false;
}
void BmFont_Init(BmFont* bmf, BufferReader* br, const char* fontName)
{
	Utils_memset(bmf, 0, sizeof(BmFont));

	BmFontData_Init(&bmf->_mFontData, br);
	Utils_strlcpy(bmf->_mFontName, fontName, EE_FILENAME_MAX);

	BmFontDataChar* chars = bmf->_mFontData.mChars;
	int64_t charsLen = arrlen(chars);
	for (int32_t i = 0; i < charsLen; i += 1)
	{
		BmFontDataChar fontChar = chars[i];
		hmput(bmf->hm_font_char_map, fontChar.ID, fontChar);
	}
}
BmFont* BmFont_GetBmFont(BmFont* bmf, const char* font)
{
	return (BmFont*)ResourceManager_GetResourceData(ResourceManagerList_Font(), font);
}
const FontMapData* BmFont_GetReplacement(BmFont* bmf)
{
	return FontMap_GetReplacement(bmf->_mFontName);
}

#ifndef DISABLE_TT_FONT
static TTFont* BmFont_GetTTFont(const char* font)
{
	return (TTFont*)ResourceManager_GetResourceData(ResourceManagerList_TTFont(), font);
}
#endif

Rectangle BmFont_GetBounds(BmFont* bmf, const char* text, bool doNotReplaceFont)
{
	#ifndef DISABLE_TT_FONT
	const FontMapData* replacement = BmFont_GetReplacement(bmf);
	if ((replacement != NULL) && !doNotReplaceFont)
	{
		if (replacement->mIsBitmap)
		{
			return Renderer_RenderBmFont(false, BmFont_GetBmFont(bmf, replacement->mFontName), text, COLOR_WHITE, Vector2_Zero);
		}
		else
		{
			return RenderTTFont_GetBounds(replacement, text);
		}
	}
	#endif
	return Renderer_RenderBmFont(false, bmf, text, COLOR_WHITE, Vector2_Zero);
}

BmFont* BmFont_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	BmFont* bmf = (BmFont*)Utils_calloc(1, sizeof(BmFont));
	BmFont_Init(bmf, br, filenameWithoutExtension);
	return bmf;
}

void BmFont_Dispose(BmFont* bmf)
{
	//UNUSED FOR NOW
}