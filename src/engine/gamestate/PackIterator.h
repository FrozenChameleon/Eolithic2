/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"

typedef struct PackIterator
{
	Entity mEntity;
	void* mComponent;
	int32_t mIndex;
	int32_t mSeekPosition;
} PackIterator;

extern const PackIterator PackIterator_Begin;