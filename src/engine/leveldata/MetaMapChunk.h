/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "../math/Point.h"
#include "../resources/ResourceManager.h"
#include "../render/Color.h"
#include "../utils/MString.h"

typedef struct MetaMapChunk
{
	Point _mPosition;
	Resource* _mLevelDataResource;
	int32_t mMyId;
	Color mColor;
	char mFileName[EE_FILENAME_MAX];
} MetaMapChunk;

