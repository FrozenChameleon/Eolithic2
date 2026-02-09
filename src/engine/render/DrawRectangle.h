/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "Color.h"
#include "../math/Rectangle.h"

typedef struct DrawRectangle
{
	Color mColor;
	Rectangle mRectangle;
} DrawRectangle;

void DrawRectangle_Init(DrawRectangle* drect, Color color, Rectangle rectangle);
DrawRectangle DrawRectangle_Create(Color color, Rectangle rectangle);