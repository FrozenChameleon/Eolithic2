/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "LayerData.h"

#include "../io/BufferReader.h"

void LayerData_ReadIni(LayerData* layer, int32_t i, BufferReader* reader)
{
	layer->mSpeedX = BufferReader_ReadFloat(reader);
	layer->mSpeedY = BufferReader_ReadFloat(reader);
	layer->mDepth = BufferReader_ReadI32(reader);
}