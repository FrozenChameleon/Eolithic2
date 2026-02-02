/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"

typedef enum DrawState
{
	OeState_DEFAULT = 0,
	OeState_HEAD = 1,
	OeState_SHOOT = 2,
	OeState_NEW = 3,
	OeState_TAGS = 4,
	OeState_WHEELS = 5,
	OeState_WHEELFX = 6,
	OeState_MUZZLE = 7,
	OeState_STEP1 = 8,
	OeState_STEP2 = 9,
	OeState_STEP3 = 10,
	OeState_STEP4 = 11,
	OeState_STEP5 = 12,
	OeState_STEP6 = 13,
	OeState_STEP7 = 14,
	OeState_STEP8 = 15,
	OeState_STEP9 = 16,
} DrawState;

typedef struct DrawStateMap
{
	char* key;
	int32_t value;
} DrawStateMap;

DrawStateMap* OeState_GetShDrawStateMap(void);

/*
const std::vector<std::string>& OeState_GetDrawStateArray();
std::unordered_map<std::string, int32_t>& OeState_GetDrawStateMap();
const std::string& OeState_GetDefaultState();
void OeState_Init();
*/