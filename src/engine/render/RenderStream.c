/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "RenderStream.h"

void RenderStream_Clear(RenderStream* rs)
{
	DynamicByteBuffer_SetLength(rs->_mBuffer, 0);
}
RenderCommandTileLayer* RenderStream_GetRenderCommandLayerUninitialized(RenderStream* rs)
{
	return (RenderCommandTileLayer*)DynamicByteBuffer_GetUninitializedMemoryBlock(rs->_mBuffer, sizeof(RenderCommandTileLayer));
}
RenderCommandManyRectangle* RenderStream_GetRenderCommandManyRectangleUninitialized(RenderStream* rs)
{
	return (RenderCommandManyRectangle*)DynamicByteBuffer_GetUninitializedMemoryBlock(rs->_mBuffer, sizeof(RenderCommandManyRectangle));
}
RenderCommandString* RenderStream_GetRenderCommandStringUninitialized(RenderStream* rs)
{
	return (RenderCommandString*)DynamicByteBuffer_GetUninitializedMemoryBlock(rs->_mBuffer, sizeof(RenderCommandString));
}
RenderCommandSheet* RenderStream_GetRenderCommandSheetUninitialized(RenderStream* rs)
{
	return (RenderCommandSheet*)DynamicByteBuffer_GetUninitializedMemoryBlock(rs->_mBuffer, sizeof(RenderCommandSheet));
}
DynamicByteBuffer* RenderStream_GetBuffer(RenderStream* rs)
{
	return rs->_mBuffer;
}
void RenderStream_PreallocateBuffer(RenderStream* rs, int32_t initialCapacity)
{
	if (initialCapacity <= 4) //No change.
	{
		return;
	}

	DynamicByteBuffer_GetUninitializedMemoryBlock(rs->_mBuffer, initialCapacity - 1);
	RenderStream_Clear(rs);
}