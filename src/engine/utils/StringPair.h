/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"

typedef struct BufferReader BufferReader;
typedef struct DynamicByteBuffer DynamicByteBuffer;

typedef struct StringPair
{
	char mKey[EE_FILENAME_MAX];
	char mValue[EE_FILENAME_MAX];
} StringPair;

extern const StringPair StringPair_Empty;

void StringPair_Read(StringPair* pair, BufferReader* br);
void StringPair_Write(StringPair* pair, DynamicByteBuffer* dbb);