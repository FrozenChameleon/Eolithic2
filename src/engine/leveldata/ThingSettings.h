/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../resources/ResourceManager.h"
#include "ImageData.h"

typedef struct MString MString;
typedef struct BufferReader BufferReader;
typedef struct DynamicByteBuffer DynamicByteBuffer;

enum
{
	THING_SETTINGS_BINARY_VERSION = 3
};

typedef struct ThingGraphicsEntry
{
	char* key; 
	ImageData* value; //THIS IS AN STB ARRAY, SHOULD BE ARR_
} ThingGraphicsEntry;

typedef struct ThingGraphicsData
{
	char* key;
	ThingGraphicsEntry* value; //THIS IS AN STB SHMAP, SHOULD BE SH_
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
	ThingGraphicsData* sh_graphics_data;
} ThingSettings;

void ThingSettings_Init(ThingSettings* ts);

void ThingSettings_Read(ThingSettings* ts, BufferReader* br);
void ThingSettings_Write(ThingSettings* ts, DynamicByteBuffer* dbb);
ThingSettings* ThingSettings_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);
void ThingSettings_Dispose(ThingSettings* ts);