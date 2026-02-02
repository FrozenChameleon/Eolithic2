/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"

typedef struct BmFontDataPage
{
	int32_t ID;
	char File[EE_FILENAME_MAX];
} BmFontDataPage;