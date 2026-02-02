/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "../math/Rectangle.h"

typedef struct TextureOffsetInfo
{
	Rectangle mRect;
	char mPath[EE_FILENAME_MAX];
	char mVirtualName[EE_FILENAME_MAX];
	char mFilenameWithoutExtension[EE_FILENAME_MAX];
} TextureOffsetInfo;
