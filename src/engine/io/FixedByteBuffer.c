/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "FixedByteBuffer.h"

#include "../utils/Utils.h"

#define INITIAL_CAPACITY 1024

static uint64_t _mRefs;

typedef struct FixedByteBuffer
{
	uint64_t mLength;
	uint8_t* mBuffer;
} FixedByteBuffer;

uint8_t* FixedByteBuffer_GetBuffer(const FixedByteBuffer* fbb)
{
	return fbb->mBuffer;
}
uint64_t FixedByteBuffer_GetLength(const FixedByteBuffer* fbb)
{
	return fbb->mLength;
}
FixedByteBuffer* FixedByteBuffer_CreateAndTakeOwnership(uint64_t length, uint8_t* buffer)
{
	_mRefs += 1;
	FixedByteBuffer* fbb = (FixedByteBuffer*)Utils_calloc(1, sizeof(FixedByteBuffer));
	fbb->mLength = length;
	fbb->mBuffer = buffer;
	return fbb;
}
FixedByteBuffer* FixedByteBuffer_Create(uint64_t length)
{
	return FixedByteBuffer_CreateAndTakeOwnership(length, (uint8_t*)Utils_calloc(length, sizeof(uint8_t)));
}
void FixedByteBuffer_Dispose(FixedByteBuffer* fbb)
{
	_mRefs -= 1;
	Utils_free(fbb->mBuffer);
	Utils_free(fbb);
}
uint64_t FixedByteBuffer_GetRefs(void)
{
	return _mRefs;
}
FixedByteBuffer* FixedByteBuffer_Clone(FixedByteBuffer* cloneThis)
{
	_mRefs += 1;
	FixedByteBuffer* fbbToReturn = (FixedByteBuffer*)Utils_calloc(1, sizeof(FixedByteBuffer));
	fbbToReturn->mLength = cloneThis->mLength;
	fbbToReturn->mBuffer = (uint8_t*)Utils_calloc(cloneThis->mLength, sizeof(uint8_t));
	Utils_memcpy(fbbToReturn->mBuffer, cloneThis->mBuffer, cloneThis->mLength);
	return fbbToReturn;
}
