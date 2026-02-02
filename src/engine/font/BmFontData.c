/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "BmFontData.h"

#include "../../third_party/stb_ds.h"

static MString* _mTempString;
static char SEPARATOR_SPACE = ' ';

#define DUMMY_STR_NOTHING "!NOTHING_777"

void BmFontData_Load(BmFontData* bmfd, BufferReader* br)
{
	IStringArray* listOfStrings = File_ReadAllToStrings(br);

	for (int32_t i = 0; i < IStringArray_Length(listOfStrings); i += 1)
	{
		const char* line = IStringArray_Get(listOfStrings, i);
		if (Utils_StringEqualTo(line, EE_STR_EMPTY))
		{
			continue;
		}

		{
			IStringArray* tagSplit = IStringArray_Create();

			Utils_SplitString(line, Utils_strlen(line), SEPARATOR_SPACE, tagSplit);

			const char* tag = IStringArray_Get(tagSplit, 0);
			if (Utils_StringEqualTo(tag, "info"))
			{
				BmFontData_ReadInfo(bmfd, tagSplit);
			}
			else if (Utils_StringEqualTo(tag, "common"))
			{
				BmFontData_ReadCommon(bmfd, tagSplit);
			}
			else if (Utils_StringEqualTo(tag, "page"))
			{
				BmFontData_ReadPage(bmfd, tagSplit);
			}
			else if (Utils_StringEqualTo(tag, "char"))
			{
				BmFontData_ReadChar(bmfd, tagSplit);
			}
			else if (Utils_StringEqualTo(tag, "kerning"))
			{
				BmFontData_ReadKerning(bmfd, tagSplit);
			}

			IStringArray_Dispose(tagSplit);
		}
	}

	IStringArray_Dispose(listOfStrings);
}
void BmFontData_ReadInfo(BmFontData* bmfd, IStringArray* tagSplit)
{
	Utils_strlcpy(bmfd->mInfo.Face, BmFontData_FindString("face", tagSplit), EE_FILENAME_MAX);
	bmfd->mInfo.Size = BmFontData_FindInt("size", tagSplit);
	bmfd->mInfo.Bold = BmFontData_FindInt("bold", tagSplit);
	bmfd->mInfo.Italic = BmFontData_FindInt("italic", tagSplit);
	Utils_strlcpy(bmfd->mInfo.Charset, BmFontData_FindString("charset", tagSplit), EE_FILENAME_MAX);
	bmfd->mInfo.Unicode = BmFontData_FindInt("unicode", tagSplit);
	bmfd->mInfo.StretchH = BmFontData_FindInt("stretchH", tagSplit);
	bmfd->mInfo.Smooth = BmFontData_FindInt("smooth", tagSplit);
	bmfd->mInfo.AA = BmFontData_FindInt("aa", tagSplit);

	{
		const char* padding = BmFontData_FindString("padding", tagSplit);
		if (!Utils_StringEqualTo(padding, DUMMY_STR_NOTHING))
		{
			{
				IStringArray* paddingSplit = IStringArray_Create();
				Utils_SplitString(padding, Utils_strlen(padding), ',', paddingSplit);
				bmfd->mInfo.PaddingUp = Utils_ParseInt(IStringArray_Get(paddingSplit, 0));
				bmfd->mInfo.PaddingRight = Utils_ParseInt(IStringArray_Get(paddingSplit, 1));
				bmfd->mInfo.PaddingDown = Utils_ParseInt(IStringArray_Get(paddingSplit, 2));
				bmfd->mInfo.PaddingLeft = Utils_ParseInt(IStringArray_Get(paddingSplit, 3));
				IStringArray_Dispose(paddingSplit);
			}
		}
	}
	{
		const char* spacing = BmFontData_FindString("spacing", tagSplit);
		if (!Utils_StringEqualTo(spacing, DUMMY_STR_NOTHING))
		{
			{
				IStringArray* spacingSplit = IStringArray_Create();
				Utils_SplitString(spacing, Utils_strlen(spacing), ',', spacingSplit);
				bmfd->mInfo.SpacingHoriz = Utils_ParseInt(IStringArray_Get(spacingSplit, 0));
				bmfd->mInfo.SpacingVert = Utils_ParseInt(IStringArray_Get(spacingSplit, 1));
				IStringArray_Dispose(spacingSplit);
			}
		}
	}
	bmfd->mInfo.Outline = BmFontData_FindInt("outline", tagSplit);
}
void BmFontData_ReadCommon(BmFontData* bmfd, IStringArray* tagSplit)
{
	bmfd->mCommon.LineHeight = BmFontData_FindInt("lineHeight", tagSplit);
	bmfd->mCommon.Base = BmFontData_FindInt("base", tagSplit);
	bmfd->mCommon.ScaleW = BmFontData_FindInt("scaleW", tagSplit);
	bmfd->mCommon.ScaleH = BmFontData_FindInt("scaleH", tagSplit);
	bmfd->mCommon.Pages = BmFontData_FindInt("pages", tagSplit);
	bmfd->mCommon.Packed = BmFontData_FindInt("packed", tagSplit);
	bmfd->mCommon.AlphaChnl = BmFontData_FindInt("alphaChnl", tagSplit);
	bmfd->mCommon.RedChnl = BmFontData_FindInt("redChnl", tagSplit);
	bmfd->mCommon.GreenChnl = BmFontData_FindInt("greenChnl", tagSplit);
	bmfd->mCommon.BlueChnl = BmFontData_FindInt("blueChnl", tagSplit);
}
void BmFontData_ReadPage(BmFontData* bmfd, IStringArray* tagSplit)
{
	BmFontDataPage bmPage = { 0 };
	bmPage.ID = BmFontData_FindInt("id", tagSplit);
	Utils_strlcpy(bmPage.File, BmFontData_FindString("file", tagSplit), EE_FILENAME_MAX);
	arrput(bmfd->mPages, bmPage);
}
void BmFontData_ReadChar(BmFontData* bmfd, IStringArray* tagSplit)
{
	BmFontDataChar bmChar = { 0 };
	bmChar.ID = BmFontData_FindInt("id", tagSplit);
	bmChar.X = BmFontData_FindInt("x", tagSplit);
	bmChar.Y = BmFontData_FindInt("y", tagSplit);
	bmChar.Width = BmFontData_FindInt("width", tagSplit);
	bmChar.Height = BmFontData_FindInt("height", tagSplit);
	bmChar.XOffset = BmFontData_FindInt("xoffset", tagSplit);
	bmChar.YOffset = BmFontData_FindInt("yoffset", tagSplit);
	bmChar.XAdvance = BmFontData_FindInt("xadvance", tagSplit);
	bmChar.Page = BmFontData_FindInt("page", tagSplit);
	bmChar.Chnl = BmFontData_FindInt("chnl", tagSplit);
	arrput(bmfd->mChars, bmChar);
}
void BmFontData_ReadKerning(BmFontData* bmfd, IStringArray* tagSplit)
{
	BmFontDataKerning bmKerning = { 0 };
	bmKerning.First = BmFontData_FindInt("first", tagSplit);
	bmKerning.Second = BmFontData_FindInt("second", tagSplit);
	bmKerning.Amount = BmFontData_FindInt("amount", tagSplit);
	arrput(bmfd->mKernings, bmKerning);
}
int32_t BmFontData_FindInt(const char* key, IStringArray* tagSplit)
{
	const char* str = BmFontData_FindString(key, tagSplit);
	if (Utils_StringEqualTo(str, DUMMY_STR_NOTHING))
	{
		return 0;
	}
	int32_t value = Utils_ParseInt(str);
	return value;
}
const char* BmFontData_FindString(const char* key, IStringArray* tagSplit)
{
	for (int32_t i = 0; i < IStringArray_Length(tagSplit); i += 1)
	{
		const char* str = IStringArray_Get(tagSplit, i);
		bool strContains = Utils_StringContains(str, key);
		if (strContains)
		{
			int32_t index = Utils_StringIndexOf('=', str, Utils_strlen(str), false);
			if (index != -1)
			{
				int32_t strLen = (int32_t)Utils_strlen(str);
				MString_AssignSubString(&_mTempString, str, index + 1, strLen - index - 1);
				return MString_Text(_mTempString);
			}
		}
	}
	return DUMMY_STR_NOTHING;
}
void BmFontData_Init(BmFontData* bmfd, BufferReader* br)
{
	Utils_memset(bmfd, 0, sizeof(BmFontData));

	BmFontData_Load(bmfd, br);
}
