/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "BmFontDataInfo.h"
#include "BmFontDataCommon.h"
#include "BmFontDataPage.h"
#include "BmFontDataChar.h"
#include "BmFontDataKerning.h"
#include "../io/File.h"
#include "../utils/Utils.h"
#include "../utils/IStringArray.h"
#include "../io/BufferReader.h"

typedef struct BmFontData
{
	BmFontDataInfo mInfo;
	BmFontDataCommon mCommon;
	BmFontDataPage* mPages;
	BmFontDataChar* mChars;
	BmFontDataKerning* mKernings;
} BmFontData;

void BmFontData_Load(BmFontData* bmfd, BufferReader* br);
void BmFontData_ReadInfo(BmFontData* bmfd, IStringArray* tagSplit);
void BmFontData_ReadCommon(BmFontData* bmfd, IStringArray* tagSplit);
void BmFontData_ReadPage(BmFontData* bmfd, IStringArray* tagSplit);
void BmFontData_ReadChar(BmFontData* bmfd, IStringArray* tagSplit);
void BmFontData_ReadKerning(BmFontData* bmfd, IStringArray* tagSplit);
int32_t BmFontData_FindInt(const char* key, IStringArray* tagSplit);
const char* BmFontData_FindString(const char* key, IStringArray* tagSplit);
void BmFontData_Init(BmFontData* bmfd, BufferReader* br);
