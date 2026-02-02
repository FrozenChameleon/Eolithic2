/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../io/DynamicByteBuffer.h"
#include "RenderCommands.h"

typedef struct RenderStream
{
	DynamicByteBuffer* _mBuffer;
} RenderStream;

void RenderStream_Clear(RenderStream* rs);
RenderCommandTileLayer* RenderStream_GetRenderCommandLayerUninitialized(RenderStream* rs);
RenderCommandManyRectangle* RenderStream_GetRenderCommandManyRectangleUninitialized(RenderStream* rs);
RenderCommandString* RenderStream_GetRenderCommandStringUninitialized(RenderStream* rs);
RenderCommandSheet* RenderStream_GetRenderCommandSheetUninitialized(RenderStream* rs);
DynamicByteBuffer* RenderStream_GetBuffer(RenderStream* rs);
void RenderStream_PreallocateBuffer(RenderStream* rs, int32_t initialCapacity);