/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdbool.h"

void Strings_Init(void);
const char* Strings_Get(const char* key);
void Strings_Add(const char* key, const char* val);
void Strings_AddDoNotReplace(const char* key, const char* val, bool doNotReplace);
bool Strings_Contains(const char* key);