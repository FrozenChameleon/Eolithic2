/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "CvarNames.h"
#include "stdint.h"
#include "stdbool.h"

typedef struct FixedByteBuffer FixedByteBuffer;
typedef struct DynamicByteBuffer DynamicByteBuffer;
typedef struct IStringArray IStringArray;
typedef struct BufferReader BufferReader;

void Cvars_FlipAsBool(const char* key);
void Cvars_SetAsBool(const char* key, bool value);
void Cvars_SetAsInt(const char* key, int32_t value);
void Cvars_SetAsFloat(const char* key, float value);
void Cvars_Set(const char* key, const char* value);
const char* Cvars_Get(const char* key);
float Cvars_GetAsFloat(const char* key);
int32_t Cvars_GetAsInt(const char* key);
bool Cvars_GetAsBool(const char* key);
void Cvars_Read(bool isBinary, BufferReader* br);
int64_t Cvars_Length(void);
void Cvars_SaveUserConfig(void);
void Cvars_SaveUserConfig2(bool showSavingAnimation);
FixedByteBuffer* Cvars_CreateBufferFromUserConfigs(void);
void Cvars_CreateSaveDirectories(void);
void Cvars_LoadInitialCvars(void);
void Cvars_CopyFromUserDefaults(void);
bool Cvars_LoadSaveCvarsFromBlob(void);
bool Cvars_HasLoadedSaveDataCvars(void);
void Cvars_ClearCvars(void);
void Cvars_LoadDataDirCvars(void);
void Cvars_Write(bool isBinary, DynamicByteBuffer* dbb, IStringArray* includePrefixes, IStringArray* excludePrefixes);