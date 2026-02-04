/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "File.h"

#include "../utils/Macros.h"
#include "SDL3/SDL.h"
#include "../utils/Utils.h"
#include "../utils/Exception.h"
#include "../utils/Cvars.h"
#include "FixedByteBuffer.h"
#include "BufferReader.h"
#include "../utils/IStringArray.h"
#include "../utils/Logger.h"

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

static const char* _mBasePath;
static const char* _mPrefPath;

#define LARGE_CHAR_BUFFER_LEN 8192
static char _mLargeCharBuffer[LARGE_CHAR_BUFFER_LEN];

void File_GetFiles(IStringArray* addToThis, const char* path, const char* pattern, bool removeExtension)
{
	int32_t count = 0;
	char** files = NULL;

	{
		MString* tempString = NULL;
		File_PathCombine2(&tempString, File_GetBasePath(), path);
		files = SDL_GlobDirectory(MString_Text(tempString), pattern, 0, &count);
		MString_Dispose(&tempString);
	}

	if (files == NULL)
	{
		Logger_LogWarning(SDL_GetError());
	}
	else
	{
		for (int i = 0; i < count; i += 1)
		{
			char* filename = files[i];
			if (removeExtension)
			{
				MString* filenameWithoutExtension = NULL;
				File_GetFileNameWithoutExtension(&filenameWithoutExtension, filename);
				IStringArray_Add(addToThis, MString_Text(filenameWithoutExtension));
				MString_Dispose(&filenameWithoutExtension);
			}
			else
			{
				IStringArray_Add(addToThis, filename);
			}
		}
		SDL_free(files);
		files = NULL;
	}
}
void File_EnumerationTest(void)
{
	MString* tempString = NULL;
	File_PathCombine3(&tempString, File_GetBasePath(), "data", "tester");
	int32_t count = 0;
	char** files = SDL_GlobDirectory(MString_Text(tempString), "*", 0, &count);
	for (int i = 0; i < count; i += 1)
	{
		char* file = files[i];
		Logger_LogInformation(file);

		int tester = 0;
		tester += 1;
	}
	MString_Dispose(&tempString);
}
FixedByteBuffer* File_ReadAll(const char* path)
{
	SDL_IOStream* rwops = NULL;
	{
		MString* tempString = NULL;
		File_PathCombine2(&tempString, File_GetBasePath(), path);
		rwops = SDL_IOFromFile(MString_Text(tempString), "rb");
		MString_Dispose(&tempString);
	}
	if (rwops == NULL)
	{
		return NULL;
	}
	int64_t dataSize = SDL_GetIOSize(rwops);
	if (dataSize < 0)
	{
		return NULL;
	}
	FixedByteBuffer* fbb = FixedByteBuffer_Create(dataSize);
	SDL_ReadIO(rwops, FixedByteBuffer_GetBuffer(fbb), dataSize);
	SDL_CloseIO(rwops);
	return fbb;
}
void File_WriteAll(const char* path, FixedByteBuffer* fbb)
{
	SDL_IOStream* rwops = NULL;
	{
		MString* tempString = NULL;
		File_PathCombine2(&tempString, File_GetBasePath(), path);
		rwops = SDL_IOFromFile(MString_Text(tempString), "w+");
		MString_Dispose(&tempString);
	}
	if (rwops == NULL)
	{
		return;
	}
	SDL_WriteIO(rwops, FixedByteBuffer_GetBuffer(fbb), FixedByteBuffer_GetLength(fbb));
	SDL_CloseIO(rwops);
}
void File_ReadAllToBigString(MString** assignToThis, BufferReader* br)
{
	uint64_t len = BufferReader_GetSize(br);
	uint8_t* bufferData = BufferReader_GetBufferData(br);
	MString_AssignEmpty(assignToThis, (int32_t)(len + 1));
	Utils_memcpy(MString_Text(*assignToThis), bufferData, len);
}
bool File_Exists(const char* path)
{
	SDL_IOStream* file = NULL;
	{
		MString* tempString = NULL;
		File_PathCombine2(&tempString, File_GetBasePath(), path);
		file = SDL_IOFromFile(MString_Text(tempString), "r");
		MString_Dispose(&tempString);
	}
	if (file != NULL)
	{
		SDL_CloseIO(file);
		return true;
	}
	else
	{
		return false;
	}
}
const char* File_GetBasePath(void)
{
	if (_mBasePath == NULL)
	{
		_mBasePath = SDL_GetBasePath();
	}

	return _mBasePath;
}
const char* File_GetPrefPath(void)
{
	if (_mPrefPath == NULL)
	{
		_mPrefPath = SDL_GetPrefPath(Cvars_Get(CVARS_ENGINE_ORGANIZATION_NAME), Cvars_Get(CVARS_ENGINE_SAVE_NAME));
	}

	return _mPrefPath;
}

void File_AppendPathSeparator(MString** str)
{
	MString_AddAssignChar(str, PATH_SEPARATOR);
}

static void File_GetFileNameHelper(MString** assignToThis, const char* path, bool removeTheExtension)
{
	int32_t len = (int32_t)Utils_strlen(path);
	
	int32_t begin;
	{
		int32_t lastPathLoc = Utils_StringIndexOf(PATH_SEPARATOR, path, len, true);
		if (lastPathLoc < 0)
		{
			begin = 0;
		}
		else
		{
			begin = lastPathLoc + 1;
		}
	}

	int32_t end;
	{
		int32_t lastPeriodLoc = Utils_StringIndexOf('.', path, len, true);
		if (!removeTheExtension || (lastPeriodLoc < 0))
		{
			end = len;
		}
		else
		{
			end = lastPeriodLoc;
		}
	}

	MString_AssignEmpty(assignToThis, end - begin + 1);
	char* returnStrText = MString_Text(*assignToThis);

	int32_t counter = 0;
	for (int32_t i = begin; i < end; i += 1)
	{
		returnStrText[counter] = path[i];
		counter += 1;
	}

	returnStrText[counter] = '\0';
}
void File_GetFileName(MString** assignToThis, const char* path)
{
	File_GetFileNameHelper(assignToThis, path, false);
}
void File_GetFileNameWithoutExtension(MString** assignToThis, const char* path)
{
	File_GetFileNameHelper(assignToThis, path, true);
}
IStringArray* File_ReadAllToStrings(BufferReader* br)
{
	uint64_t len = BufferReader_GetSize(br);
	uint8_t* bufferData = BufferReader_GetBufferData(br);
	IStringArray* sa = IStringArray_Create();
	int32_t counter = 0;
	for (int32_t i = 0; i < (len + 1); i += 1) //Add +1 to len because...
	{
		if (counter >= LARGE_CHAR_BUFFER_LEN)
		{
			Exception_Run("Buffer overflow in read all strings...", false);
			return sa;
		}

		bool isAtEndOfString = false;
		if (i == len) //We need to make sure we get the stuff at the end...
		{
			isAtEndOfString = true;
		}
		else
		{
			char c = bufferData[i];
			if (c == '\r')
			{
				continue;
			}
			if (c == '\n')
			{
				isAtEndOfString = true;
			}
			else
			{
				_mLargeCharBuffer[counter] = c;
				counter += 1;
			}
		}

		if (isAtEndOfString)
		{
			_mLargeCharBuffer[counter] = '\0';
			IStringArray_Add(sa, _mLargeCharBuffer);
			Utils_memset(_mLargeCharBuffer, 0, LARGE_CHAR_BUFFER_LEN);
			counter = 0;
		}
	}
	return sa;
}

static void CombineHelper(MString** assignToThis, const char* addedPath)
{
	MString* newStr = *assignToThis;
	char lastCharacter = MString_GetLastChar(newStr);
	if (lastCharacter != PATH_SEPARATOR)
	{
		MString_AddAssignChar(&newStr, PATH_SEPARATOR);
	}
	MString_AddAssignString(&newStr, addedPath);
	*assignToThis = newStr;
}
void File_PathCombine2(MString** assignToThis, const char* path1, const char* path2)
{
	MString_AssignString(assignToThis, path1);
	CombineHelper(assignToThis, path2);
}
void File_PathCombine3(MString** assignToThis, const char* path1, const char* path2, const char* path3)
{
	File_PathCombine2(assignToThis, path1, path2);
	CombineHelper(assignToThis, path3);
}
void File_PathCombine4(MString** assignToThis, const char* path1, const char* path2, const char* path3, const char* path4)
{
	File_PathCombine3(assignToThis, path1, path2, path3);
	CombineHelper(assignToThis, path4);
}
void File_PathCombine5(MString** assignToThis, const char* path1, const char* path2, const char* path3, const char* path4, const char* path5)
{
	File_PathCombine4(assignToThis, path1, path2, path3, path4);
	CombineHelper(assignToThis, path5);
}
void File_PathCombine6(MString** assignToThis, const char* path1, const char* path2, const char* path3, const char* path4, const char* path5, const char* path6)
{
	File_PathCombine5(assignToThis, path1, path2, path3, path4, path5);
	CombineHelper(assignToThis, path6);
}
void File_PathCombine7(MString** assignToThis, const char* path1, const char* path2, const char* path3, const char* path4, const char* path5, const char* path6, const char* path7)
{
	File_PathCombine6(assignToThis, path1, path2, path3, path4, path5, path6);
	CombineHelper(assignToThis, path7);
}
void File_PathCombine8(MString** assignToThis, const char* path1, const char* path2, const char* path3, const char* path4, const char* path5, const char* path6, const char* path7,
	const char* path8)
{
	File_PathCombine7(assignToThis, path1, path2, path3, path4, path5, path6, path7);
	CombineHelper(assignToThis, path8);
}
void File_PathCombine9(MString** assignToThis, const char* path1, const char* path2, const char* path3, const char* path4, const char* path5, const char* path6, const char* path7,
	const char* path8, const char* path9)
{
	File_PathCombine8(assignToThis, path1, path2, path3, path4, path5, path6, path7, path8);
	CombineHelper(assignToThis, path9);
}
void File_PathCombine10(MString** assignToThis, const char* path1, const char* path2, const char* path3, const char* path4, const char* path5, const char* path6, const char* path7,
	const char* path8, const char* path9, const char* path10)
{
	File_PathCombine9(assignToThis, path1, path2, path3, path4, path5, path6, path7, path8, path9);
	CombineHelper(assignToThis, path10);
}