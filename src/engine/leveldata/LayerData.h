/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"

typedef struct BufferWriter BufferWriter;
typedef struct BufferReader BufferReader;

typedef struct LayerData
{
	float mSpeedX;
	float mSpeedY;
	int32_t mDepth;
} LayerData;

void LayerData_ReadIni(LayerData* layer, int32_t i, BufferReader* reader);