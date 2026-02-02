/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "SystemSimple.h"
#include "../components/BounceData.h"

System* SysResetBounceData_CreateSystem(void);
void SysResetBounceData_UpdateRoutine(Entity owner, BounceData* data);