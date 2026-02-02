/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "InputCheck.h"

enum
{
	INPUTCHECKS_BEGINNING_OF_CONTROLLER = 0,
	INPUTCHECKS_END_OF_CONTROLLER = 2,
	INPUTCHECKS_BEGINNING_OF_KBM = 3,
	INPUTCHECKS_END_OF_KBM = 5,
	INPUTCHECKS_SECTION_LENGTH = 3,
	INPUTCHECKS_LENGTH = 6
};

typedef struct InputChecks
{
	InputCheck Checks[INPUTCHECKS_LENGTH];
} InputChecks;


InputCheck* InputChecks_Get(InputChecks* data, int32_t index);
void InputChecks_Set(InputChecks* data, int32_t index, InputCheck check);