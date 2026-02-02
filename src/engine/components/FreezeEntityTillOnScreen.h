/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Rectangle.h"

typedef struct FreezeEntityTillOnScreen
{
	Rectangle mRectangle;
	bool mFacesPlayer;
	bool mIsFacingFlipped;
} FreezeEntityTillOnScreen;

