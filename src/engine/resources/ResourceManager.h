/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */
 
#pragma once

#include "../utils/Macros.h"
#include "../io/BufferReader.h"

#define RESOURCEMANAGER_DIRECTORIES_LEN 5

typedef struct MString MString;
typedef struct DynamicByteBuffer DynamicByteBuffer;

typedef void* (*ResourceManager_FromStreamFunc)(const char* path, const char* filenameWithoutExtension, BufferReader* br);
typedef void (*ResourceManager_DisposeFunc)(void* resourceData);
typedef void (*ResourceManager_WriteFunc)(void* resourceData, DynamicByteBuffer* dbb);
typedef void (*ResourceManager_ReadFunc)(void* resourceData, BufferReader* br);

typedef struct Resource Resource;

const char* Resource_GetFilenameWithoutExtension(Resource* res);
void* Resource_GetData(Resource* res);
MString* Resource_GetPath(Resource* res);

typedef struct ResourceManager
{
	int32_t _mResourceCounter;
	struct { char* key; Resource* value; } *sh_resources;
	bool _mHasInit;
	ResourceManager_ReadFunc _mRead;
	ResourceManager_WriteFunc _mWrite;
	ResourceManager_FromStreamFunc _mFromStream;
	ResourceManager_DisposeFunc _mDispose;
	char _mDatFileName[EE_FILENAME_MAX];
	char _mResourceType[EE_FILENAME_MAX];
	char _mDefaultResource[EE_FILENAME_MAX];
	char _mFileExtension[EE_FILENAME_MAX];
	char _mDirectories[RESOURCEMANAGER_DIRECTORIES_LEN][EE_PATH_MAX];
	bool _mReadFromDirectory;
	bool _mIsReadingText;
	bool _mDelayLoading;
} ResourceManager;

void ResourceManager_Init(ResourceManager* rm);
bool ResourceManager_HasResource(ResourceManager* rm, const char* filenameWithoutExtension);
Resource* ResourceManager_GetDefaultResource(ResourceManager* rm);
void* ResourceManager_GetDefaultResourceData(ResourceManager* rm);
Resource* ResourceManager_GetResource(ResourceManager* rm, const char* filenameWithoutExtension);
void* ResourceManager_GetResourceData(ResourceManager* rm, const char* filenameWithoutExtension);
Resource* ResourceManager_LoadAssetFromStreamAndCreateResource(ResourceManager* rm, BufferReader* br, const char* filenameWithoutExtension, const char* path);
const char* ResourceManager_GetDatFileName(ResourceManager* rm);
void ResourceManager_LoadAllFromDat(ResourceManager* rm);
void ResourceManager_Dispose(ResourceManager* rm, const char* filenameWithoutExtension);
void ResourceManager_DisposeAll(ResourceManager* rm);
int64_t ResourceManager_Length(ResourceManager* rm);
Resource* ResourceManager_GetResourceByIndex(ResourceManager* rm, int32_t index);
void* ResourceManager_GetResourceDataByIndex(ResourceManager* rm, int32_t index);
const char* ResourceManager_GetKey(ResourceManager* rm, const char* filenameWithoutExtension);
void ResourceManager_Write(ResourceManager* rm, const char* filenameWithoutExtension, bool isWritingText);
void ResourceManager_WriteAll(ResourceManager* rm, bool isWritingText);
void ResourceManager_Read(ResourceManager* rm, const char* filenameWithoutExtension, bool isReadingText);
void ResourceManager_ReadAll(ResourceManager* rm, bool isReadingText);
void ResourceManager_LoadAllFromDirectories(ResourceManager* rm, bool isReadingText);