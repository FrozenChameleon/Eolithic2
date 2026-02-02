/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "Body.h"
#include "../systems/SystemSimple.h"

void BodySys_InitRoutine(Entity owner, Body* data);
System* BodySys_CreateSystem(void);