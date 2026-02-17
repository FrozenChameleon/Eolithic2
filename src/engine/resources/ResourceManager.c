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
#include "../utils/IStringArray.h"
#include "../io/DynamicByteBuffer.h"

typedef struct Resource
{
	void* mData;
	ResourceManager* mManager;
	ResourceID mID;
	MString* mPath;
	char mFilenameWithoutExtension[EE_FILENAME_MAX];
} Resource;

static void* LoadResourceDataFromBufferReader(Resource* resource, BufferReader* br)
{
	if ((resource->mPath == NULL) || (resource->mFilenameWithoutExtension == NULL))
	{
		Logger_LogInformation("Resource missing path!");
		return NULL;
	}

	return resource->mManager->_mFromStream(MString_Text(resource->mPath), resource->mFilenameWithoutExtension, br);
}
static void LoadResourceDataFromResourcePath(Resource* resource)
{
	if ((resource->mPath == NULL) || (resource->mFilenameWithoutExtension == NULL))
	{
		Logger_LogInformation("Resource missing path!");
		return;
	}

	BufferReader* br = BufferReader_CreateFromPath(MString_Text(resource->mPath));
	resource->mData = LoadResourceDataFromBufferReader(resource, br);
	BufferReader_Dispose(br);

	{
		MString* tempString = NULL;
		MString_Combine2(&tempString, "Loaded resource late: ", resource->mFilenameWithoutExtension);
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}
}
static void LoadResourceDataFromResourcePathIfMissing(Resource* resource)
{
	if (resource->mData != NULL)
	{
		return;
	}

	LoadResourceDataFromResourcePath(resource);
}

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

const char* Resource_GetFilenameWithoutExtension(Resource* res)
{
	return res->mFilenameWithoutExtension;
}
void* Resource_GetData(Resource* res)
{
	LoadResourceDataFromResourcePathIfMissing(res);
	return res->mData;
}
MString* Resource_GetPath(Resource* res)
{
	return res->mPath;
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
		if (Utils_StringEqualTo(filenameWithoutExtension, rm->_mDefaultResource))
		{
			PrintHelper("Unable to get default resource!", filenameWithoutExtension);
			PrintResourceType(rm);
			return NULL;
		}
		else
		{
			PrintHelper("Unable to get resource: ", filenameWithoutExtension);
			PrintResourceType(rm);
			return ResourceManager_GetDefaultResource(rm);
		}
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
	LoadResourceDataFromResourcePathIfMissing(resource);
	return resource->mData;
}
Resource* ResourceManager_LoadAssetFromStreamAndCreateResource(ResourceManager* rm, BufferReader* br, const char* filenameWithoutExtension, const char* path)
{
	{
		MString* tempString = NULL;
		MString_Combine4(&tempString, "Loading ", rm->_mResourceType, " from: ", path);
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}
	Resource* resource = (Resource*)Utils_calloc(1, sizeof(Resource));
	resource->mManager = rm;
	MString_AssignString(&resource->mPath, path);
	Utils_strlcpy(resource->mFilenameWithoutExtension, filenameWithoutExtension, EE_FILENAME_MAX);
	resource->mID = rm->_mResourceCounter;
	rm->_mResourceCounter += 1;
	if (!rm->_mDelayLoading && (rm->_mFromStream != NULL))
	{
		resource->mData = LoadResourceDataFromBufferReader(resource, br);
	}
	shput(rm->sh_resources, resource->mFilenameWithoutExtension, resource);
	return resource;
}
const char* ResourceManager_GetDatFileName(ResourceManager* rm)
{
	return rm->_mDatFileName;
}
void ResourceManager_LoadAllFromDat(ResourceManager* rm)
{
	if (rm->_mReadFromDirectory)
	{
		ResourceManager_LoadAllFromDirectories(rm, rm->_mIsReadingText);
		return;
	}

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
	Resource* resource = rm->sh_resources[index].value;
	LoadResourceDataFromResourcePathIfMissing(resource);
	return resource->mData;
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
static void LoadAllFromDirectoriesHelper(ResourceManager* rm, bool isReadingText, const char* directory)
{
	if (Utils_StringEqualTo(directory, EE_STR_EMPTY))
	{
		return;
	}

	IStringArray* filePaths = IStringArray_Create();
	File_GetFilePaths(filePaths, directory, rm->_mFileExtension);

	for (int i = 0; i < IStringArray_Length(filePaths); i += 1)
	{
		MString* nextPath = NULL;
		const char* filePath = IStringArray_Get(filePaths, i);
		MString* fileNameWithoutExtension = NULL;
		File_PathCombine2(&nextPath, directory, filePath);
		File_GetFileNameWithoutExtension(&fileNameWithoutExtension, MString_Text(nextPath));
		BufferReader* br = BufferReader_CreateFromPath(MString_Text(nextPath));
		BufferReader_SetIsReadingText(br, isReadingText);
		ResourceManager_LoadAssetFromStreamAndCreateResource(rm, br, MString_Text(fileNameWithoutExtension), MString_Text(nextPath));
		BufferReader_Dispose(br);
		MString_Dispose(&nextPath);
		MString_Dispose(&fileNameWithoutExtension);
	}

	IStringArray_Dispose(filePaths);
}
void ResourceManager_LoadAllFromDirectories(ResourceManager* rm, bool isReadingText)
{
	for (int i = 0; i < RESOURCEMANAGER_DIRECTORIES_LEN; i += 1)
	{
		LoadAllFromDirectoriesHelper(rm, isReadingText, rm->_mDirectories[i]);
	}
}
