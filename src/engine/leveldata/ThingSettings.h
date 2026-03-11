/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../resources/ResourceMan.h"
#include "ImageData.h"

typedef struct MString MString;
typedef struct BufferReader BufferReader;
typedef struct DynamicByteBuffer DynamicByteBuffer;

enum
{
	THING_SETTINGS_BINARY_VERSION = 3
};

typedef struct ThingGraphicsPhases
{
	char* key; 
	ImageData* value; //THIS IS AN STB ARRAY, SHOULD BE ARR_
} ThingGraphicsEntry;

typedef struct ThingGraphicsStates
{
	char* key;
	ThingGraphicsPhases* value; //THIS IS AN STB SHMAP, SHOULD BE SH_
} ThingGraphicsData;

typedef struct ThingSettings
{
	int32_t mRoutineId;
	int32_t mCollisionWidth;
	int32_t mCollisionHeight;
	bool mDoNotDispose;
	bool mHasAI;
	bool mHasDrawing;
	bool mHasCollision;
	char mDefaultState[EE_FILENAME_MAX];
	char mDefaultPhase[EE_FILENAME_MAX];
	char mPreviewSheet[EE_FILENAME_MAX];
	ThingGraphicsStates* sh_graphics_data;
} ThingSettings;

void ThingSettings_Init(ThingSettings* ts);
ThingSettings* ThingSettings_CreateNew();
void ThingSettings_Read(ThingSettings* ts, BufferReader* br);
void ThingSettings_Write(ThingSettings* ts, DynamicByteBuffer* dbb);
ThingSettings* ThingSettings_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);
void ThingSettings_Dispose(ThingSettings* ts);
int64_t ThingSettings_GetStateIndex(ThingSettings* ts, const char* state);
int64_t ThingSettings_GetPhaseIndex(ThingSettings* ts, const char* state, const char* phase);
void ThingSettings_AddState(ThingSettings* ts, const char* state);
void ThingSettings_AddPhase(ThingSettings* ts, const char* state, const char* phase);
bool ThingSettings_HasState(ThingSettings* ts, const char* state);
void ThingSettings_RemoveState(ThingSettings* ts, const char* state);
bool ThingSettings_HasStateAndPhase(ThingSettings* ts, const char* state, const char* phase);
void ThingSettings_RemovePhase(ThingSettings* ts, const char* state, const char* phase);
void ThingSettings_GetStates(ThingSettings* ts, IStringArray* addToThis);
void ThingSettings_GetPhases(ThingSettings* ts, IStringArray* addToThis, const char* state);
void ThingSettings_GetImages(ThingSettings* ts, IStringArray* addToThis, const char* state, const char* phase);
int64_t ThingSettings_GetStateLength(ThingSettings* ts);
int64_t ThingSettings_GetPhaseLength(ThingSettings* ts, const char* state);
int64_t ThingSettings_GetImagesLength(ThingSettings* ts, const char* state, const char* phase);
ImageData* ThingSettings_GetImageDatas(ThingSettings* ts, const char* state, const char* phase, int32_t* out_image_datas_len);
void ThingSettings_AddImageData(ThingSettings* ts, const char* state, const char* phase, ImageData data);
void ThingSettings_RemoveImageData(ThingSettings* ts, const char* state, const char* phase, int32_t imageIndex);
void ThingSettings_CopyTo(ThingSettings* dst, ThingSettings* src);