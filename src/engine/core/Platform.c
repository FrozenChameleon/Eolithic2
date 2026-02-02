/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Platform.h"

#include "../utils/Macros.h"
#include "SDL3/SDL.h"
#include "../utils/Utils.h"

static bool _mHasTriedToGetPreferredLocale;
static char _mPreferredLocale[EE_FILENAME_MAX];

const char* Platform_GetPreferredLocalesEFIGS(void)
{
	if (_mHasTriedToGetPreferredLocale)
	{
		return _mPreferredLocale;
	}

	int32_t locales_count = 0;
	SDL_Locale** locales = SDL_GetPreferredLocales(&locales_count);
	for (int32_t i = 0; i < locales_count; i += 1)
	{
		const char* newLocale = locales[i]->language;
		if (Utils_StringEqualTo(newLocale, UTILS_ENGLISH_LANGUAGE_CODE) ||
			Utils_StringEqualTo(newLocale, UTILS_FRENCH_LANGUAGE_CODE) ||
			Utils_StringEqualTo(newLocale, UTILS_ITALIAN_LANGUAGE_CODE) ||
			Utils_StringEqualTo(newLocale, UTILS_GERMAN_LANGUAGE_CODE) ||
			Utils_StringEqualTo(newLocale, UTILS_SPANISH_LANGUAGE_CODE))
		{
			Utils_strlcpy(_mPreferredLocale, newLocale, EE_FILENAME_MAX);
			break;
		}
	}

	SDL_free(locales);

	_mHasTriedToGetPreferredLocale = true;

	return _mPreferredLocale;
}