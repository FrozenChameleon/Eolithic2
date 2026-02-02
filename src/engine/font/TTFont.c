/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "TTFont.h"

#include "../render/RenderTTFont.h"
#include "../io/File.h"
#include "../utils/Utils.h"
#include "../io/BufferReader.h"

TTFont* TTFont_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	RenderTTFont_Create(filenameWithoutExtension, BufferReader_GetBuffer(br));
	TTFont* ttf = (TTFont*)Utils_calloc(1, sizeof(TTFont));
	return ttf;
}

void TTFont_Dispose(TTFont* ttf)
{
	Utils_free(ttf);
}