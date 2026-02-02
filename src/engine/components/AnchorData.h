/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../../engine/utils/Macros.h"

typedef struct AnchorData
{
	char mAnchorName[EE_FILENAME_MAX];
	int32_t mAnchorNumber;
	int32_t mAnchorEntityNumber;
} AnchorData;

