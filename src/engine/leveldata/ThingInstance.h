/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Point.h"
#include "../utils/StringPair.h"
#include "../render/Color.h"

typedef struct MString MString;
typedef struct BufferReader BufferReader;
typedef struct DynamicByteBuffer DynamicByteBuffer;
typedef struct ThingSettings ThingSettings;
typedef struct SpriteBatch SpriteBatch;

extern const char* THINGINSTANCE_SETTING_DTN_OFFSET_X;
extern const char* THINGINSTANCE_SETTING_DTN_OFFSET_Y;

extern const char* THINGINSTANCE_SETTING_BLN_DIFFICULTY_EASY;
extern const char* THINGINSTANCE_SETTING_BLN_DIFFICULTY_NORMAL;
extern const char* THINGINSTANCE_SETTING_BLN_DIFFICULTY_HARD;
extern const char* THINGINSTANCE_SETTING_BLN_DIFFICULTY_VERY_HARD;

typedef struct ThingInstance
{
	char mName[EE_FILENAME_MAX];
	Point* arr_nodes;
	StringPair* arr_settings;
} ThingInstance;

void ThingInstance_Init(ThingInstance* ti);

StringPair ThingInstance_GetSetting(ThingInstance* ti, const char* key);
void ThingInstance_SetSetting(ThingInstance* ti, const char* key, const char* value);
void ThingInstance_Write(ThingInstance* ti, DynamicByteBuffer* dbb);
void ThingInstance_Read(int32_t version, ThingInstance* ti, BufferReader* reader);
void ThingInstance_SetupSettings(ThingInstance* instance, bool ignoreWarnings);
void ThingInstance_RemoveUnrelatedPairs(ThingInstance* instance, StringPair* arr_from, StringPair* arr_to, bool ignoreWarnings);
void ThingInstance_AddMissingPairs(StringPair* arr_from, StringPair* arr_to, bool ignoreWarnings);
bool ThingInstance_Contains(StringPair* pair, StringPair* arr_string_pairs);
ThingInstance* ThingInstance_GetClone(ThingInstance* ti);
bool ThingInstance_IsEqualTo(ThingInstance* ti, ThingInstance* instance);
ThingSettings* ThingInstance_GetThingSettings(ThingInstance* ti);
void ThingInstance_Draw(ThingInstance* ti, SpriteBatch* spriteBatch, Color color, Point position, bool isSelected);
void ThingInstance_Draw2(ThingInstance* ti, SpriteBatch* spriteBatch, Color color, int32_t depth, Point position, bool isSelected);
const char* ThingInstance_GetName(ThingInstance* ti);
int32_t ThingInstance_GetLeft(ThingInstance* ti, float positionX);
int32_t ThingInstance_GetTop(ThingInstance* ti, float positionY);
int32_t ThingInstance_GetWidth(ThingInstance* ti);
int32_t ThingInstance_GetHeight(ThingInstance* ti);