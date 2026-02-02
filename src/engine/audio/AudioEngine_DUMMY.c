/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#ifdef AUDIO_DUMMY

#include "AudioEngine.h"

int32_t AudioEngine_Init(void)
{
	return 0;
}
void* AudioEngine_GetContext(void)
{
	return NULL;
}

#endif

typedef int compiler_warning_compliance;