/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "FontMap.h"

#include "../utils/Macros.h"
#include "../utils/Utils.h"
#include "../utils/Cvars.h"
#include "../../third_party/stb_ds.h"

static FontMapData* arr_data;
static char _mPath[EE_FILENAME_MAX];
static bool _mHasInit;

static void FillWithEfigs(IStringArray* fillThis)
{
	IStringArray_Add(fillThis, "en");
	IStringArray_Add(fillThis, "fr");
	IStringArray_Add(fillThis, "it");
	IStringArray_Add(fillThis, "de");
	IStringArray_Add(fillThis, "es");
}
static void FillWithFigs(IStringArray* fillThis)
{
	IStringArray_Add(fillThis, "fr");
	IStringArray_Add(fillThis, "it");
	IStringArray_Add(fillThis, "de");
	IStringArray_Add(fillThis, "es");
}

//Normally these are filled in by reading an external file...
#ifdef GLOBAL_DEF_GAME_IS_MUTE_CRIMSON_DX
static void BuildMuteCrimsonFontReplacements(void)
{
	arrsetlen(arr_data, 0);

	//Originals in comments
	{
		FontMapData fontEditor;
		FontMapData_Init(&fontEditor);
		Utils_strlcpy(fontEditor.mNote, "Editor", EE_FILENAME_MAX);
		Utils_strlcpy(fontEditor.mFontName, "AnekLatin-Regular", EE_FILENAME_MAX);
		Utils_strlcpy(fontEditor.mReplace, "editor", EE_FILENAME_MAX);
		fontEditor.mFontSize = 24;
		arrput(arr_data, fontEditor);
	}

	{
		FontMapData fontCardGame1;
		FontMapData_Init(&fontCardGame1);
		Utils_strlcpy(fontCardGame1.mNote, "Card Game 1", EE_FILENAME_MAX);
		Utils_strlcpy(fontCardGame1.mFontName, "Kanit-Italic", EE_FILENAME_MAX);
		Utils_strlcpy(fontCardGame1.mReplace, "cardgame", EE_FILENAME_MAX);
		FillWithEfigs(fontCardGame1.mLanguages);
		fontCardGame1.mFontSize = 52;
		fontCardGame1.mLineSpacing = -20;
		fontCardGame1.mCharacterSpacing = 0;
		fontCardGame1.mOffset.X = 0;
		fontCardGame1.mOffset.Y = 0;
		fontCardGame1.mOverrideBoundsHeight = 32;
		arrput(arr_data, fontCardGame1);
	}

	{
		FontMapData fontCardGame2;
		FontMapData_Init(&fontCardGame2);
		Utils_strlcpy(fontCardGame2.mNote, "Card Game 2", EE_FILENAME_MAX);
		Utils_strlcpy(fontCardGame2.mFontName, "Kanit-BoldItalic", EE_FILENAME_MAX);
		Utils_strlcpy(fontCardGame2.mReplace, "cardgame2", EE_FILENAME_MAX);
		FillWithEfigs(fontCardGame2.mLanguages);
		fontCardGame2.mFontSize = 106;
		fontCardGame2.mLineSpacing = -56;
		fontCardGame2.mCharacterSpacing = 0;
		fontCardGame2.mOffset.X = 0;
		fontCardGame2.mOffset.Y = 0;
		fontCardGame2.mOverrideBoundsHeight = 54;
		arrput(arr_data, fontCardGame2);
	}

	{
		FontMapData fontCardGame3;
		FontMapData_Init(&fontCardGame3);
		Utils_strlcpy(fontCardGame3.mNote, "Card Game 3", EE_FILENAME_MAX);
		Utils_strlcpy(fontCardGame3.mFontName, "Kanit-BoldItalic", EE_FILENAME_MAX);
		Utils_strlcpy(fontCardGame3.mReplace, "cardgame3", EE_FILENAME_MAX);
		FillWithEfigs(fontCardGame3.mLanguages);
		fontCardGame3.mFontSize = 86;
		fontCardGame3.mLineSpacing = -24;
		fontCardGame3.mCharacterSpacing = 0;
		fontCardGame3.mOffset.X = 0;
		fontCardGame3.mOffset.Y = 0;
		arrput(arr_data, fontCardGame3);
	}
}
#endif

void FontMap_Init(void)
{
	if (_mHasInit)
	{
		return;
	}

#ifdef GLOBAL_DEF_GAME_IS_MUTE_CRIMSON_DX
	BuildMuteCrimsonFontReplacements();
#endif

	_mHasInit = true;
}
const FontMapData* FontMap_GetReplacement(const char* fontName)
{
	const char* language = Utils_GetCurrentUserLanguageCode();
	if (Cvars_GetAsBool(CVARS_USER_IS_USING_ALT_FONT))
	{
		if (Utils_StringEqualTo(language, UTILS_ENGLISH_LANGUAGE_CODE))
		{
			language = UTILS_FRENCH_LANGUAGE_CODE;
		}
	}

	for (int32_t i = 0; i < arrlen(arr_data); i += 1)
	{
		FontMapData* data = &arr_data[i];
		if (IStringArray_Length(data->mLanguages) > 0)
		{
			bool contains = IStringArray_Contains(data->mLanguages, language);
			if (contains)
			{
				if ((Utils_StringEqualTo(data->mReplace, EE_STR_EMPTY)) || Utils_StringEqualTo(data->mReplace, fontName))
				{
					return data;
				}
			}
		}
	}

	return NULL;
}