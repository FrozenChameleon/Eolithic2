/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "stdint.h"
#include "../utils/MString.h"

typedef struct Resource
{
	void* mData;
	ResourceID mID;
	MString* mPath;
	char mFileNameWithoutExtension[EE_FILENAME_MAX];
} Resource;