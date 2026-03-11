#include "VolumeTool.h"

#include "../io/File.h"
#include "../utils/Utils.h"
#include "../utils/IStringMap.h"
#include "../io/BufferReader.h"
#include "../io/DynamicByteBuffer.h"
#include "../../third_party/stb_ds.h"
#include "../globals/Globals.h"
#include "../utils/Cvars.h"
#include "../gamestate/GameStateManager.h"
#include "../gamestate/GameState.h"
#include "../utils/Logger.h"

typedef struct VolumeData
{
	char* key; 
	int32_t value;
} VolumeData;

static VolumeData* sh_music_volume;
static VolumeData* sh_sfx_volume;

static bool _mHasInit;
static void Init()
{
	if (_mHasInit)
	{
		return;
	}

	sh_new_arena(sh_music_volume);
	sh_new_arena(sh_sfx_volume);

	_mHasInit = true;
}
static const char* GetVolumeTypeString(bool isMusic)
{
	if (isMusic)
	{
		return "music";
	}
	else
	{
		return "sfx";
	}
}
static void PathHelper(MString** assignPathToThis, bool isMusic)
{
	if (isMusic)
	{
		File_PathCombine2(assignPathToThis, "data", "musicvolume.txt");
	}
	else
	{
		File_PathCombine2(assignPathToThis, "data", "sfxvolume.txt");
	}
}
static void PrintLoadFailure(const char* strVolumeType)
{
	MString* temp = MString_CreateForJustThisFrame();
	MString_Combine2(&temp, "Failed to load volume data for ", strVolumeType);
	Logger_Log(LOGGER_WARNING, MString_Text(temp));
}

int32_t VolumeTool_GetVolume(const char* name, bool isMusic)
{
	Init();

	const VolumeData* vd = isMusic ? sh_music_volume : sh_sfx_volume;
	int64_t loc = shgeti(vd, name);
	if (loc < 0)
	{
		return 100;
	}
	return vd[loc].value;
}
void VolumeTool_SetVolume(const char* name, int32_t volume, bool isMusic)
{
	Init();

	if (isMusic)
	{
		shput(sh_music_volume, name, volume);
	}
	else
	{
		shput(sh_sfx_volume, name, volume);
	}
}
static void LoadHelper(bool isMusic)
{
	const char* strVolumeType = GetVolumeTypeString(isMusic);

	MString* path = MString_CreateForJustThisFrame();
	PathHelper(&path, isMusic);

	if (!File_Exists(MString_Text(path)))
	{
		PrintLoadFailure(strVolumeType);
		return;
	}

	BufferReader* br = BufferReader_CreateFromPath(MString_Text(path));
	BufferReader_SetIsReadingText(br, true);
	if (!BufferReader_ReadMagicNumber(br))
	{
		PrintLoadFailure(strVolumeType);
		return;
	}
	int32_t count = BufferReader_ReadI32(br);
	for (int32_t i = 0; i < count; i += 1)
	{
		MString* key = NULL;
		BufferReader_ReadMString(&key, br);
		int32_t volume = BufferReader_ReadI32(br);
		VolumeTool_SetVolume(MString_Text(key), volume, isMusic);
	}
	BufferReader_Dispose(br);

	MString* tempString = MString_CreateForJustThisFrame();
	MString_Combine2(&tempString, "Loaded volume data for ", strVolumeType);
	Logger_Log(LOGGER_WARNING, MString_Text(tempString));
}
void VolumeTool_Load()
{
	Init();

	LoadHelper(true);
	LoadHelper(false);
}
void VolumeTool_Save(bool isMusic)
{
	Init();

	const VolumeData* vd = isMusic ? sh_music_volume : sh_sfx_volume;

	DynamicByteBuffer* dbb = DynamicByteBuffer_Create();
	DynamicByteBuffer_SetIsWritingText(dbb, true);
	DynamicByteBuffer_WriteMagicNumber(dbb);
	DynamicByteBuffer_WriteNewline(dbb);
	DynamicByteBuffer_WriteI32(dbb, (int32_t)shlen(vd));
	for (int32_t i = 0; i < shlen(vd); i += 1)
	{
		DynamicByteBuffer_WriteNewline(dbb);
		const char* name = vd[i].key;
		int32_t volume = vd[i].value;
		DynamicByteBuffer_WriteString(dbb, name, Utils_strlen(name));
		DynamicByteBuffer_WriteI32(dbb, volume);
	}

	FixedByteBuffer* fbb = DynamicByteBuffer_ConvertToFixedByteBufferAndDisposeDBB(dbb);
	MString* path = MString_CreateForJustThisFrame();
	PathHelper(&path, isMusic);
	File_WriteAll(MString_Text(path), fbb);
	FixedByteBuffer_Dispose(fbb);
}
float VolumeTool_GetFinalVolumeHelper(const char* cvar, const char* name, bool isMusic)
{
	Init();

	if (Globals_IsAudioDisabledPermanently() || (name == NULL) || (Utils_IsStringEmpty(name)))
	{
		return 0;
	}

	float userVolume = Cvars_GetAsInt(cvar) / 100.0f;
	float userVolumeMaster = Cvars_GetAsInt(CVARS_USER_VOLUME_MASTER) / 100.0f;
	float clipVolume = VolumeTool_GetVolume(name, isMusic) / 100.0f;
	float rewindingVolume;
	if (GameState_IsRewinding(GameStateManager_GetGameState()))
	{
		rewindingVolume = 0.6f;
	}
	else
	{
		rewindingVolume = 1.0f;
	}
	return clipVolume * userVolume * userVolumeMaster * rewindingVolume;
}
