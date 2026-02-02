/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "SystemSimple.h"
#include "../components/FloatyMovementData.h"

System* SysResetFloatyMovementData_CreateSystem(void);
void SysResetFloatyMovementData_UpdateRoutine(Entity owner, FloatyMovementData* data);