/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "StringPair.h"

#include "../utils/Macros.h"
#include "../io/BufferReader.h"
#include "../io/DynamicByteBuffer.h"

const StringPair StringPair_Empty = { EE_STR_EMPTY, EE_STR_EMPTY };

void StringPair_Read(StringPair* pair, BufferReader* br)
{
	BufferReader_ReadString(br, pair->mKey, EE_FILENAME_MAX);
	BufferReader_ReadString(br, pair->mValue, EE_FILENAME_MAX);
}
void StringPair_Write(StringPair* pair, DynamicByteBuffer* dbb)
{
	DynamicByteBuffer_WriteString(dbb, pair->mKey, EE_FILENAME_MAX);
	DynamicByteBuffer_WriteString(dbb, pair->mValue, EE_FILENAME_MAX);
}
