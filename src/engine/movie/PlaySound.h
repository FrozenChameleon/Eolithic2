/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"

typedef struct PlaySound
{
	bool mIsLoaded;
	int32_t mPoint;
	char mSound[EE_FILENAME_MAX];
} PlaySound;

void PlaySound_PlayTheSoundIfAtPoint(const PlaySound* ps, int32_t value);