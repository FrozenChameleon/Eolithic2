/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "Macros.h"

typedef struct IStringArray IStringArray;

IStringArray* IStringArray_Create(void);
void IStringArray_Clear(IStringArray* sa);
void IStringArray_Dispose(IStringArray* sa);
void IStringArray_Add(IStringArray* sa, const char* str);
bool IStringArray_Contains(IStringArray* sa, const char* containsThis);
int64_t IStringArray_Length(IStringArray* sa);
const char* IStringArray_Get(IStringArray* sa, int32_t index);
uint64_t IStringArray_GetRefs(void);