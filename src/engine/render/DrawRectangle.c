/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "DrawRectangle.h"

#include "../utils/Utils.h"

void DrawRectangle_Init(DrawRectangle* drect, Color color, Rectangle rectangle)
{
	Utils_memset(drect, 0, sizeof(DrawRectangle));

	drect->mColor = color;
	drect->mRectangle = rectangle;
}
DrawRectangle DrawRectangle_Create(Color color, Rectangle rectangle)
{
	DrawRectangle drect;
	DrawRectangle_Init(&drect, color, rectangle);
	return drect;
}
