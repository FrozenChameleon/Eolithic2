/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"

int32_t ButtonList_GetButton(int32_t index);
const int32_t* ButtonList_GetArray(void);
int32_t ButtonList_GetArrayLength(void);