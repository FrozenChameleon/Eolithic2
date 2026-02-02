/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/MString.h"

typedef struct DatInfo
{
	MString* mPath;
	int64_t mLength;
	int64_t mPosition;
} DatInfo;

