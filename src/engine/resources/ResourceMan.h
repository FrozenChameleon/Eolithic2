/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "../io/BufferReader.h"

#define RESOURCEMAN_DIRECTORIES_LEN 5

typedef struct MString MString;
typedef struct DynamicByteBuffer DynamicByteBuffer;
typedef struct IStringArray IStringArray;

typedef void* (*ResourceMan_CreateNewFunc)();
typedef void* (*ResourceMan_CopyToFunc)(void* dst, void* src);
typedef void* (*ResourceMan_FromStreamFunc)(const char* path, const char* name, BufferReader* br);
typedef void (*ResourceMan_DisposeFunc)(void* resourceData);
typedef void (*ResourceMan_WriteFunc)(void* resourceData, DynamicByteBuffer* dbb);
typedef void (*ResourceMan_ReadFunc)(void* resourceData, BufferReader* br);

typedef struct Resource Resource;

const char* Resource_GetName(Resource* res);
void* Resource_GetData(Resource* res);
void Resource_SetData(Resource* res, void* value);
void* Resource_PeekData(Resource* res);
MString* Resource_GetPath(Resource* res);
void Resource_Save(Resource* res, bool isWritingText);

typedef struct ResourceMan
{
	int32_t _mResourceCounter;
	struct { char* key; Resource* value; } *sh_resources;
	bool _mHasInit;
	ResourceMan_ReadFunc _mRead;
	ResourceMan_WriteFunc _mWrite;
	ResourceMan_FromStreamFunc _mFromStream;
	ResourceMan_DisposeFunc _mDispose;
	ResourceMan_CreateNewFunc _mCreateNew;
	ResourceMan_CopyToFunc _mCopyTo;
	char _mDatFileName[EE_FILENAME_MAX];
	char _mResourceType[EE_FILENAME_MAX];
	char _mDefaultResource[EE_FILENAME_MAX];
	char _mFileExtension[EE_FILENAME_MAX];
	char _mDirectories[RESOURCEMAN_DIRECTORIES_LEN][EE_PATH_MAX];
	bool _mReadFromDirectory;
	bool _mIsReadingText;
	bool _mDelayLoading;
	bool _mHasLoaded;
} ResourceMan;

void ResourceMan_Init(ResourceMan* rm);
bool ResourceMan_HasResource(ResourceMan* rm, const char* name);
Resource* ResourceMan_GetDefaultResource(ResourceMan* rm);
void* ResourceMan_GetDefaultResourceData(ResourceMan* rm);
Resource* ResourceMan_GetResource(ResourceMan* rm, const char* name);
void* ResourceMan_GetResourceData(ResourceMan* rm, const char* name);
Resource* ResourceMan_CreateResource(ResourceMan* rm, const char* name, const char* path, bool createNewDefaultData);
Resource* ResourceMan_LoadAssetFromStreamAndCreateResource(ResourceMan* rm, BufferReader* br, const char* name, const char* path);
const char* ResourceMan_GetDatFileName(ResourceMan* rm);
void ResourceMan_LoadAllFromDat(ResourceMan* rm);
void ResourceMan_Dispose(ResourceMan* rm, const char* name);
void ResourceMan_DisposeAll(ResourceMan* rm);
int64_t ResourceMan_Length(ResourceMan* rm);
Resource* ResourceMan_GetResourceByIndex(ResourceMan* rm, int32_t index);
void* ResourceMan_GetResourceDataByIndex(ResourceMan* rm, int32_t index);
const char* ResourceMan_GetKey(ResourceMan* rm, const char* name);
void ResourceMan_Write(ResourceMan* rm, const char* name, bool isWritingText);
void ResourceMan_WriteAll(ResourceMan* rm, bool isWritingText);
void ResourceMan_Read(ResourceMan* rm, const char* name, bool isReadingText);
void ResourceMan_ReadAll(ResourceMan* rm, bool isReadingText);
void ResourceMan_LoadAllFromDirectories(ResourceMan* rm, bool isReadingText);
//Pointer that is returned must be freed!
void ResourceMan_FillArrayWithAllResourceNames(ResourceMan* rm, IStringArray* sa);
//CopyTo src to dst and then save. Pass NULL path to use directory[0]
void ResourceMan_CopyToResourceDataAndThenSaveAsText(ResourceMan* rm, const char* name, const char* path, void* copyThisData);