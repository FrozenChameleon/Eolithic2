#pragma once

#include "DynamicByteBuffer.h"

void BufferWriter_WriteU8(DynamicByteBuffer* dbb, uint8_t value);
void BufferWriter_WriteU16(DynamicByteBuffer* dbb, uint16_t value);
void BufferWriter_WriteU32(DynamicByteBuffer* dbb, uint32_t value);
void BufferWriter_WriteU64(DynamicByteBuffer* dbb, uint64_t value);
void BufferWriter_WriteI8(DynamicByteBuffer* dbb, int8_t value);
void BufferWriter_WriteI16(DynamicByteBuffer* dbb, int16_t value);
void BufferWriter_WriteI32(DynamicByteBuffer* dbb, int32_t value);
void BufferWriter_WriteI64(DynamicByteBuffer* dbb, int64_t value);
void BufferWriter_WriteFloat(DynamicByteBuffer* dbb, float value);
void BufferWriter_WriteDouble(DynamicByteBuffer* dbb, double value);
void BufferWriter_WriteBoolean(DynamicByteBuffer* dbb, bool value);
void BufferWriter_WriteNewline(DynamicByteBuffer* dbb);
void BufferWriter_WriteString(DynamicByteBuffer* dbb, const char* str, size_t maxlen);