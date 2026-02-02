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

static const char* TAG_ID = "id";
static const char* TAG_ROUTINE_ID = "routine_id";
static const char* TAG_COLLISION_WIDTH = "collision_width";
static const char* TAG_COLLISION_HEIGHT = "collision_height";
static const char* TAG_DO_NOT_DISPOSE = "do_not_dispose";
static const char* TAG_HAS_AI = "has_ai";
static const char* TAG_HAS_DRAWING = "has_drawing";
static const char* TAG_HAS_COLLISION = "has_collision";
static const char* TAG_DEFAULT_STATE = "default_state";
static const char* TAG_DEFAULT_PHASE = "default_phase";
static const char* TAG_PREVIEW_SHEET = "preview_sheet";
static const char* TAG_GRAPHICS_STATE_COUNT = "state_count";

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
		MString* key1 = NULL;
		BufferReader_ReadMString(&key1, br);

		ThingGraphicsEntry* entry = NULL;
		sh_new_arena(entry);

		int32_t phaseCount = BufferReader_ReadI32(br);
		for (int32_t j = 0; j < phaseCount; j += 1)
		{
			MString* key2 = NULL;
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

			MString_Dispose(&key2);
		}

		shput(ts->sh_graphics_data, MString_Text(key1), entry);

		MString_Dispose(&key1);
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
