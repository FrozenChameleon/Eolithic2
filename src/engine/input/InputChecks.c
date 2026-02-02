/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "InputChecks.h"

#include "../utils/Exception.h"

InputCheck* InputChecks_Get(InputChecks* data, int32_t index)
{
	if ((index >= 0) && (index < INPUTCHECKS_LENGTH))
	{
		return &data->Checks[index];
	}

	Exception_Run("Out of input checks bounds!", false);
	return NULL;
}

void InputChecks_Set(InputChecks* data, int32_t index, InputCheck check)
{
	if ((index >= 0) && (index < INPUTCHECKS_LENGTH))
	{
		data->Checks[index] = check;
		return;
	}

	Exception_Run("Out of input checks bounds!", false);
}
