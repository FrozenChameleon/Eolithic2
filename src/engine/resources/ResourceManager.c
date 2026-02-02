/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */
 
#include "ResourceManager.h"

#include "../../third_party/stb_ds.h"
#include "../utils/Utils.h"
#include "../utils/Logger.h"
#include "../io/File.h"
#include "../io/DatReader.h"
#include "../utils/IStrings.h"
#include "../io/DynamicByteBuffer.h"

static void PrintHelper(const char* str1, const char* str2)
{
	MString* tempString = NULL;
	MString_Combine2(&tempString, str1, str2);
	Logger_LogInformation(MString_Text(tempString));
	MString_Dispose(&tempString);
}
static void PrintResourceType(ResourceManager* rm)
{
	PrintHelper("Resource type: ", rm->_mResourceType);
}

void ResourceManager_Init(ResourceManager* rm)
{
	if (rm->_mHasInit)
	{
		return;
	}

	sh_new_arena(rm->sh_resources);

	rm->_mHasInit = true;
}
bool ResourceManager_HasResource(ResourceManager* rm, const char* filenameWithoutExtension)
{
	int64_t index = shgeti(rm->sh_resources, filenameWithoutExtension);
	if (index < 0)
	{
		return false;
	}
	return true;
}
Resource* ResourceManager_GetDefaultResource(ResourceManager* rm)
{
	if (!Utils_StringEqualTo(rm->_mDefaultResource, EE_STR_EMPTY))
	{
		return ResourceManager_GetResource(rm, rm->_mDefaultResource);
	}
	else
	{
		return NULL;
	}
}
void* ResourceManager_GetDefaultResourceData(ResourceManager* rm)
{
	Resource* resource = ResourceManager_GetDefaultResource(rm);
	if (resource != NULL)
	{
		return resource->mData;
	}
	else
	{
		return NULL;
	}
}
Resource* ResourceManager_GetResource(ResourceManager* rm, const char* filenameWithoutExtension)
{
	Resource* resource = shget(rm->sh_resources, filenameWithoutExtension);
	if (resource == NULL)
	{
		PrintHelper("Unable to get resource: ", filenameWithoutExtension);
		PrintResourceType(rm);
		return ResourceManager_GetDefaultResource(rm);
	}
	return resource;
}
void* ResourceManager_GetResourceData(ResourceManager* rm, const char* filenameWithoutExtension)
{
	Resource* resource = ResourceManager_GetResource(rm, filenameWithoutExtension);
	if (resource == NULL)
	{
		PrintHelper("Unable to get resource data: ", filenameWithoutExtension);
		PrintResourceType(rm);
		return NULL;
	}
	return resource->mData;
}
Resource* ResourceManager_LoadAssetFromStreamAndCreateResource(ResourceManager* rm, BufferReader* br, const char* filenameWithoutExtension, const char* path)
{
	Resource* resource = (Resource*)Utils_calloc(1, sizeof(Resource));
	Utils_memset(resource, 0, sizeof(Resource));
	MString_AssignString(&resource->mPath, path);
	Utils_strlcpy(resource->mFileNameWithoutExtension, filenameWithoutExtension, EE_FILENAME_MAX);
	resource->mID = rm->_mResourceCounter;
	rm->_mResourceCounter += 1;
	if (rm->_mFromStream != NULL)
	{
		resource->mData = rm->_mFromStream(MString_Text(resource->mPath), resource->mFileNameWithoutExtension, br);
	}
	shput(rm->sh_resources, resource->mFileNameWithoutExtension, resource);
	return resource;
}
const char* ResourceManager_GetDatFileName(ResourceManager* rm)
{
	return rm->_mDatFileName;
}
void ResourceManager_LoadAllFromDat(ResourceManager* rm)
{
	MString* path = NULL;
	File_PathCombine2(&path, "data", ResourceManager_GetDatFileName(rm));
	if (!File_Exists(MString_Text(path)))
	{
		PrintHelper("Unable to load from dat: ", MString_Text(path));
		PrintResourceType(rm);
		return;
	}
	
	DatReader* dr = DatReader_Create(MString_Text(path));
	while (DatReader_HasNext(dr))
	{
		MString* nextPath = NULL;
		MString* fileName = NULL;
		MString* fileNameWithoutExtension = NULL;
		DatReader_NextFilePath(&nextPath, dr);
		File_GetFileName(&fileName, MString_Text(nextPath));
		File_GetFileNameWithoutExtension(&fileNameWithoutExtension, MString_Text(nextPath));
		BufferReader* br = DatReader_NextStream(dr, false);
		ResourceManager_LoadAssetFromStreamAndCreateResource(rm, br, MString_Text(fileNameWithoutExtension), MString_Text(nextPath));
		BufferReader_Dispose(br);
		MString_Dispose(&nextPath);
		MString_Dispose(&fileName);
		MString_Dispose(&fileNameWithoutExtension);
	}

	DatReader_Dispose(dr);
	MString_Dispose(&path);
}
void ResourceManager_Dispose(ResourceManager* rm, const char* filenameWithoutExtension)
{
	Resource* resource = shget(rm->sh_resources, filenameWithoutExtension);
	if ((resource->mData != NULL) && (rm->_mDispose != NULL))
	{
		rm->_mDispose(resource->mData);
		resource->mData = NULL;
	}
	Utils_free(resource);
	shdel(rm->sh_resources, filenameWithoutExtension);
}
void ResourceManager_DisposeAll(ResourceManager* rm)
{
	for (int32_t i = 0; i < shlen(rm->sh_resources); i += 1)
	{
		ResourceManager_Dispose(rm, rm->sh_resources[i].key);
	}

	shfree(rm->sh_resources);
	rm->sh_resources = NULL;
	rm->_mResourceCounter = 0;
	rm->_mHasInit = false;
}
int64_t ResourceManager_Length(ResourceManager* rm)
{
	return shlen(rm->sh_resources);
}
Resource* ResourceManager_GetResourceByIndex(ResourceManager* rm, int32_t index)
{
	return rm->sh_resources[index].value;
}
void* ResourceManager_GetResourceDataByIndex(ResourceManager* rm, int32_t index)
{
	return rm->sh_resources[index].value->mData;
}
const char* ResourceManager_GetKey(ResourceManager* rm, const char* filenameWithoutExtension)
{
	int64_t index = shgeti(rm->sh_resources, filenameWithoutExtension);
	if (index < 0)
	{
		PrintHelper("Unable to get key: ", filenameWithoutExtension);
		return NULL;
	}

	return rm->sh_resources[index].key;
}
void ResourceManager_Write(ResourceManager* rm, const char* filenameWithoutExtension, bool isWritingText)
{
	if (rm->_mWrite == NULL)
	{
		return;
	}

	Resource* resource = ResourceManager_GetResource(rm, filenameWithoutExtension);
	if ((resource == NULL) || (resource->mData == NULL))
	{
		MString* tempString = NULL;
		MString_Combine2(&tempString, "Unable to write: ", filenameWithoutExtension);
		Logger_LogWarning(MString_Text(tempString));
		MString_Dispose(&tempString);
		return;
	}

	DynamicByteBuffer* dbb = DynamicByteBuffer_Create();
	DynamicByteBuffer_SetIsWritingText(dbb, isWritingText);
	rm->_mWrite(resource->mData, dbb);
	FixedByteBuffer* fbb = DynamicByteBuffer_ConvertToFixedByteBufferAndDisposeDBB(dbb);
	File_WriteAll(MString_Text(resource->mPath), fbb);
	FixedByteBuffer_Dispose(fbb);

	{
		MString* tempString = NULL;
		MString_Combine2(&tempString, "Wrote to: ", MString_Text(resource->mPath));
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}
}
void ResourceManager_WriteAll(ResourceManager* rm, bool isWritingText)
{
	for (int32_t i = 0; i < shlen(rm->sh_resources); i += 1)
	{
		ResourceManager_Write(rm, rm->sh_resources[i].key, isWritingText);
	}
}

void ResourceManager_Read(ResourceManager* rm, const char* filenameWithoutExtension, bool isReadingText)
{
	if (rm->_mRead == NULL)
	{
		return;
	}

	Resource* resource = ResourceManager_GetResource(rm, filenameWithoutExtension);
	if ((resource == NULL) || (resource->mData == NULL) || !File_Exists(MString_Text(resource->mPath)))
	{
		MString* tempString = NULL;
		MString_Combine2(&tempString, "Unable to read: ", filenameWithoutExtension);
		Logger_LogWarning(MString_Text(tempString));
		MString_Dispose(&tempString);
		return;
	}

	{
		FixedByteBuffer* fbb = File_ReadAll(MString_Text(resource->mPath));
		BufferReader* br = BufferReader_Create(fbb);
		BufferReader_SetIsReadingText(br, isReadingText);
		rm->_mRead(resource->mData, br);
		BufferReader_Dispose(br);
	}

	{
		MString* tempString = NULL;
		MString_Combine2(&tempString, "Read from: ", MString_Text(resource->mPath));
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}
}

void ResourceManager_ReadAll(ResourceManager* rm, bool isReadingText)
{
	for (int32_t i = 0; i < shlen(rm->sh_resources); i += 1)
	{
		ResourceManager_Read(rm, rm->sh_resources[i].key, isReadingText);
	}
}
