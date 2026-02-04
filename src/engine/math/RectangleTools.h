/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "Rectangle.h"

typedef struct BufferReader BufferReader;
typedef struct DynamicByteBuffer DynamicByteBuffer;

extern const Rectangle RectangleTools_NegativeOne;

void RectangleTools_Read(Rectangle* rectangle, BufferReader* br);
void RectangleTools_Write(Rectangle* rectangle, DynamicByteBuffer* dbb);