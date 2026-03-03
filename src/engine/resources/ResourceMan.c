/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */
 
#include "ResourceMan.h"

#include "../../third_party/stb_ds.h"
#include "../utils/Utils.h"
#include "../utils/Logger.h"
#include "../io/File.h"
#include "../io/DatReader.h"
#include "../utils/IStrings.h"
#include "../utils/IStringArray.h"
#include "../io/DynamicByteBuffer.h"
#include "../core/Func.h"

typedef struct Resource
{
	void* data;
	ResourceMan* manager;
	ResourceID id;
	MString* path;
	char name[EE_FILENAME_MAX];
} Resource;

static void* LoadResourceDataFromBufferReader(Resource* resource, BufferReader* br)
{
	if ((resource->path == NULL) || (resource->name == NULL))
	{
		Logger_LogInformation("Resource missing path!");
		return NULL;
	}

	return resource->manager->_mFromStream(MString_Text(resource->path), resource->name, br);
}
static void LoadResourceDataFromResourcePath(Resource* resource)
{
	if ((resource->path == NULL) || (resource->name == NULL))
	{
		Logger_LogInformation("Resource missing path!");
		return;
	}

	BufferReader* br = BufferReader_CreateFromPath(MString_Text(resource->path));
	resource->data = LoadResourceDataFromBufferReader(resource, br);
	BufferReader_Dispose(br);

	{
		MString* tempString = MString_CreateForJustThisFrame();
		MString_Combine2(&tempString, "Loaded resource late: ", resource->name);
		Logger_LogInformation(MString_Text(tempString));
	}
}
static void LoadResourceDataFromResourcePathIfMissing(Resource* resource)
{
	if (resource->data != NULL)
	{
		return;
	}

	LoadResourceDataFromResourcePath(resource);
}

static void PrintHelper(const char* str1, const char* str2)
{
	MString* tempString = MString_CreateForJustThisFrame();
	MString_Combine2(&tempString, str1, str2);
	Logger_LogInformation(MString_Text(tempString));
}
static void PrintResourceType(ResourceMan* rm)
{
	PrintHelper("Resource type: ", rm->_mResourceType);
}

const char* Resource_GetName(Resource* res)
{
	return res->name;
}
void* Resource_GetData(Resource* res)
{
	if (res->data == NULL)
	{
		LoadResourceDataFromResourcePathIfMissing(res);
	}
	return res->data;
}
void Resource_SetData(Resource* res, void* value)
{
	res->data = value;
}
void* Resource_PeekData(Resource* res)
{
	return res->data;
}
const char* Resource_GetPath(Resource* res)
{
	return MString_Text(res->path);
}
void Resource_Save(Resource* res, bool isWritingText)
{
	ResourceMan_Write(res->manager, res->name, isWritingText);
}

void ResourceMan_Init(ResourceMan* rm)
{
	if (rm->_mHasInit)
	{
		return;
	}

	sh_new_arena(rm->sh_resources);

	rm->_mHasInit = true;
}
bool ResourceMan_HasResource(ResourceMan* rm, const char* name)
{
	int64_t index = shgeti(rm->sh_resources, name);
	if (index < 0)
	{
		return false;
	}
	return true;
}
Resource* ResourceMan_GetDefaultResource(ResourceMan* rm)
{
	if (!Utils_StringEqualTo(rm->_mDefaultResource, EE_STR_EMPTY))
	{
		return ResourceMan_GetResource(rm, rm->_mDefaultResource);
	}
	else
	{
		return NULL;
	}
}
void* ResourceMan_GetDefaultResourceData(ResourceMan* rm)
{
	Resource* resource = ResourceMan_GetDefaultResource(rm);
	if (resource != NULL)
	{
		return resource->data;
	}
	else
	{
		return NULL;
	}
}
static Resource* JustGetTheResource(ResourceMan* rm, const char* name)
{
	return shget(rm->sh_resources, name);;
}
Resource* ResourceMan_GetResource(ResourceMan* rm, const char* name)
{
	Resource* resource = JustGetTheResource(rm, name);
	if (resource == NULL)
	{
		if (Utils_StringEqualTo(name, rm->_mDefaultResource))
		{
			PrintHelper("Unable to get default resource!", name);
			PrintResourceType(rm);
			return NULL;
		}
		else
		{
			PrintHelper("Unable to get resource: ", name);
			PrintResourceType(rm);
			return ResourceMan_GetDefaultResource(rm);
		}
	}
	return resource;
}
void* ResourceMan_GetResourceData(ResourceMan* rm, const char* name)
{
	Resource* resource = ResourceMan_GetResource(rm, name);
	if (resource == NULL)
	{
		PrintHelper("Unable to get resource data: ", name);
		PrintResourceType(rm);
		return NULL;
	}
	LoadResourceDataFromResourcePathIfMissing(resource);
	return resource->data;
}
Resource* ResourceMan_CreateResource(ResourceMan* rm, const char* name, const char* path, bool createNewDefaultData)
{
	Resource* resource;
	if (ResourceMan_HasResource(rm, name))
	{
		resource = JustGetTheResource(rm, name);
	}
	else
	{
		{
			MString* tempString = MString_CreateForJustThisFrame();
			MString_Combine4(&tempString, "Creating ", rm->_mResourceType, ": ", name);
			Logger_LogInformation(MString_Text(tempString));
		}

		resource = (Resource*)Utils_calloc(1, sizeof(Resource));
		resource->manager = rm;
		MString_AssignString(&resource->path, path);
		Utils_strlcpy(resource->name, name, EE_FILENAME_MAX);
		resource->id = rm->_mResourceCounter;
		rm->_mResourceCounter += 1;
		shput(rm->sh_resources, resource->name, resource);
	}

	if (createNewDefaultData && (resource->data == NULL) && (rm->_mCreateNew != NULL))
	{
		resource->data = rm->_mCreateNew();
	}

	return resource;
}
Resource* ResourceMan_LoadAssetFromStreamAndCreateResource(ResourceMan* rm, BufferReader* br, const char* name, const char* path)
{
	{
		MString* tempString = MString_CreateForJustThisFrame();
		MString_Combine4(&tempString, "Loading ", rm->_mResourceType, " from: ", path);
		Logger_LogInformation(MString_Text(tempString));
	}

	Resource* resource = ResourceMan_CreateResource(rm, name, path, false);
	if (!rm->_mDelayLoading && (rm->_mFromStream != NULL))
	{
		resource->data = LoadResourceDataFromBufferReader(resource, br);
	}
	return resource;
}
const char* ResourceMan_GetDatFileName(ResourceMan* rm)
{
	return rm->_mDatFileName;
}
void ResourceMan_LoadAllFromDat(ResourceMan* rm)
{
	if (rm->_mReadFromDirectory)
	{
		ResourceMan_LoadAllFromDirectories(rm, rm->_mIsReadingText);
		return;
	}

	MString* path = MString_CreateForJustThisFrame();
	File_PathCombine2(&path, "data", ResourceMan_GetDatFileName(rm));
	if (!File_Exists(MString_Text(path)))
	{
		PrintHelper("Unable to load from dat: ", MString_Text(path));
		PrintResourceType(rm);
		return;
	}
	
	DatReader* dr = DatReader_Create(MString_Text(path));
	while (DatReader_HasNext(dr))
	{
		MString* nextPath = MString_CreateForJustThisFrame();
		MString* fileName = MString_CreateForJustThisFrame();
		MString* name = MString_CreateForJustThisFrame();
		DatReader_NextFilePath(&nextPath, dr);
		File_GetFilename(&fileName, MString_Text(nextPath));
		File_GetFilenameWithoutExtension(&name, MString_Text(nextPath));
		BufferReader* br = DatReader_NextStream(dr, false);
		ResourceMan_LoadAssetFromStreamAndCreateResource(rm, br, MString_Text(name), MString_Text(nextPath));
		BufferReader_Dispose(br);
	}

	DatReader_Dispose(dr);
}
void ResourceMan_Dispose(ResourceMan* rm, const char* name)
{
	Resource* resource = shget(rm->sh_resources, name);
	if ((resource->data != NULL) && (rm->_mDispose != NULL))
	{
		rm->_mDispose(resource->data);
		resource->data = NULL;
	}
	Utils_free(resource);
	shdel(rm->sh_resources, name);
}
void ResourceMan_DisposeAll(ResourceMan* rm)
{
	for (int32_t i = 0; i < shlen(rm->sh_resources); i += 1)
	{
		ResourceMan_Dispose(rm, rm->sh_resources[i].key);
	}

	shfree(rm->sh_resources);
	rm->sh_resources = NULL;
	rm->_mResourceCounter = 0;
	rm->_mHasInit = false;
}
int64_t ResourceMan_Length(ResourceMan* rm)
{
	return shlen(rm->sh_resources);
}
Resource* ResourceMan_GetResourceByIndex(ResourceMan* rm, int32_t index)
{
	return rm->sh_resources[index].value;
}
void* ResourceMan_GetResourceDataByIndex(ResourceMan* rm, int32_t index)
{
	Resource* resource = rm->sh_resources[index].value;
	LoadResourceDataFromResourcePathIfMissing(resource);
	return resource->data;
}
const char* ResourceMan_GetKey(ResourceMan* rm, const char* name)
{
	int64_t index = shgeti(rm->sh_resources, name);
	if (index < 0)
	{
		PrintHelper("Unable to get key: ", name);
		return NULL;
	}

	return rm->sh_resources[index].key;
}
void ResourceMan_Write(ResourceMan* rm, const char* name, bool isWritingText)
{
	if (rm->_mWrite == NULL)
	{
		return;
	}

	Resource* resource = ResourceMan_GetResource(rm, name);
	if ((resource == NULL) || (resource->data == NULL))
	{
		MString* tempString = MString_CreateForJustThisFrame();
		MString_Combine2(&tempString, "Unable to write: ", name);
		Logger_LogWarning(MString_Text(tempString));
		return;
	}

	DynamicByteBuffer* dbb = DynamicByteBuffer_Create();
	DynamicByteBuffer_SetIsWritingText(dbb, isWritingText);
	rm->_mWrite(resource->data, dbb);
	FixedByteBuffer* fbb = DynamicByteBuffer_ConvertToFixedByteBufferAndDisposeDBB(dbb);
	File_WriteAll(MString_Text(resource->path), fbb);
	FixedByteBuffer_Dispose(fbb);

	{
		MString* tempString = NULL;
		MString_Combine2(&tempString, "Wrote to: ", MString_Text(resource->path));
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}
}
void ResourceMan_WriteAll(ResourceMan* rm, bool isWritingText)
{
	for (int32_t i = 0; i < shlen(rm->sh_resources); i += 1)
	{
		ResourceMan_Write(rm, rm->sh_resources[i].key, isWritingText);
	}
}
void ResourceMan_Read(ResourceMan* rm, const char* name, bool isReadingText)
{
	if (rm->_mRead == NULL)
	{
		return;
	}

	Resource* resource = ResourceMan_GetResource(rm, name);
	if ((resource == NULL) || (resource->data == NULL) || !File_Exists(MString_Text(resource->path)))
	{
		MString* tempString = NULL;
		MString_Combine2(&tempString, "Unable to read: ", name);
		Logger_LogWarning(MString_Text(tempString));
		MString_Dispose(&tempString);
		return;
	}

	{
		FixedByteBuffer* fbb = File_ReadAll(MString_Text(resource->path));
		BufferReader* br = BufferReader_Create(fbb);
		BufferReader_SetIsReadingText(br, isReadingText);
		rm->_mRead(resource->data, br);
		BufferReader_Dispose(br);
	}

	{
		MString* tempString = NULL;
		MString_Combine2(&tempString, "Read from: ", MString_Text(resource->path));
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}
}
void ResourceMan_ReadAll(ResourceMan* rm, bool isReadingText)
{
	for (int32_t i = 0; i < shlen(rm->sh_resources); i += 1)
	{
		ResourceMan_Read(rm, rm->sh_resources[i].key, isReadingText);
	}
}
static void LoadAllFromDirectoriesHelper(ResourceMan* rm, bool isReadingText, const char* directory)
{
	if (Utils_StringEqualTo(directory, EE_STR_EMPTY))
	{
		return;
	}

	IStringArray* filePaths = IStringArray_CreateForJustThisFrame();
	File_GetFilePaths(filePaths, directory, rm->_mFileExtension);

	for (int i = 0; i < IStringArray_Length(filePaths); i += 1)
	{
		MString* nextPath = NULL;
		const char* filePath = IStringArray_Get(filePaths, i);
		MString* name = NULL;
		File_PathCombine2(&nextPath, directory, filePath);
		File_GetFilenameWithoutExtension(&name, MString_Text(nextPath));
		BufferReader* br = BufferReader_CreateFromPath(MString_Text(nextPath));
		BufferReader_SetIsReadingText(br, isReadingText);
		ResourceMan_LoadAssetFromStreamAndCreateResource(rm, br, MString_Text(name), MString_Text(nextPath));
		BufferReader_Dispose(br);
		MString_Dispose(&nextPath);
		MString_Dispose(&name);
	}
}
void ResourceMan_LoadAllFromDirectories(ResourceMan* rm, bool isReadingText)
{
	for (int i = 0; i < RESOURCEMAN_DIRECTORIES_LEN; i += 1)
	{
		LoadAllFromDirectoriesHelper(rm, isReadingText, rm->_mDirectories[i]);
	}
}
void ResourceMan_FillArrayWithAllResourceNames(ResourceMan* rm, IStringArray* sa)
{
	for (int i = 0; i < ResourceMan_Length(rm); i += 1)
	{
		Resource* res = ResourceMan_GetResourceByIndex(rm, i);
		IStringArray_Add(sa, Resource_GetName(res));
	}
}
void ResourceMan_CopyToResourceDataAndThenSaveAsText(ResourceMan* rm, const char* name, const char* path, void* copyThisData)
{
	if (path == NULL)
	{
		if (Utils_IsStringEmpty(rm->_mDirectories[0]))
		{
			Logger_LogWarning("Invalid directory to save resource.");
			return;
		}

		MString* tempString = MString_CreateForJustThisFrame();
		File_PathCombine2(&tempString, rm->_mDirectories[0], name);
		MString_AddAssignString(&tempString, ".txt");
		path = MString_Text(tempString);
	}

	if (Utils_IsStringEmptyOrNotSet(name))
	{
		Logger_LogWarning("Invalid filename to save resource.");
		return;
	}

	{
		MString* strDebugInfo = MString_CreateForJustThisFrame();
		if (ResourceMan_HasResource(rm, name))
		{
			MString_Combine2(&strDebugInfo, "Overwriting resource ", path);
		}
		else
		{
			MString_Combine2(&strDebugInfo, "Saving new resource ", path);
		}
		Logger_LogInformation(MString_Text(strDebugInfo));
	}

	Resource* resource = ResourceMan_CreateResource(rm, name, path, true);
	rm->_mCopyTo(resource->data, copyThisData);
	Resource_Save(resource, true);
	Do_PlaySound2("editorSave", 1.0f);
}
