/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../systems/SystemSimple.h"
#include "../components/ShakeCamera.h"

System* SpecialShakeCameraSys_CreateSystem(void);
void SpecialShakeCameraSys_UpdateRoutine(Entity owner, ShakeCamera* data);
void SpecialShakeCameraSys_Setup(ShakeCamera* data, int32_t timerLimit, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY);
void SpecialShakeCameraSys_Create(int32_t timerLimit, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY);