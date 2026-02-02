/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../systems/SystemSimple.h"
#include "HitFlashKit.h"

void HitFlashKitSys_InitRoutine(Entity owner, HitFlashKit* data);
void HitFlashKitSys_UpdateRoutine(Entity owner, HitFlashKit* data);

System* HitFlashKitSys_CreateSystem(void);