/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Point.h"

typedef struct CollisionImprintData
{
	Point mPosition;
	int32_t mWidth;
	int32_t mHeight;
	int32_t mType;
} CollisionImprintData;
