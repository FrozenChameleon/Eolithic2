/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "DynamicByteBuffer.h"

#include "../utils/Utils.h"
#include "../utils/Logger.h"
#include "../utils/Exception.h"
#include "../utils/Macros.h"
#include "../utils/MString.h"

#define INITIAL_CAPACITY 1024

static uint64_t _mRefs;
static MString* _mTempString;

typedef struct DynamicByteBuffer
{
	uint64_t mLength;
	uint64_t mCapacity;
	uint8_t* mBuffer;
	bool mIsWritingText;
	bool mNeedToWriteSpace;
} DynamicByteBuffer;

//BEGIN BINARY WRITER
static void BinaryWriter_CheckLength(DynamicByteBuffer* dbb, size_t size)
{
	if ((dbb->mLength + size) < dbb->mCapacity)
	{
		return;
	}

	uint64_t newCapacity = dbb->mCapacity * 2;
	dbb->mBuffer = (uint8_t*)Utils_grow(dbb->mBuffer, sizeof(uint8_t) * dbb->mCapacity, sizeof(uint8_t) * newCapacity);
	dbb->mCapacity = newCapacity;
}
static void* BinaryWriter_GetUninitializedMemoryBlock(DynamicByteBuffer* dbb, uint64_t memoryBlockLength)
{
	BinaryWriter_CheckLength(dbb, memoryBlockLength);
	void* memoryBlockStart = dbb->mBuffer + dbb->mLength;
	dbb->mLength += memoryBlockLength;
	return memoryBlockStart;
}
static void BinaryWriter_WriteSomething(DynamicByteBuffer* dbb, void* value, size_t size)
{
	BinaryWriter_CheckLength(dbb, size);
	Utils_memcpy(dbb->mBuffer + dbb->mLength, value, size);
	dbb->mLength += size;
}

static void BinaryWriter_WriteU8(DynamicByteBuffer* dbb, uint8_t value)
{
	BinaryWriter_WriteSomething(dbb, &value, sizeof(uint8_t));
}
static void BinaryWriter_WriteU16(DynamicByteBuffer* dbb, uint16_t value)
{
	BinaryWriter_WriteSomething(dbb, &value, sizeof(uint16_t));
}
static void BinaryWriter_WriteU32(DynamicByteBuffer* dbb, uint32_t value)
{
	BinaryWriter_WriteSomething(dbb, &value, sizeof(uint32_t));
}
static void BinaryWriter_WriteU64(DynamicByteBuffer* dbb, uint64_t value)
{
	BinaryWriter_WriteSomething(dbb, &value, sizeof(uint64_t));
}
static void BinaryWriter_WriteI8(DynamicByteBuffer* dbb, int8_t value)
{
	BinaryWriter_WriteSomething(dbb, &value, sizeof(int8_t));
}
static void BinaryWriter_WriteI16(DynamicByteBuffer* dbb, int16_t value)
{
	BinaryWriter_WriteSomething(dbb, &value, sizeof(int16_t));
}
static void BinaryWriter_WriteI32(DynamicByteBuffer* dbb, int32_t value)
{
	BinaryWriter_WriteSomething(dbb, &value, sizeof(int32_t));
}
static void BinaryWriter_WriteI64(DynamicByteBuffer* dbb, int64_t value)
{
	BinaryWriter_WriteSomething(dbb, &value, sizeof(int64_t));
}
static void BinaryWriter_WriteFloat(DynamicByteBuffer* dbb, float value)
{
	BinaryWriter_WriteSomething(dbb, &value, sizeof(float));
}
static void BinaryWriter_WriteDouble(DynamicByteBuffer* dbb, double value)
{
	BinaryWriter_WriteSomething(dbb, &value, sizeof(double));
}
static void BinaryWriter_WriteBool(DynamicByteBuffer* dbb, bool value)
{
	uint8_t valueToWrite = Utils_BoolToUInt8(value);
	BinaryWriter_WriteSomething(dbb, &valueToWrite, sizeof(uint8_t));
}
static void BinaryWriter_WriteStringHelper(DynamicByteBuffer* dbb, const char* str, size_t maxlen, bool writeLength)
{
	size_t str_len = Utils_strnlen(str, maxlen);

	if ((maxlen > 0xFF) || (str_len > 0xFF))
	{
		str_len = 0xFF;
		Exception_Run("Cannot write string longer than 255 to DBB!!!", false);
	}

	if (!Utils_IsStringASCII(str, str_len))
	{
		str = "CANONLYWRITEASCII!";
		str_len = Utils_strlen(str);
		Exception_Run("Cannot write non-ASCII characters to DBB!!!", false);
	}

	if (writeLength)
	{
		BinaryWriter_WriteU8(dbb, (uint8_t)str_len);
	}

	for (int32_t i = 0; i < str_len; i += 1)
	{
		BinaryWriter_WriteU8(dbb, str[i]);
	}
}
static void BinaryWriter_WriteString(DynamicByteBuffer* dbb, const char* str, size_t maxlen)
{
	BinaryWriter_WriteStringHelper(dbb, str, maxlen, true);
}
static void BinaryWriter_WriteStringJustTheData(DynamicByteBuffer* dbb, const char* str, size_t maxlen)
{
	BinaryWriter_WriteStringHelper(dbb, str, maxlen, false);
}
//END BINARY WRITER

//BEGIN TEXT WRITER
static void TextWriter_WriteTempString(DynamicByteBuffer* dbb)
{
	BinaryWriter_WriteStringJustTheData(dbb, MString_Text(_mTempString), MString_Length(_mTempString));
}
static void TextWriter_WriteSpace(DynamicByteBuffer* dbb)
{
	MString_AssignString(&_mTempString, " ");
	TextWriter_WriteTempString(dbb);
}
static void TextWriter_WriteSpaceIfNeeded(DynamicByteBuffer* dbb)
{
	if (dbb->mNeedToWriteSpace)
	{
		TextWriter_WriteSpace(dbb);
		dbb->mNeedToWriteSpace = false;
	}
}

static void TextWriter_WriteU8(DynamicByteBuffer* dbb, uint8_t value)
{
	TextWriter_WriteSpaceIfNeeded(dbb);
	MString_AssignEmptyString(&_mTempString);
	MString_AddAssignUInt8(&_mTempString, value);
	TextWriter_WriteTempString(dbb);
	dbb->mNeedToWriteSpace = true;
}
static void TextWriter_WriteU16(DynamicByteBuffer* dbb, uint16_t value)
{
	TextWriter_WriteSpaceIfNeeded(dbb);
	MString_AssignEmptyString(&_mTempString);
	MString_AddAssignUInt16(&_mTempString, value);
	TextWriter_WriteTempString(dbb);
	dbb->mNeedToWriteSpace = true;
}
static void TextWriter_WriteU32(DynamicByteBuffer* dbb, uint32_t value)
{
	TextWriter_WriteSpaceIfNeeded(dbb);
	MString_AssignEmptyString(&_mTempString);
	MString_AddAssignUInt32(&_mTempString, value);
	TextWriter_WriteTempString(dbb);
	dbb->mNeedToWriteSpace = true;
}
static void TextWriter_WriteU64(DynamicByteBuffer* dbb, uint64_t value)
{
	TextWriter_WriteSpaceIfNeeded(dbb);
	MString_AssignEmptyString(&_mTempString);
	MString_AddAssignUInt64(&_mTempString, value);
	TextWriter_WriteTempString(dbb);
	dbb->mNeedToWriteSpace = true;
}
static void TextWriter_WriteI8(DynamicByteBuffer* dbb, int8_t value)
{
	TextWriter_WriteSpaceIfNeeded(dbb);
	MString_AssignEmptyString(&_mTempString);
	MString_AddAssignInt8(&_mTempString, value);
	TextWriter_WriteTempString(dbb);
	dbb->mNeedToWriteSpace = true;
}
static void TextWriter_WriteI16(DynamicByteBuffer* dbb, int16_t value)
{
	TextWriter_WriteSpaceIfNeeded(dbb);
	MString_AssignEmptyString(&_mTempString);
	MString_AddAssignInt16(&_mTempString, value);
	TextWriter_WriteTempString(dbb);
	dbb->mNeedToWriteSpace = true;
}
static void TextWriter_WriteI32(DynamicByteBuffer* dbb, int32_t value)
{
	TextWriter_WriteSpaceIfNeeded(dbb);
	MString_AssignEmptyString(&_mTempString);
	MString_AddAssignInt(&_mTempString, value);
	TextWriter_WriteTempString(dbb);
	dbb->mNeedToWriteSpace = true;
}
static void TextWriter_WriteI64(DynamicByteBuffer* dbb, int64_t value)
{
	TextWriter_WriteSpaceIfNeeded(dbb);
	MString_AssignEmptyString(&_mTempString);
	MString_AddAssignInt64(&_mTempString, value);
	TextWriter_WriteTempString(dbb);
	dbb->mNeedToWriteSpace = true;
}
static void TextWriter_WriteFloat(DynamicByteBuffer* dbb, float value)
{
	TextWriter_WriteSpaceIfNeeded(dbb);
	MString_AssignEmptyString(&_mTempString);
	MString_AddAssignFloat(&_mTempString, value);
	TextWriter_WriteTempString(dbb);
	dbb->mNeedToWriteSpace = true;
}
static void TextWriter_WriteDouble(DynamicByteBuffer* dbb, double value)
{
	TextWriter_WriteSpaceIfNeeded(dbb);
	MString_AssignEmptyString(&_mTempString);
	MString_AddAssignDouble(&_mTempString, value);
	TextWriter_WriteTempString(dbb);
	dbb->mNeedToWriteSpace = true;
}
static void TextWriter_WriteBool(DynamicByteBuffer* dbb, bool value)
{
	TextWriter_WriteSpaceIfNeeded(dbb);
	MString_AssignEmptyString(&_mTempString);
	MString_AddAssignUInt8(&_mTempString, Utils_BoolToUInt8(value));
	TextWriter_WriteTempString(dbb);
	dbb->mNeedToWriteSpace = true;
}
static void TextWriter_WriteNewline(DynamicByteBuffer* dbb)
{
	MString_AssignString(&_mTempString, "\r");
	TextWriter_WriteTempString(dbb);
	MString_AssignString(&_mTempString, " \n");
	TextWriter_WriteTempString(dbb);
	dbb->mNeedToWriteSpace = false;
}
static void TextWriter_WriteString(DynamicByteBuffer* dbb, const char* str, size_t maxlen)
{
	TextWriter_WriteSpaceIfNeeded(dbb);
	MString_AssignString(&_mTempString, "\"");
	TextWriter_WriteTempString(dbb);
	MString_AssignString(&_mTempString, str);
	TextWriter_WriteTempString(dbb);
	MString_AssignString(&_mTempString, "\"");
	TextWriter_WriteTempString(dbb);
	dbb->mNeedToWriteSpace = true;
}
//END TEXT WRITER

void* DynamicByteBuffer_GetUninitializedMemoryBlock(DynamicByteBuffer* dbb, uint64_t memoryBlockLength)
{
	return BinaryWriter_GetUninitializedMemoryBlock(dbb, memoryBlockLength);
}
void DynamicByteBuffer_WriteU8(DynamicByteBuffer* dbb, uint8_t value)
{
	if (dbb->mIsWritingText)
	{
		TextWriter_WriteU8(dbb, value);
	}
	else
	{
		BinaryWriter_WriteU8(dbb, value);
	}
}
void DynamicByteBuffer_WriteU16(DynamicByteBuffer* dbb, uint16_t value)
{
	if (dbb->mIsWritingText)
	{
		TextWriter_WriteU16(dbb, value);
	}
	else
	{
		BinaryWriter_WriteU16(dbb, value);
	}
}
void DynamicByteBuffer_WriteU32(DynamicByteBuffer* dbb, uint32_t value)
{
	if (dbb->mIsWritingText)
	{
		TextWriter_WriteU32(dbb, value);
	}
	else
	{
		BinaryWriter_WriteU32(dbb, value);
	}
}
void DynamicByteBuffer_WriteU64(DynamicByteBuffer* dbb, uint64_t value)
{
	if (dbb->mIsWritingText)
	{
		TextWriter_WriteU64(dbb, value);
	}
	else
	{
		BinaryWriter_WriteU64(dbb, value);
	}
}
void DynamicByteBuffer_WriteI8(DynamicByteBuffer* dbb, int8_t value)
{
	if (dbb->mIsWritingText)
	{
		TextWriter_WriteI8(dbb, value);
	}
	else
	{
		BinaryWriter_WriteI8(dbb, value);
	}
}
void DynamicByteBuffer_WriteI16(DynamicByteBuffer* dbb, int16_t value)
{
	if (dbb->mIsWritingText)
	{
		TextWriter_WriteI16(dbb, value);
	}
	else
	{
		BinaryWriter_WriteI16(dbb, value);
	}
}
void DynamicByteBuffer_WriteI32(DynamicByteBuffer* dbb, int32_t value)
{
	if (dbb->mIsWritingText)
	{
		TextWriter_WriteI32(dbb, value);
	}
	else
	{
		BinaryWriter_WriteI32(dbb, value);
	}
}
void DynamicByteBuffer_WriteI64(DynamicByteBuffer* dbb, int64_t value)
{
	if (dbb->mIsWritingText)
	{
		TextWriter_WriteI64(dbb, value);
	}
	else
	{
		BinaryWriter_WriteI64(dbb, value);
	}
}
void DynamicByteBuffer_WriteFloat(DynamicByteBuffer* dbb, float value)
{
	if (dbb->mIsWritingText)
	{
		TextWriter_WriteFloat(dbb, value);
	}
	else
	{
		BinaryWriter_WriteFloat(dbb, value);
	}
}
void DynamicByteBuffer_WriteDouble(DynamicByteBuffer* dbb, double value)
{
	if (dbb->mIsWritingText)
	{
		TextWriter_WriteDouble(dbb, value);
	}
	else
	{
		BinaryWriter_WriteDouble(dbb, value);
	}
}
void DynamicByteBuffer_WriteBoolean(DynamicByteBuffer* dbb, bool value)
{
	if (dbb->mIsWritingText)
	{
		TextWriter_WriteBool(dbb, value);
	}
	else
	{
		BinaryWriter_WriteBool(dbb, value);
	}
}
void DynamicByteBuffer_WriteString(DynamicByteBuffer* dbb, const char* str, size_t maxlen)
{
	if (str == NULL)
	{
		Logger_LogWarning("Handed null pointer to write string!");
		return;
	}

	if (dbb->mIsWritingText)
	{
		TextWriter_WriteString(dbb, str, maxlen);
	}
	else
	{
		BinaryWriter_WriteString(dbb, str, maxlen);
	}
}
void DynamicByteBuffer_WriteEOF(DynamicByteBuffer* dbb)
{

}
void DynamicByteBuffer_WriteNewline(DynamicByteBuffer* dbb)
{
	if (dbb->mIsWritingText)
	{
		TextWriter_WriteNewline(dbb);
	}
}
uint8_t* DynamicByteBuffer_GetBuffer(const DynamicByteBuffer* dbb)
{
	return dbb->mBuffer;
}
uint64_t DynamicByteBuffer_GetLength(const DynamicByteBuffer* dbb)
{
	return dbb->mLength;
}
void DynamicByteBuffer_SetLength(DynamicByteBuffer* dbb, uint64_t value)
{
	dbb->mLength = value;
}
FixedByteBuffer* DynamicByteBuffer_ConvertToFixedByteBufferAndDisposeDBB(DynamicByteBuffer* dbb)
{
	FixedByteBuffer* fbb = FixedByteBuffer_Create(dbb->mLength);
	uint8_t* toBuffer = FixedByteBuffer_GetBuffer(fbb);
	Utils_memcpy(toBuffer, dbb->mBuffer, dbb->mLength);
	DynamicByteBuffer_Dispose(dbb);
	return fbb;
}
DynamicByteBuffer* DynamicByteBuffer_Create(void)
{
	_mRefs += 1;
	DynamicByteBuffer* dbb = (DynamicByteBuffer*)Utils_calloc(1, sizeof(DynamicByteBuffer));
	dbb->mLength = 0;
	dbb->mCapacity = INITIAL_CAPACITY;
	dbb->mBuffer = (uint8_t*)Utils_calloc(INITIAL_CAPACITY, sizeof(uint8_t));
	return dbb;
}
void DynamicByteBuffer_Dispose(DynamicByteBuffer* dbb)
{
	_mRefs -= 1;
	Utils_free(dbb->mBuffer);
	Utils_free(dbb);
}
uint64_t DynamicByteBuffer_GetRefs(void)
{
	return _mRefs;
}
void DynamicByteBuffer_SetIsWritingText(DynamicByteBuffer* dbb, bool isWritingText)
{
	dbb->mIsWritingText = isWritingText;
}
