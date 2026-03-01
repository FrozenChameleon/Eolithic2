/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "TextureOffset.h"

#include "../../third_party/stb_ds.h"
#include "../utils/Utils.h"
#include "../io/File.h"
#include "../utils/IStrings.h"
#include "../utils/IStringArray.h"

TextureOffset* TextureOffset_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	TextureOffset* textureOffset = (TextureOffset*)Utils_calloc(1, sizeof(TextureOffset));

	IStringArray* lines = IStringArray_CreateForJustThisFrame();
	File_ReadAllToStrings(br, lines);
	size_t linesLen = IStringArray_Length(lines);

	for (int32_t i = 0; i < linesLen; i += 1)
	{
		const char* line = IStringArray_Get(lines, i);
		IStringArray* spaceSplit = IStringArray_CreateForJustThisFrame();
		Utils_SplitString(line, Utils_strlen(line), ' ', spaceSplit);
		size_t spaceSplitLen = IStringArray_Length(spaceSplit);
		if (spaceSplitLen != 6)
		{
			continue;
		}

		Rectangle rect;
		rect.X = Utils_ParseInt(IStringArray_Get(spaceSplit, 2));
		rect.Y = Utils_ParseInt(IStringArray_Get(spaceSplit, 3));
		rect.Width = Utils_ParseInt(IStringArray_Get(spaceSplit, 4));
		rect.Height = Utils_ParseInt(IStringArray_Get(spaceSplit, 5));

		TextureOffsetInfo info = { 0 };
		info.mRect = rect;
		Utils_strlcpy(info.mPath, path, EE_FILENAME_MAX);
		Utils_strlcpy(info.mVirtualName, IStringArray_Get(spaceSplit, 0), EE_FILENAME_MAX);
		Utils_strlcpy(info.mFilenameWithoutExtension, filenameWithoutExtension, EE_FILENAME_MAX);

		arrput(textureOffset->arr_offsets, info);
	}

	return textureOffset;
}
void TextureOffset_Dispose(TextureOffset* to)
{

}
