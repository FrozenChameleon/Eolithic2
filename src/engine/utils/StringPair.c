/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "StringPair.h"

#include "../utils/Macros.h"
#include "../io/BufferReader.h"

const StringPair StringPair_Empty = { EE_STR_EMPTY, EE_STR_EMPTY };

void StringPair_Read(StringPair* pair, BufferReader* reader)
{
	BufferReader_ReadString(reader, pair->mKey, EE_FILENAME_MAX);
	BufferReader_ReadString(reader, pair->mValue, EE_FILENAME_MAX);
}
