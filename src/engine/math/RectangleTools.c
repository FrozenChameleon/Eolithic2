/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "RectangleTools.h"

#include "../io/BufferReader.h"

const Rectangle RectangleTools_NegativeOne = { -1, -1, 0, 0 };

void RectangleTools_Read(Rectangle* rectangle, BufferReader* reader)
{
	rectangle->X = BufferReader_ReadI32(reader);
	rectangle->Y = BufferReader_ReadI32(reader);
	rectangle->Width = BufferReader_ReadI32(reader);
	rectangle->Height = BufferReader_ReadI32(reader);
}
