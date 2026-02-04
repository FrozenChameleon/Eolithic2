/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "Macros.h"

typedef struct IStringMap IStringMap;
typedef struct BufferReader BufferReader;
typedef struct DynamicByteBuffer DynamicByteBuffer;

IStringMap* IStringMap_Create(void);
void IStringMap_Clear(IStringMap* sm);
void IStringMap_Dispose(IStringMap* sm);
void IStringMap_Add(IStringMap* sm, const char* key, const char* value);
const char* IStringMap_Get(IStringMap* sm, const char* key);
bool IStringMap_Contains(IStringMap* sm, const char* key);
int64_t IStringMap_Length(IStringMap* sm);
const char* IStringMap_GetKeyByIndex(IStringMap* sm, int32_t index);
const char* IStringMap_GetValueByIndex(IStringMap* sm, int32_t index);
uint64_t IStringMap_GetRefs(void);
void IStringMap_Read(IStringMap* sm, BufferReader* br);
void IStringMap_Write(IStringMap* sm, DynamicByteBuffer* dbb);