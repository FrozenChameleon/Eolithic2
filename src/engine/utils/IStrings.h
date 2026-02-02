/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"

typedef struct IStrings IStrings;

IStrings* IStrings_Create(void);
void IStrings_Dispose(IStrings* is);
void IStrings_Clear(IStrings* is);
const char* IStrings_Get(IStrings* is, const char* str);
size_t IStrings_Length(IStrings* is);
const char* IStrings_GetByIndex(IStrings* is, int32_t index);
uint64_t IStrings_GetRefs(void);