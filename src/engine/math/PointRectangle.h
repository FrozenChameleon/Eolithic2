/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "Point.h"
#include "Rectangle.h"

typedef struct BufferReader BufferReader;
typedef struct DynamicByteBuffer DynamicByteBuffer;

typedef struct PointRectangle
{
	Point mPointOne;
	Point mPointTwo;
} PointRectangle;

bool PointRectangle_IsEmpty(PointRectangle* prect);
void PointRectangle_CorrectPoints(PointRectangle* prect);
void PointRectangle_Read(PointRectangle* prect, BufferReader* br);
void PointRectangle_Write(PointRectangle* prect, DynamicByteBuffer* dbb);
Rectangle PointRectangle_GetRectangle(PointRectangle* prect);
Rectangle PointRectangle_GetRectanglePoint(Point pointOne, Point pointTwo);
void PointRectangle_Add(PointRectangle* prect, int32_t amountX, int32_t amountY);