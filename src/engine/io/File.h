/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "../utils/MString.h"

typedef struct FixedByteBuffer FixedByteBuffer;
typedef struct BufferReader BufferReader;
typedef struct IStringArray IStringArray;

void File_GetFilenames(IStringArray* addToThis, const char* path, const char* pattern, bool removeTheExtension);
void File_GetFilePaths(IStringArray* addToThis, const char* path, const char* onlyWithThisExtension);
void File_AppendPathSeparator(MString** str);
FixedByteBuffer* File_ReadAll(const char* path);
void File_WriteAll(const char* path, FixedByteBuffer* fbb);
void File_ReadAllToBigString(MString** assignToThis, BufferReader* br);
bool File_Exists(const char* path);
const char* File_GetBasePath(void);
const char* File_GetPrefPath(void);

void File_GetFileName(MString** assignToThis, const char* path);
void File_GetFileNameWithoutExtension(MString** assignToThis, const char* path);
IStringArray* File_ReadAllToStrings(BufferReader* br);

void File_PathCombine2(MString** assignToThis, const char* path1, const char* path2);
void File_PathCombine3(MString** assignToThis, const char* path1, const char* path2, const char* path3);
void File_PathCombine4(MString** assignToThis, const char* path1, const char* path2, const char* path3, const char* path4);
void File_PathCombine5(MString** assignToThis, const char* path1, const char* path2, const char* path3, const char* path4, const char* path5);
void File_PathCombine6(MString** assignToThis, const char* path1, const char* path2, const char* path3, const char* path4, const char* path5, const char* path6);
void File_PathCombine7(MString** assignToThis, const char* path1, const char* path2, const char* path3, const char* path4, const char* path5, const char* path6, const char* path7);
void File_PathCombine8(MString** assignToThis, const char* path1, const char* path2, const char* path3, const char* path4, const char* path5, const char* path6, const char* path7,
	const char* path8);
void File_PathCombine9(MString** assignToThis, const char* path1, const char* path2, const char* path3, const char* path4, const char* path5, const char* path6, const char* path7,
	const char* path8, const char* path9);
void File_PathCombine10(MString** assignToThis, const char* path1, const char* path2, const char* path3, const char* path4, const char* path5, const char* path6, const char* path7,
	const char* path8, const char* path9, const char* path10);