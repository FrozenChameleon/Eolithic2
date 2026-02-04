/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#ifndef AUDIO_DUMMY

#include "VolumeData.h"

#include "../io/File.h"
#include "../utils/Utils.h"
#include "../utils/IStringMap.h"
#include "../io/BufferReader.h"
#include "../io/DynamicByteBuffer.h"
#include "../../third_party/stb_ds.h"

void VolumeData_Init(VolumeData* vd)
{
	Utils_memset(vd, 0, sizeof(VolumeData));

	sh_new_arena(vd->sh_volume_map);
}

int32_t VolumeData_GetVolume(VolumeData* vd, const char* name)
{
	int64_t loc = shgeti(vd->sh_volume_map, name);
	if (loc < 0)
	{
		return 100;
	}
	else
	{
		return vd->sh_volume_map[loc].value;
	}
}
/*void VolumeData_SetVolume(VolumeData* vd, const char* name, int32_t volume) //UNUSED
{
	shput(vd->sh_volume_map, name, volume);
}*/

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
void VolumeData_Load(VolumeData* vd, bool isMusic)
{
	MString* path = NULL;
	PathHelper(&path, isMusic);

	if (File_Exists(MString_Text(path)))
	{
		BufferReader* br = BufferReader_CreateFromPath(MString_Text(path));
		BufferReader_SetIsReadingText(br, true);
		int32_t count = BufferReader_ReadI32(br);
		for (int32_t i = 0; i < count; i += 1)
		{
			MString* key = NULL;
			BufferReader_ReadMString(&key, br);
			int32_t volume = BufferReader_ReadI32(br);
			shput(vd->sh_volume_map, MString_Text(key), volume);
		}
		BufferReader_Dispose(br);
	}

	MString_Dispose(&path);
}
void VolumeData_Save(VolumeData* vd, bool isMusic)
{
	DynamicByteBuffer* dbb = DynamicByteBuffer_Create();
	DynamicByteBuffer_SetIsWritingText(dbb, true);
	DynamicByteBuffer_WriteMagicNumber(dbb);
	DynamicByteBuffer_WriteNewline(dbb);
	DynamicByteBuffer_WriteI32(dbb, (int32_t)shlen(vd->sh_volume_map));
	for (int32_t i = 0; i < shlen(vd->sh_volume_map); i += 1)
	{
		DynamicByteBuffer_WriteNewline(dbb);
		const char* name = vd->sh_volume_map[i].key;
		int32_t volume = vd->sh_volume_map[i].value;
		DynamicByteBuffer_WriteString(dbb, name, Utils_strlen(name));
		DynamicByteBuffer_WriteI32(dbb, volume);
	}

	FixedByteBuffer* fbb = DynamicByteBuffer_ConvertToFixedByteBufferAndDisposeDBB(dbb);
	MString* path = NULL;
	PathHelper(&path, isMusic);
	File_WriteAll(MString_Text(path), fbb);
	FixedByteBuffer_Dispose(fbb);
	MString_Dispose(&path);
}

#endif

typedef int compiler_warning_compliance;