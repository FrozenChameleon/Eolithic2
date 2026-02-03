/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "BufferReader.h"

#include "SDL3/SDL.h"
#include "../utils/Utils.h"
#include "File.h"
#include "../utils/MString.h"
#include "../utils/Logger.h"

static uint64_t _mRefs;

typedef struct BufferReader
{
	FixedByteBuffer* mBuffer;
	SDL_IOStream* mIOStream;
	bool mIsReadingText;
} BufferReader;

static MString* _mNextString;

static uint8_t BinaryReader_ReadU8(BufferReader* br)
{
	uint8_t value;
	SDL_ReadU8(br->mIOStream, &value);
	return value;
}
static uint16_t BinaryReader_ReadU16(BufferReader* br)
{
	uint16_t value;
	SDL_ReadU16LE(br->mIOStream, &value);
	return value;
}
static int16_t BinaryReader_ReadI16(BufferReader* br)
{
	uint16_t source = BinaryReader_ReadU16(br);
	int16_t dst;
	Utils_memcpy(&dst, &source, sizeof(int16_t));
	return dst;
}
static uint32_t BinaryReader_ReadU32(BufferReader* br)
{
	uint32_t value;
	SDL_ReadU32LE(br->mIOStream, &value);
	return value;
}
static int32_t BinaryReader_ReadI32(BufferReader* br)
{
	uint32_t source = BinaryReader_ReadU32(br);
	int32_t dst;
	Utils_memcpy(&dst, &source, sizeof(int32_t));
	return dst;
}
static uint64_t BinaryReader_ReadU64(BufferReader* br)
{
	uint64_t value;
	SDL_ReadU64LE(br->mIOStream, &value);
	return value;
}
static int64_t BinaryReader_ReadI64(BufferReader* br)
{
	uint64_t source = BinaryReader_ReadU64(br);
	int64_t dst;
	Utils_memcpy(&dst, &source, sizeof(int64_t));
	return dst;
}
static float BinaryReader_ReadFloat(BufferReader* br)
{
	uint32_t source = BinaryReader_ReadU32(br);
	float dst;
	Utils_memcpy(&dst, &source, sizeof(float));
	return dst;
}
static double BinaryReader_ReadDouble(BufferReader* br)
{
	uint64_t source = BinaryReader_ReadU64(br);
	double dst;
	Utils_memcpy(&dst, &source, sizeof(double));
	return dst;
}
static uint8_t BinaryReader_ReadJustTheStringLength(BufferReader* br)
{
	return BinaryReader_ReadU8(br);
}
static void BinaryReader_ReadJustTheStringData(BufferReader* br, uint8_t stringLength, char* dst, size_t maxlen)
{
	Utils_memset(dst, 0, maxlen);
	for (int32_t i = 0; i < stringLength; i += 1)
	{
		uint8_t c = BinaryReader_ReadU8(br);
		if (i < maxlen)
		{
			dst[i] = c;
		}
	}
	if (stringLength < maxlen)
	{
		dst[stringLength] = '\0';
	}
}
static void BinaryReader_ReadString(BufferReader* br, char* dst, size_t maxlen)
{
	uint8_t stringLength = BinaryReader_ReadJustTheStringLength(br);
	BinaryReader_ReadJustTheStringData(br, stringLength, dst, maxlen);
}

static void TextReader_ReadToNextString(BufferReader* br)
{
	MString_AssignEmptyString(&_mNextString);

	bool wasEscaped = false;
	bool isEscaped = false;
	bool hasReachedText = false;
	while (BufferReader_HasNext(br)) //TODO THIS ESCAPE SETUP IS NOT GOOD ENOUGH
	{
		uint8_t nextByte = BinaryReader_ReadU8(br);

		if ((nextByte == ' ') || (nextByte == '\n') || (nextByte == '\r'))
		{
			if (!isEscaped && hasReachedText)
			{
				break;
			}
		}
		else
		{
			if (nextByte == '"')
			{
				wasEscaped = true;
				isEscaped = !isEscaped;
			}
			hasReachedText = true;
			MString_AddAssignChar(&_mNextString, nextByte);
		}
	}

	if (wasEscaped && (MString_GetLastChar(_mNextString) == '"')) //Trim escape
	{
		MString_AssignSubString(&_mNextString, MString_Text(_mNextString), 1, MString_Length(_mNextString) - 2);
	}
}
static bool ReadNextStringAndCheckIfEmpty(BufferReader* br)
{
	TextReader_ReadToNextString(br);
	if (MString_IsEmpty(_mNextString))
	{
		return true;
	}
	return false;
}
static uint8_t TextReader_ReadU8(BufferReader* br)
{
	if (ReadNextStringAndCheckIfEmpty(br))
	{
		return 0;
	}

	return (uint8_t)Utils_ParseInt(MString_Text(_mNextString));
}
static uint16_t TextReader_ReadU16(BufferReader* br)
{
	if (ReadNextStringAndCheckIfEmpty(br))
	{
		return 0;
	}

	return (uint16_t)Utils_ParseInt(MString_Text(_mNextString));
}
static uint32_t TextReader_ReadU32(BufferReader* br)
{
	if (ReadNextStringAndCheckIfEmpty(br))
	{
		return 0;
	}

	return Utils_ParseUInt32(MString_Text(_mNextString));
}
static uint64_t TextReader_ReadU64(BufferReader* br)
{
	if (ReadNextStringAndCheckIfEmpty(br))
	{
		return 0;
	}

	return Utils_ParseUInt64(MString_Text(_mNextString));
}
static int8_t TextReader_ReadI8(BufferReader* br)
{
	if (ReadNextStringAndCheckIfEmpty(br))
	{
		return 0;
	}

	return (int8_t)Utils_ParseInt(MString_Text(_mNextString));
}
static int16_t TextReader_ReadI16(BufferReader* br)
{
	if (ReadNextStringAndCheckIfEmpty(br))
	{
		return 0;
	}

	return (int16_t)Utils_ParseInt(MString_Text(_mNextString));
}
static int32_t TextReader_ReadI32(BufferReader* br)
{
	if (ReadNextStringAndCheckIfEmpty(br))
	{
		return 0;
	}

	return Utils_ParseInt(MString_Text(_mNextString));
}
static int64_t TextReader_ReadI64(BufferReader* br)
{
	if (ReadNextStringAndCheckIfEmpty(br))
	{
		return 0;
	}

	return (int64_t)Utils_ParseInt64(MString_Text(_mNextString));
}
static float TextReader_ReadFloat(BufferReader* br)
{
	if (ReadNextStringAndCheckIfEmpty(br))
	{
		return 0;
	}

	return Utils_ParseFloat(MString_Text(_mNextString));
}
static double TextReader_ReadDouble(BufferReader* br)
{
	if (ReadNextStringAndCheckIfEmpty(br))
	{
		return 0;
	}

	return Utils_ParseDouble(MString_Text(_mNextString));
}
static int32_t TextReader_ReadNextStringAndReturnIfValid(BufferReader* br)
{
	if (ReadNextStringAndCheckIfEmpty(br))
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

FixedByteBuffer* BufferReader_ReadBytes(BufferReader* br, int64_t length)
{
	FixedByteBuffer* fbb = FixedByteBuffer_Create(length);
	SDL_ReadIO(br->mIOStream, FixedByteBuffer_GetBuffer(fbb), length);
	return fbb;
}
int64_t BufferReader_Position(BufferReader* br)
{
	return BufferReader_Tell(br);
}
void BufferReader_Seek(BufferReader* br, int64_t offset, SeekFrom type)
{
	SDL_IOWhence whence = SDL_IO_SEEK_SET;
	switch (type)
	{
	case BUFFER_READER_SEEK_FROM_BEGIN:
		whence = SDL_IO_SEEK_SET;
		break;
	case 	BUFFER_READER_SEEK_FROM_CURRENT:
		whence = SDL_IO_SEEK_CUR;
		break;
	case BUFFER_READER_SEEK_FROM_END:
		whence = SDL_IO_SEEK_END;
		break;
	}

	SDL_SeekIO(br->mIOStream, offset, whence);
}
FixedByteBuffer* BufferReader_GetBuffer(BufferReader* br)
{
	return br->mBuffer;
}
uint64_t BufferReader_GetSize(BufferReader* br)
{
	return FixedByteBuffer_GetLength(br->mBuffer);
}
uint8_t* BufferReader_GetBufferData(BufferReader* br)
{
	return FixedByteBuffer_GetBuffer(br->mBuffer);
}
int64_t BufferReader_Tell(BufferReader* br)
{
	return SDL_TellIO(br->mIOStream);
}
bool BufferReader_ReadBoolean(BufferReader* br)
{
	if (br->mIsReadingText)
	{
		return (TextReader_ReadU8(br) != 0);
	}
	else
	{
		return (BinaryReader_ReadU8(br) != 0);
	}
}
uint8_t BufferReader_ReadU8(BufferReader* br)
{
	if (br->mIsReadingText)
	{
		return TextReader_ReadU8(br);
	}
	else
	{
		return BinaryReader_ReadU8(br);
	}
}
uint16_t BufferReader_ReadU16(BufferReader* br)
{
	if (br->mIsReadingText)
	{
		return TextReader_ReadU16(br);
	}
	else
	{
		return BinaryReader_ReadU16(br);
	}
}
uint32_t BufferReader_ReadU32(BufferReader* br)
{
	if (br->mIsReadingText)
	{
		return TextReader_ReadU32(br);
	}
	else
	{
		return BinaryReader_ReadU32(br);
	}
}
uint64_t BufferReader_ReadU64(BufferReader* br)
{
	if (br->mIsReadingText)
	{
		return TextReader_ReadU64(br);
	}
	else
	{
		return BinaryReader_ReadU64(br);
	}
}
int16_t BufferReader_ReadI16(BufferReader* br)
{
	if (br->mIsReadingText)
	{
		return TextReader_ReadI16(br);
	}
	else
	{
		return BinaryReader_ReadI16(br);
	}
}
int32_t BufferReader_ReadI32(BufferReader* br)
{
	if (br->mIsReadingText)
	{
		return TextReader_ReadI32(br);
	}
	else
	{
		return BinaryReader_ReadI32(br);
	}
}
int64_t BufferReader_ReadI64(BufferReader* br)
{
	if (br->mIsReadingText)
	{
		return TextReader_ReadI64(br);
	}
	else
	{
		return BinaryReader_ReadI64(br);
	}
}
float BufferReader_ReadFloat(BufferReader* br)
{
	if (br->mIsReadingText)
	{
		return TextReader_ReadFloat(br);
	}
	else
	{
		return BinaryReader_ReadFloat(br);
	}
}
double BufferReader_ReadDouble(BufferReader* br)
{
	if (br->mIsReadingText)
	{
		return TextReader_ReadDouble(br);
	}
	else
	{
		return BinaryReader_ReadDouble(br);
	}
}
bool BufferReader_ReadMagicNumber(BufferReader* br)
{
	int16_t magicNumber = BufferReader_ReadI16(br);
	if (magicNumber != EE_MAGIC_NUMBER)
	{
		Logger_LogError("Magic number not found!");
		return false;
	}
	return true;
}
int16_t BufferReader_ReadVersionNumber(BufferReader* br)
{
	return BufferReader_ReadI16(br);
}
int32_t* BufferReader_ReadIntArray2D(BufferReader* br, int32_t width, int32_t height) //TODO MEMORY LEAK
{
	int32_t totalArrayLen = (width * height);
	int32_t* readToThis = (int32_t*)Utils_calloc(totalArrayLen, sizeof(int32_t));
	for (int32_t i = 0; i < width; i += 1)
	{
		for (int32_t j = 0; j < height; j += 1)
		{
			readToThis[i + (j * width)] = BufferReader_ReadI32(br);
		}
	}
	return readToThis;
}
uint8_t BufferReader_ReadJustTheStringLength(BufferReader* br)
{
	if (br->mIsReadingText)
	{
		return 0;
	}
	else
	{
		return BinaryReader_ReadJustTheStringLength(br);
	}
}
void BufferReader_ReadJustTheStringData(BufferReader* br, uint8_t stringLength, char* dst, size_t maxlen)
{
	if (br->mIsReadingText)
	{
	}
	else
	{
		BinaryReader_ReadJustTheStringData(br, stringLength, dst, maxlen);
	}
}
void BufferReader_ReadString(BufferReader* br, char* dst, size_t maxlen)
{
	if (br->mIsReadingText)
	{
		if (TextReader_ReadNextStringAndReturnIfValid(br) >= 0)
		{
			Utils_strlcpy(dst, MString_Text(_mNextString), maxlen);
		}
	}
	else
	{
		BinaryReader_ReadString(br, dst, maxlen);
	}
}
void BufferReader_ReadMString(MString** assignToThis, BufferReader* br)
{
	if (br->mIsReadingText)
	{
		if (TextReader_ReadNextStringAndReturnIfValid(br) >= 0)
		{
			MString_AssignMString(assignToThis, _mNextString);
		}
	}
	else
	{
		MString_Read(assignToThis, br);
	}
}
bool BufferReader_HasNext(BufferReader* br)
{
	int64_t cur = BufferReader_Tell(br);
	int64_t end = (int64_t)BufferReader_GetSize(br);
	if (cur >= end)
	{
		return false;
	}
	return true;
}
BufferReader* BufferReader_Create(FixedByteBuffer* fbb)
{
	_mRefs += 1;
	BufferReader* br = (BufferReader*)Utils_calloc(1, sizeof(BufferReader));
	br->mBuffer = fbb;
	br->mIOStream = SDL_IOFromConstMem(FixedByteBuffer_GetBuffer(fbb), FixedByteBuffer_GetLength(fbb));
	return br;
}
BufferReader* BufferReader_CreateFromPath(const char* path)
{
	return BufferReader_Create(File_ReadAll(path));
}
void BufferReader_Dispose(BufferReader* br)
{
	BufferReader_Dispose2(br, false);
}
void BufferReader_Dispose2(BufferReader* br, bool doNotDisposeBuffer)
{
	_mRefs -= 1;

	if (!doNotDisposeBuffer)
	{
		FixedByteBuffer_Dispose(br->mBuffer);
	}
	br->mBuffer = NULL;

	SDL_CloseIO(br->mIOStream);
	br->mIOStream = NULL;

	Utils_free(br);
}
uint64_t BufferReader_GetRefs(void)
{
	return _mRefs;
}
void BufferReader_SetIsReadingText(BufferReader* br, bool isReadingText)
{
	br->mIsReadingText = isReadingText;
}
