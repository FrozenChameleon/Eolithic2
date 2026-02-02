/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "System.h"

void GlobalSysDrawFPS_SetOffsetFromCorner(int32_t offset);
void GlobalSysDrawFPS_SetFont(const char* font);
void GlobalSysDrawFPS_DoNotDrawThisRightNow(void);
System* GlobalSysDrawFPS_CreateSystem(void);