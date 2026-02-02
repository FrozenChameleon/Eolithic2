/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"

typedef struct BmFontDataInfo
{
	char Face[EE_FILENAME_MAX];
	int32_t Size;
	int32_t Bold;
	int32_t Italic;
	char Charset[EE_FILENAME_MAX];
	int32_t Unicode;
	int32_t StretchH;
	int32_t Smooth;
	int32_t AA;
	int32_t PaddingUp;
	int32_t PaddingRight;
	int32_t PaddingDown;
	int32_t PaddingLeft;
	int32_t SpacingHoriz;
	int32_t SpacingVert;
	int32_t Outline;
} BmFontDataInfo;