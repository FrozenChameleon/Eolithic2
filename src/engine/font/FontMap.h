/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "FontMapData.h"

void FontMap_Init(void);
const FontMapData* FontMap_GetReplacement(const char* fontName);