/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"

typedef struct LeaderboardRequestData
{
	int32_t mScopeType;
	bool mIsAscending;
	bool mIsMilliseconds;
	char mName[EE_FILENAME_MAX];
} LeaderboardRequestData;
