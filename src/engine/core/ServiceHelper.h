/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"

void ServiceHelper_Update(double delta);
bool ServiceHelper_HasPlayerHasLostControllerConnection(void);
int32_t ServiceHelper_GetPlayerThatLostControllerConnection(void);
void ServiceHelper_HandlePlayerLostControllerConnection(void);
bool ServiceHelper_HasLoadedEverything(void);