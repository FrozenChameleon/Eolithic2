/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "DatReader.h"

#include "../utils/Utils.h"
#include "../../third_party/stb_ds.h"
#include "File.h"
#include "../utils/Exception.h"
#include "limits.h"

#define TEMP_REPLACE_INI_WITH_TXT

static uint64_t _mRefs;

typedef struct DatReader
{
	int32_t _mCurrent;
	int32_t _mCount;
	bool _mHasInit;
	MString* _mPath;
	DatInfo* arr_dat_info;
	//MString** arr_last_strings;
	BufferReader* _mReader;
} DatReader;

static DatInfo* GetCurrentDatInfo(DatReader* dr)
{
	DatInfo* info = &dr->arr_dat_info[dr->_mCurrent];
	return info;
}

FixedByteBuffer* DatReader_GetMemoryStream(DatReader* dr, int64_t length)
{
	return NULL;
}

bool DatReader_HasInit(const DatReader* dr)
{
	return dr->_mHasInit;
}
bool DatReader_HasNext(const DatReader* dr)
{
	return (dr->_mCurrent < dr->_mCount);
}
void DatReader_NextFilePath(MString** assignToThis, DatReader* dr)
{
	/*
	for (int32_t i = 0; i < arrlen(dr->arr_last_strings); i += 1)
	{
		MString_Dispose(&dr->arr_last_strings[i]);
	}
	arrsetlen(dr->arr_last_strings, 0);*/

	int32_t length = BufferReader_ReadI32(dr->_mReader);
	for (int32_t i = 0; i < length; i += 1)
	{
		MString* tempString = NULL;
#ifdef TEMP_REPLACE_INI_WITH_TXT
		MString* replacedTempString = NULL;
		BufferReader_ReadMString(&tempString, dr->_mReader);
		Utils_StringReplaceStr(&replacedTempString, MString_Text(tempString), ".ini", ".txt");
		MString_AddAssignString(assignToThis, MString_Text(replacedTempString));
#else
		BufferReader_ReadMString(&tempString, dr->_mReader);
		MString_AddAssignString(assignToThis, MString_Text(tempString));
#endif
		if (i < (length - 1))
		{
			File_AppendPathSeparator(assignToThis);
		}
		MString_Dispose(&tempString);
	}

	/*
	MString_AssignString(assignToThis, "");
	int64_t len = arrlen(dr->arr_last_strings);
	for (int32_t i = 0; i < len; i += 1)
	{
		MString_AddAssignString(assignToThis, MString_Text(dr->arr_last_strings[i]));
		if (i < (len - 1))
		{
			File_AppendPathSeparator(assignToThis);
		}
	}*/

	DatInfo* currentInfo = GetCurrentDatInfo(dr);
	MString_AssignString(&currentInfo->mPath, MString_Text(*assignToThis));
}
BufferReader* DatReader_NextStream(DatReader* dr, bool doNotReturnStream)
{
	int64_t length = BufferReader_ReadI64(dr->_mReader);
	int64_t currentActualPosition = BufferReader_Position(dr->_mReader);
	DatInfo* currentInfo = GetCurrentDatInfo(dr);
	currentInfo->mLength = length;
	currentInfo->mPosition = currentActualPosition;
	if (doNotReturnStream)
	{
		BufferReader_Seek(dr->_mReader, length, BUFFER_READER_SEEK_FROM_CURRENT);
		dr->_mCurrent += 1;
		return NULL;
	}
	else
	{
		if ((length < 0) || (length > INT_MAX))
		{
			Exception_Run("Invalid file size in dat!", false);
		}
		FixedByteBuffer* buffer = BufferReader_ReadBytes(dr->_mReader, length);
		BufferReader* bufferToReturn = BufferReader_Create(buffer);
		/*if (OeGlobals.IsDebugFileMode()) //UNUSED
		{
			if (OeCvars.GetAsBool(OeCvars.ENGINE_DECOMPRESS_DATS))
			{
				Decompress(length, stream);
				stream.Position = 0;
			}
		}*/
		dr->_mCurrent += 1;
		return bufferToReturn;
	}
}
DatReader* DatReader_Create(const char* path)
{
	_mRefs += 1;

	DatReader* dr = (DatReader*)Utils_calloc(1, sizeof(DatReader));

	dr->_mReader = BufferReader_CreateFromPath(path);
	MString_AssignString(&dr->_mPath, path);
	
	BufferReader_ReadI32(dr->_mReader); //VERSION
	dr->_mCount = BufferReader_ReadI32(dr->_mReader);
	for (int32_t i = 0; i < dr->_mCount; i += 1)
	{
		DatInfo temp = { 0 };
		arrput(dr->arr_dat_info, temp);
	}

	dr->_mHasInit = true;

	return dr;
}
void DatReader_Dispose(DatReader* dr)
{
	_mRefs -= 1;

	BufferReader_Dispose(dr->_mReader);

	Utils_free(dr);
}