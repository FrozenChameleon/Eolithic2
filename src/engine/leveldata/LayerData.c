/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "LayerData.h"

#include "../io/BufferReader.h"
#include "../io/DynamicByteBuffer.h"

void LayerData_Read(LayerData* layer, BufferReader* br)
{
	layer->mSpeedX = BufferReader_ReadFloat(br);
	layer->mSpeedY = BufferReader_ReadFloat(br);
	layer->mDepth = BufferReader_ReadI32(br);
}
void LayerData_Write(LayerData* layer, DynamicByteBuffer* dbb)
{
	DynamicByteBuffer_WriteFloat(dbb, layer->mSpeedX);
	DynamicByteBuffer_WriteFloat(dbb, layer->mSpeedY);
	DynamicByteBuffer_WriteI32(dbb, layer->mDepth);
}
