/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"

typedef struct FixedByteBuffer FixedByteBuffer;

uint8_t* FixedByteBuffer_GetBuffer(const FixedByteBuffer* fbb);
uint64_t FixedByteBuffer_GetLength(const FixedByteBuffer* fbb);
FixedByteBuffer* FixedByteBuffer_CreateAndTakeOwnership(uint64_t length, uint8_t* buffer);
FixedByteBuffer* FixedByteBuffer_Create(uint64_t length);
void FixedByteBuffer_Dispose(FixedByteBuffer* fbb);
uint64_t FixedByteBuffer_GetRefs(void);
FixedByteBuffer* FixedByteBuffer_Clone(FixedByteBuffer* fbb);