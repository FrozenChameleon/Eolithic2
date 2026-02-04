/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"
#include "FixedByteBuffer.h"

typedef struct DynamicByteBuffer DynamicByteBuffer;

void* DynamicByteBuffer_GetUninitializedMemoryBlock(DynamicByteBuffer* dbb, uint64_t memoryBlockLength);
void DynamicByteBuffer_WriteU8(DynamicByteBuffer* dbb, uint8_t value);
void DynamicByteBuffer_WriteU16(DynamicByteBuffer* dbb, uint16_t value);
void DynamicByteBuffer_WriteU32(DynamicByteBuffer* dbb, uint32_t value);
void DynamicByteBuffer_WriteU64(DynamicByteBuffer* dbb, uint64_t value);
void DynamicByteBuffer_WriteI8(DynamicByteBuffer* dbb, int8_t value);
void DynamicByteBuffer_WriteI16(DynamicByteBuffer* dbb, int16_t value);
void DynamicByteBuffer_WriteI32(DynamicByteBuffer* dbb, int32_t value);
void DynamicByteBuffer_WriteI64(DynamicByteBuffer* dbb, int64_t value);
void DynamicByteBuffer_WriteFloat(DynamicByteBuffer* dbb, float value);
void DynamicByteBuffer_WriteDouble(DynamicByteBuffer* dbb, double value);
void DynamicByteBuffer_WriteBoolean(DynamicByteBuffer* dbb, bool value);
void DynamicByteBuffer_WriteMagicNumber(DynamicByteBuffer* dbb);
void DynamicByteBuffer_WriteVersionNumber(DynamicByteBuffer* dbb, int16_t versionNumber);
void DynamicByteBuffer_WriteString(DynamicByteBuffer* dbb, const char* str, size_t maxlen);
void DynamicByteBuffer_WriteEOF(DynamicByteBuffer* dbb);
void DynamicByteBuffer_WriteNewline(DynamicByteBuffer* dbb);
void DynamicByteBuffer_WriteIntArray2D(DynamicByteBuffer* dbb, int32_t* array2D, int32_t width, int32_t height);
uint8_t* DynamicByteBuffer_GetBuffer(const DynamicByteBuffer* dbb);
uint64_t DynamicByteBuffer_GetLength(const DynamicByteBuffer* dbb);
void DynamicByteBuffer_SetLength(DynamicByteBuffer* dbb, uint64_t value);
FixedByteBuffer* DynamicByteBuffer_ConvertToFixedByteBufferAndDisposeDBB(DynamicByteBuffer* dbb);
DynamicByteBuffer* DynamicByteBuffer_Create(void);
void DynamicByteBuffer_Dispose(DynamicByteBuffer* dbb);
uint64_t DynamicByteBuffer_GetRefs(void);
void DynamicByteBuffer_SetIsWritingText(DynamicByteBuffer* dbb, bool isWritingText);