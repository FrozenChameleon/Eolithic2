#pragma once

#include "stdint.h"
#include "stdbool.h"

int32_t VolumeTool_GetVolume(const char* name, bool isMusic);
void VolumeTool_SetVolume(const char* name, int32_t volume, bool isMusic);
void VolumeTool_Load();
void VolumeTool_Save(bool isMusic);
float VolumeTool_GetFinalVolumeHelper(const char* cvar, const char* name, bool isMusic);