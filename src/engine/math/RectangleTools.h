/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "Rectangle.h"

typedef struct BufferWriter BufferWriter;
typedef struct BufferReader BufferReader;

extern const Rectangle RectangleTools_NegativeOne;

void RectangleTools_Read(Rectangle* rectangle, BufferReader* reader);
