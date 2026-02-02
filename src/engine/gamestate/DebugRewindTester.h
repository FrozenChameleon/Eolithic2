/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdbool.h"

bool DebugRewindTester_NeedToRewind(void);
void DebugRewindTester_Reset(void);
void DebugRewindTester_Update(void);