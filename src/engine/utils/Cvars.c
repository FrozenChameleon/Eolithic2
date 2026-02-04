/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Cvars.h"

#include "Macros.h"
#include "../../third_party/stb_ds.h"
#include "Utils.h"
#include "../utils/MString.h"
#include "../service/Service.h"
#include "../utils/Logger.h"
#include "../globals/Globals.h"
#include "../utils/Exception.h"
#include "../utils/IStringArray.h"
#include "../io/DynamicByteBuffer.h"
#include "../io/File.h"
#include "../io/BufferReader.h"
#include "../utils/IStringMap.h"

#define STR_NOT_SET "NOT_SET"
#define STR_ZERO "0"
#define STR_ONE "1"

#define USER_CONFIG_FILENAME "userconfig.txt"
#define CONTAINER_DISPLAY_NAME "GameConfig"
#define CONTAINER_NAME "GameConfigContainer"

//private to definition
typedef struct CvarData
{
	float mCachedNumber;
	bool mHasBeenSet;
	bool mDoNotRefreshCachedNumber;
	char mKey[EE_FILENAME_MAX];
	char mValue[EE_FILENAME_MAX];
} CvarData;

static bool _mHasInit;
static bool _mHasLoadedSaveDataCvars;
static struct { char* key; CvarData value; } *sh_cvars;
static struct { char* key; CvarData value; } *sh_user_defaults;

static void Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	sh_new_arena(sh_cvars);
	sh_new_arena(sh_user_defaults);

	_mHasInit = true;
}
static CvarData* GetCvarData(const char* key, const char* valueIfCvarHasNotBeenSet)
{
	int64_t index = shgeti(sh_cvars, key);
	if (index == -1)
	{
		CvarData cvar = { 0 };
		cvar.mCachedNumber = 0;
		cvar.mHasBeenSet = true;
		cvar.mDoNotRefreshCachedNumber = false;
		Utils_strlcpy(cvar.mKey, key, EE_FILENAME_MAX);
		Utils_strlcpy(cvar.mValue, valueIfCvarHasNotBeenSet, EE_FILENAME_MAX);
		shput(sh_cvars, key, cvar);
		index = shgeti(sh_cvars, key);
		if (index == -1)
		{
			return NULL; //Something is seriously wrong!
		}
	}
	return &sh_cvars[index].value;
}
static void SetupCvarForSet(CvarData* cvar, const char* key)
{
	cvar->mCachedNumber = 0;
	cvar->mHasBeenSet = true;
	cvar->mDoNotRefreshCachedNumber = false;
	Utils_strlcpy(cvar->mKey, key, EE_FILENAME_MAX);
}
static void CopyToUserDefaults(void)
{
	shfree(sh_user_defaults);
	sh_new_arena(sh_user_defaults);

	for (int32_t i = 0; i < shlen(sh_cvars); i += 1)
	{
		if (Utils_StringStartsWith(sh_cvars[i].key, CVARS_PREFIX_USER_CONFIG))
		{
			shput(sh_user_defaults, sh_cvars[i].key, sh_cvars[i].value);
		}
	}
}
static bool ReadDataCvars2(const char* pathWithoutExtension, const char* debugName, bool isBinary)
{
	const char* extension = Utils_GetExtension(isBinary); //TODO BINARY STUFF
	extension = ".txt";
	MString* path = NULL;
	MString_Combine2(&path, pathWithoutExtension, extension);
	bool successfullyReadTheFile = false;
	bool lookForTextVersion = false;
	if (File_Exists(MString_Text(path)))
	{
		BufferReader* reader = BufferReader_CreateFromPath(MString_Text(path));
		BufferReader_SetIsReadingText(reader, true);
		Cvars_Read(isBinary, reader);
		BufferReader_Dispose(reader);
		{
			MString* tempString = NULL;
			MString_Combine3(&tempString, debugName, " data dir cvars loaded from ", MString_Text(path));
			Logger_LogInformation(MString_Text(tempString));
			MString_Dispose(&tempString);
		}
		successfullyReadTheFile = true;
	}
	else
	{
		{
			MString* tempString = NULL;
			MString_Combine3(&tempString, debugName, " data dir cvars missing at ", MString_Text(path));
			Logger_LogInformation(MString_Text(tempString));
			MString_Dispose(&tempString);
		}
		if (isBinary)
		{
			lookForTextVersion = true;
		}
		else
		{
			successfullyReadTheFile = false;
		}
	}

	MString_Dispose(&path);

	if (lookForTextVersion)
	{
		return ReadDataCvars2(pathWithoutExtension, debugName, false);
	}
	else
	{
		return successfullyReadTheFile;
	}
}
static bool ReadDataCvars(const char* pathWithoutExtension, const char* debugName)
{
	return ReadDataCvars2(pathWithoutExtension, debugName, Utils_IsBinaryForDebugFlag());
}

void Cvars_FlipAsBool(const char* key)
{
	//
	Init();
	//

	Cvars_SetAsBool(key, !Cvars_GetAsBool(key));
}
void Cvars_SetAsBool(const char* key, bool value)
{
	//
	Init();
	//

	if (!value)
	{
		Cvars_Set(key, STR_ZERO);
	}
	else
	{
		Cvars_Set(key, STR_ONE);
	}
}
void Cvars_SetAsInt(const char* key, int32_t value)
{
	//
	Init();
	//

	CvarData* cvar = GetCvarData(key, STR_NOT_SET);
	SetupCvarForSet(cvar, key);
	Utils_IntToString(value, cvar->mValue, EE_FILENAME_MAX);
}
void Cvars_SetAsFloat(const char* key, float value)
{
	//
	Init();
	//

	CvarData* cvar = GetCvarData(key, STR_NOT_SET);
	SetupCvarForSet(cvar, key);
	Utils_FloatToString(value, cvar->mValue, EE_FILENAME_MAX);
}
void Cvars_Set(const char* key, const char* value)
{
	//
	Init();
	//

	CvarData* cvar = GetCvarData(key, value);
	SetupCvarForSet(cvar, key);
	Utils_strlcpy(cvar->mValue, value, EE_FILENAME_MAX);
}
const char* Cvars_Get(const char* key)
{
	//
	Init();
	//

	return GetCvarData(key, STR_NOT_SET)->mValue;
}
float Cvars_GetAsFloat(const char* key)
{
	//
	Init();
	//

	CvarData* data = GetCvarData(key, STR_ZERO);
	if (data->mDoNotRefreshCachedNumber)
	{
		return data->mCachedNumber;
	}
	else
	{
		data->mCachedNumber = Utils_ParseFloat(data->mValue);
		data->mDoNotRefreshCachedNumber = true;
		return data->mCachedNumber;
	}
}
int32_t Cvars_GetAsInt(const char* key)
{
	//
	Init();
	//

	return (int32_t)(Cvars_GetAsFloat(key));
}
bool Cvars_GetAsBool(const char* key)
{
	//
	Init();
	//

	const char* value = Cvars_Get(key);
	if (Utils_StringEqualTo(value, STR_ZERO))
	{
		return false;
	}
	else
	{
		return true;
	}
}
void Cvars_Read(bool isBinary, BufferReader* br)
{
	//
	Init();
	//

	if (!BufferReader_ReadMagicNumber(br))
	{
		return;
	}

	IStringMap* sm = IStringMap_Create();
	IStringMap_Read(sm, br);
	for (int32_t i = 0; i < IStringMap_Length(sm); i += 1)
	{
		const char* key = IStringMap_GetKeyByIndex(sm, i);
		const char* value = IStringMap_GetValueByIndex(sm, i);
		Cvars_Set(key, value);
	}
	IStringMap_Dispose(sm);
}
int64_t Cvars_Length(void)
{
	//
	Init();
	//

	return shlen(sh_cvars);
}
void Cvars_SaveUserConfig(void)
{
	//
	Init();
	//

	Cvars_SaveUserConfig2(true);
}
void Cvars_SaveUserConfig2(bool showSavingAnimation)
{
	//
	Init();
	//

	if (Cvars_GetAsBool(CVARS_ENGINE_SAVING_SAVES_DISABLED) || Globals_IsSavingUserDataDisabled())
	{
		return;
	}

	{
		FixedByteBuffer* bufferToSave = Cvars_CreateBufferFromUserConfigs();
		Service_SaveBuffer(false, CONTAINER_DISPLAY_NAME, CONTAINER_NAME, USER_CONFIG_FILENAME, bufferToSave);
		FixedByteBuffer_Dispose(bufferToSave);
	}

	if (showSavingAnimation)
	{
		Utils_JustSaved();
		Logger_LogInformation("User config file saved");
	}
}
FixedByteBuffer* Cvars_CreateBufferFromUserConfigs(void)
{
	//
	Init();
	//

	DynamicByteBuffer* dbb = DynamicByteBuffer_Create();
	DynamicByteBuffer_SetIsWritingText(dbb, true);
	IStringArray* prefixes = IStringArray_Create();
	IStringArray_Add(prefixes, CVARS_PREFIX_USER_CONFIG);
	Cvars_Write(true, dbb, prefixes, NULL);
	FixedByteBuffer* fbb = DynamicByteBuffer_ConvertToFixedByteBufferAndDisposeDBB(dbb);
	dbb = NULL;
	IStringArray_Dispose(prefixes);
	return fbb;
}
void Cvars_CreateSaveDirectories(void)
{
	//
	Init();
	//
	// 
	//WILLNOTDO 06262023 PC SPECIFIC AND PROBABLY NOT NEEDED ANYMORE

	/*
	if (GetAsBool(ENGINE_SAVING_SAVES_DISABLED))
	{
	return;
	}
	*/
}
void Cvars_LoadInitialCvars(void)
{
	//
	Init();
	//

	MString* pathToFinalConfigInData = NULL;

	Cvars_Set(CVARS_ENGINE_ORGANIZATION_NAME, GLOBAL_DEF_ORGANIZATION_NAME);

	Cvars_LoadDataDirCvars();

	{
		MString* tempString = NULL;
		MString_Combine2(&tempString, "Version: ", Cvars_Get(CVARS_ENGINE_VERSION));
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}

	//OeUtils::SetTileSize(GetAsInt(ENGINE_TILE_SIZE)); //Not needed, using defines

	File_PathCombine2(&pathToFinalConfigInData, "data", "finalconfig");
	ReadDataCvars2(MString_Text(pathToFinalConfigInData), "Data Final", false);

	const char* pathToFinalConfigInRoot = "finalconfig";
	ReadDataCvars2(pathToFinalConfigInRoot, "Data Final", false);

	CopyToUserDefaults();

	bool isLoadingSaveDataDisabled = Cvars_GetAsBool(CVARS_ENGINE_LOADING_SAVE_DATA_DISABLED) || Globals_IsLoadingUserDataDisabled();
	if (Service_PlatformUsesLocalStorageForSaveData() && !isLoadingSaveDataDisabled) //Load at launch time for local storage platforms like Steam
	{
		Cvars_CreateSaveDirectories();
		if (!Cvars_LoadSaveCvarsFromBlob())
		{
			Cvars_SaveUserConfig2(false);
			Logger_LogInformation("Need to test frame rate because this is the first game launch");
			Globals_SetAsNeedToTestFrameRate();
		}
#ifdef EDITOR_MODE
		//WILLNOTDO 06262023 EDITOR_MODE
		/*
		if (!ReadSaveCvarsFromStorageContainer("editorconfig.ini", "Editor"))
		{
			SaveEditorConfig(false);
		}
		*/
#endif
		_mHasLoadedSaveDataCvars = true;
	}

	if (isLoadingSaveDataDisabled)
	{
		_mHasLoadedSaveDataCvars = true;
	}

	ReadDataCvars2(MString_Text(pathToFinalConfigInData), "Data Final", false); //Override user cvars with these
	ReadDataCvars2(pathToFinalConfigInRoot, "Data Final", false);

	MString_Dispose(&pathToFinalConfigInData);
}
void Cvars_CopyFromUserDefaults(void)
{
	//
	Init();
	//

	for (int32_t i = 0; i < shlen(sh_user_defaults); i += 1)
	{
		Cvars_Set(sh_user_defaults[i].key, sh_user_defaults[i].value.mValue);
	}
}
bool Cvars_LoadSaveCvarsFromBlob(void)
{
	//
	Init();
	//

	BufferRequest request = Service_AskToRetrieveBuffer(false, CONTAINER_DISPLAY_NAME, CONTAINER_NAME, USER_CONFIG_FILENAME);
	if (request.mIsBufferReady)
	{
		_mHasLoadedSaveDataCvars = true;
		if (request.mBuffer == NULL)
		{
			return false;
		}
		else
		{
			//try
			//{
			BufferReader* reader = BufferReader_Create(request.mBuffer);
			BufferReader_SetIsReadingText(reader, true);
			Cvars_Read(true, reader);
			BufferReader_Dispose(reader);
			return true;
			/* }
			catch (...)
			{
				OeLogger::LogError("Unable to read cvar data!");
				return;
			}*/
		}
	}
	return false;
}
bool Cvars_HasLoadedSaveDataCvars(void)
{
	return _mHasLoadedSaveDataCvars;
}
void Cvars_ClearCvars(void)
{
	_mHasInit = false;
	_mHasLoadedSaveDataCvars = false;
	shfree(sh_cvars);
	shfree(sh_user_defaults);

	//
	Init();
	//
}
void Cvars_LoadDataDirCvars(void)
{
	//
	Init();
	//

	{
		MString* dataEngineConfig = NULL;
		File_PathCombine2(&dataEngineConfig, "data", "engineconfig");
		ReadDataCvars(MString_Text(dataEngineConfig), "Data Engine");
		MString_Dispose(&dataEngineConfig);
	}

	{
		MString* dataUserConfig = NULL;
		File_PathCombine2(&dataUserConfig, "data", "userconfig");
		ReadDataCvars(MString_Text(dataUserConfig), "Data User");
		MString_Dispose(&dataUserConfig);
	}

#ifdef EDITOR_MODE
	{
		MString* dataEditorConfig = NULL;
		File_PathCombine2(&dataEditorConfig, "data", "editorconfig");
		ReadDataCvars(MString_Text(dataEditorConfig), "Data Editor");
		MString_Dispose(&dataEditorConfig);
	}
#endif
}
void Cvars_Write(bool isBinary, DynamicByteBuffer* dbb, IStringArray* includePrefixes, IStringArray* excludePrefixes)
{
	//
	Init();
	//

	if (!isBinary)
	{
		Exception_Run("Text writer not implemented for cvars", false);
		return;
	}

	DynamicByteBuffer_WriteMagicNumber(dbb);
	DynamicByteBuffer_WriteNewline(dbb);

	IStringMap* sm = IStringMap_Create();
	for (int32_t i = 0; i < shlen(sh_cvars); i += 1)
	{
		CvarData* cvar = &sh_cvars[i].value;

		bool write = true;
		if (IStringArray_Length(includePrefixes) > 0)
		{
			write = false;
			for (int32_t j = 0; j < IStringArray_Length(includePrefixes); j += 1)
			{
				if (Utils_StringStartsWith(cvar->mKey, IStringArray_Get(includePrefixes, j)))
				{
					write = true;
				}
			}
		}
		if (IStringArray_Length(excludePrefixes) > 0)
		{
			for (int32_t j = 0; j < IStringArray_Length(excludePrefixes); j += 1)
			{
				if (Utils_StringStartsWith(cvar->mKey, IStringArray_Get(excludePrefixes, j)))
				{
					write = false;
				}
			}
		}
		if (write)
		{
			IStringMap_Add(sm, cvar->mKey, cvar->mValue);
		
		}
	}
	IStringMap_Write(sm, dbb);
	IStringMap_Dispose(sm);
}
