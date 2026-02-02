/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"

typedef struct BufferReader BufferReader;

typedef struct MovieTiming
{
	int32_t len;
	int32_t* timings;
} MovieTiming;

MovieTiming* MovieTiming_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);
void MovieTiming_Dispose(MovieTiming* mt);