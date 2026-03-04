/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "ThingSettings.h"

#include "../utils/MString.h"
#include "../utils/Utils.h"
#include "../io/File.h"
#include "../../third_party/stb_ds.h"
#include "../io/BufferReader.h"
#include "../io/DynamicByteBuffer.h"
#include "../utils/IStringArray.h"

#define VERSION_THINGSETTINGS 7

void ThingSettings_Init(ThingSettings* ts)
{
	Utils_memset(ts, 0, sizeof(ThingSettings));

	Utils_strlcpy(ts->mDefaultState, "DEFAULT", EE_FILENAME_MAX);
	Utils_strlcpy(ts->mDefaultPhase, "DEFAULT", EE_FILENAME_MAX);
	Utils_strlcpy(ts->mPreviewSheet, EE_STR_NOT_SET, EE_FILENAME_MAX);

	sh_new_arena(ts->sh_graphics_data);
}

void ThingSettings_Read(ThingSettings* ts, BufferReader* br)
{
	if (!BufferReader_ReadMagicNumber(br))
	{
		return;
	}
	BufferReader_ReadVersionNumber(br);

	BufferReader_ReadI32(br); //ID has been removed
	ts->mRoutineId = BufferReader_ReadI32(br);

	ts->mCollisionWidth = BufferReader_ReadI32(br);
	ts->mCollisionHeight = BufferReader_ReadI32(br);
	ts->mDoNotDispose = BufferReader_ReadBoolean(br);
	ts->mHasAI = BufferReader_ReadBoolean(br);
	ts->mHasDrawing = BufferReader_ReadBoolean(br);
	ts->mHasCollision = BufferReader_ReadBoolean(br);
	BufferReader_ReadString(br, ts->mDefaultState, EE_FILENAME_MAX);
	BufferReader_ReadString(br, ts->mDefaultPhase, EE_FILENAME_MAX);
	BufferReader_ReadString(br, ts->mPreviewSheet, EE_FILENAME_MAX);

	int32_t stateCount = BufferReader_ReadI32(br);
	for (int32_t i = 0; i < stateCount; i += 1)
	{
		MString* key1 = MString_CreateForJustThisFrame();
		BufferReader_ReadMString(&key1, br);

		ThingGraphicsEntry* entry = NULL;
		sh_new_arena(entry);

		int32_t phaseCount = BufferReader_ReadI32(br);
		for (int32_t j = 0; j < phaseCount; j += 1)
		{
			MString* key2 = MString_CreateForJustThisFrame();
			BufferReader_ReadMString(&key2, br);

			ImageData* arr_images = NULL;

			int32_t imageCount = BufferReader_ReadI32(br);
			for (int32_t k = 0; k < imageCount; k += 1)
			{
				ImageData data = { 0 };
				ImageData_Init(&data);
				ImageData_Read(&data, br);
				arrput(arr_images, data);
			}

			shput(entry, MString_Text(key2), arr_images);
		}

		shput(ts->sh_graphics_data, MString_Text(key1), entry);
	}
}

void ThingSettings_Write(ThingSettings* ts, DynamicByteBuffer* dbb)
{
	DynamicByteBuffer_WriteMagicNumber(dbb);
	DynamicByteBuffer_WriteVersionNumber(dbb, VERSION_THINGSETTINGS);

	DynamicByteBuffer_WriteI32(dbb, 0); //ID has been removed
	DynamicByteBuffer_WriteI32(dbb,  ts->mRoutineId);
	DynamicByteBuffer_WriteI32(dbb, ts->mCollisionWidth);
	DynamicByteBuffer_WriteI32(dbb, ts->mCollisionHeight);
	DynamicByteBuffer_WriteBoolean(dbb, ts->mDoNotDispose);
	DynamicByteBuffer_WriteBoolean(dbb, ts->mHasAI);
	DynamicByteBuffer_WriteBoolean(dbb, ts->mHasDrawing);
	DynamicByteBuffer_WriteBoolean(dbb, ts->mHasCollision);
	DynamicByteBuffer_WriteString(dbb, ts->mDefaultState, EE_FILENAME_MAX);
	DynamicByteBuffer_WriteString(dbb, ts->mDefaultPhase, EE_FILENAME_MAX);
	DynamicByteBuffer_WriteString(dbb, ts->mPreviewSheet, EE_FILENAME_MAX);

	DynamicByteBuffer_WriteNewline(dbb);

	DynamicByteBuffer_WriteI32(dbb, (int32_t)shlen(ts->sh_graphics_data));
	for (int i = 0; i < shlen(ts->sh_graphics_data); i += 1)
	{
		DynamicByteBuffer_WriteNewline(dbb);
		DynamicByteBuffer_WriteString(dbb, ts->sh_graphics_data[i].key, EE_FILENAME_MAX);
		ThingGraphicsEntry* innerMap = ts->sh_graphics_data[i].value;
		DynamicByteBuffer_WriteI32(dbb, (int32_t)shlen(innerMap));
		for (int j = 0; j < shlen(innerMap); j += 1)
		{
			DynamicByteBuffer_WriteNewline(dbb);
			DynamicByteBuffer_WriteString(dbb, innerMap[j].key, EE_FILENAME_MAX);
			ImageData* arr_image_data = innerMap[j].value;
			DynamicByteBuffer_WriteI32(dbb, (int32_t)arrlen(arr_image_data));
			for (int k = 0; k < arrlen(arr_image_data); k += 1)
			{
				DynamicByteBuffer_WriteNewline(dbb);
				ImageData_Write(&arr_image_data[k], dbb);
			}
		}
	}
}

ThingSettings* ThingSettings_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	ThingSettings* ts = (ThingSettings*)Utils_calloc(1, sizeof(ThingSettings));
	ThingSettings_Init(ts);
	ThingSettings_Read(ts, br);
	return ts;
}
void ThingSettings_Dispose(ThingSettings* ts)
{
	Utils_free(ts);
}
int64_t ThingSettings_GetStateIndex(ThingSettings* ts, const char* state)
{
	return shgeti(ts->sh_graphics_data, state);
}
int64_t ThingSettings_GetPhaseIndex(ThingSettings* ts, const char* state, const char* phase)
{
	int64_t stateIndex = ThingSettings_GetStateIndex(ts, state);
	if (stateIndex < 0)
	{
		return -1;
	}

	ThingGraphicsEntry* sh_entry = ts->sh_graphics_data[stateIndex].value;
	return shgeti(sh_entry, phase);
}
void ThingSettings_AddState(ThingSettings* ts, const char* state)
{
	if (ThingSettings_HasState(ts, state))
	{
		return;
	}

	shput(ts->sh_graphics_data, state, NULL);
}
void ThingSettings_AddPhase(ThingSettings* ts, const char* state, const char* phase)
{
	if (ThingSettings_HasStateAndPhase(ts, state, phase))
	{
		return;
	}

	int64_t stateIndex = ThingSettings_GetStateIndex(ts, state);
	shput(ts->sh_graphics_data[stateIndex].value, phase, NULL);
}
bool ThingSettings_HasState(ThingSettings* ts, const char* state)
{
	int64_t stateIndex = ThingSettings_GetStateIndex(ts, state);
	if (stateIndex < 0)
	{
		return false;
	}
	return true;
}
void ThingSettings_RemoveState(ThingSettings* ts, const char* state)
{
	shdel(ts->sh_graphics_data, state);
}
bool ThingSettings_HasStateAndPhase(ThingSettings* ts, const char* state, const char* phase)
{
	int64_t phaseIndex = ThingSettings_GetPhaseIndex(ts, state, phase);
	if (phaseIndex < 0)
	{
		return false;
	}
	return true;
}
void ThingSettings_RemovePhase(ThingSettings* ts, const char* state, const char* phase)
{
	int64_t stateIndex = ThingSettings_GetStateIndex(ts, state);
	if (stateIndex < 0)
	{
		return;
	}

	shdel(ts->sh_graphics_data[stateIndex].value, phase);
}
void ThingSettings_GetStates(ThingSettings* ts, IStringArray* addToThis)
{
	for (int i = 0; i < shlen(ts->sh_graphics_data); i += 1)
	{
		IStringArray_Add(addToThis, ts->sh_graphics_data[i].key);
	}
}
void ThingSettings_GetPhases(ThingSettings* ts, IStringArray* addToThis, const char* state)
{
	int64_t stateIndex = ThingSettings_GetStateIndex(ts, state);
	if (stateIndex < 0)
	{
		return;
	}

	for (int i = 0; i < shlen(ts->sh_graphics_data[stateIndex].value); i += 1)
	{
		IStringArray_Add(addToThis, ts->sh_graphics_data[stateIndex].value[i].key);
	}
}
void ThingSettings_GetImages(ThingSettings* ts, IStringArray* addToThis, const char* state, const char* phase)
{
	int32_t len = 0;
	ImageData* imageDatas = ThingSettings_GetImageDatas(ts, state, phase, &len);
	for (int i = 0; i < len; i += 1)
	{
		IStringArray_Add(addToThis, imageDatas[i].mImage);
	}
}
int64_t ThingSettings_GetStateLength(ThingSettings* ts)
{
	return shlen(ts->sh_graphics_data);
}
int64_t ThingSettings_GetPhaseLength(ThingSettings* ts, const char* state)
{
	int64_t stateIndex = ThingSettings_GetStateIndex(ts, state);
	if (stateIndex < 0)
	{
		return 0;
	}

	return shlen(ts->sh_graphics_data[stateIndex].value);
}
int64_t ThingSettings_GetImagesLength(ThingSettings* ts, const char* state, const char* phase)
{
	int64_t stateIndex = ThingSettings_GetStateIndex(ts, state);
	if (stateIndex < 0)
	{
		return 0;
	}

	int64_t phaseIndex = ThingSettings_GetPhaseIndex(ts, state, phase);
	if (phaseIndex < 0)
	{
		return 0;
	}

	return arrlen(ts->sh_graphics_data[stateIndex].value[phaseIndex].value);
}
ImageData* ThingSettings_GetImageDatas(ThingSettings* ts, const char* state, const char* phase, int32_t* out_image_datas_len)
{
	*out_image_datas_len = 0;

	int64_t stateIndex = ThingSettings_GetStateIndex(ts, state);
	if (stateIndex < 0)
	{
		return NULL;
	}

	int64_t phaseIndex = ThingSettings_GetPhaseIndex(ts, state, phase);
	if (phaseIndex < 0)
	{
		return NULL;
	}

	ImageData* imageDatas = ts->sh_graphics_data[stateIndex].value[phaseIndex].value;
	*out_image_datas_len = (int32_t)arrlen(imageDatas);
	return imageDatas;
}

void ThingSettings_AddImageData(ThingSettings* ts, const char* state, const char* phase, ImageData data)
{
	int64_t stateIndex = ThingSettings_GetStateIndex(ts, state);
	if (stateIndex < 0)
	{
		return;
	}

	int64_t phaseIndex = ThingSettings_GetPhaseIndex(ts, state, phase);
	if (phaseIndex < 0)
	{
		return;
	}

	arrput(ts->sh_graphics_data[stateIndex].value[phaseIndex].value, data);
}

void ThingSettings_RemoveImageData(ThingSettings* ts, const char* state, const char* phase, int32_t imageIndex)
{
	int64_t stateIndex = ThingSettings_GetStateIndex(ts, state);
	if (stateIndex < 0)
	{
		return;
	}

	int64_t phaseIndex = ThingSettings_GetPhaseIndex(ts, state, phase);
	if (phaseIndex < 0)
	{
		return;
	}

	if (imageIndex < ThingSettings_GetImagesLength(ts, state, phase))
	{
		arrdel(ts->sh_graphics_data[stateIndex].value[phaseIndex].value, imageIndex);
	}
}
