/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"

#ifdef AUDIO_DUMMY
typedef struct VolumeData VolumeData;
#else
typedef struct VolumeData
{
	struct { char* key; int32_t value; }* sh_volume_map;
} VolumeData;
#endif

void VolumeData_Init(VolumeData* vd);
int32_t VolumeData_GetVolume(VolumeData* vd, const char* name);
//void VolumeData_SetVolume(VolumeData* vd, const char* name, int32_t volume); //UNUSED
void VolumeData_Load(VolumeData* vd, bool isMusic);
void VolumeData_Save(VolumeData* vd, bool isMusic);