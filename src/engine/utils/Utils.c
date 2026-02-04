/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Utils.h"

#include "SDL3/SDL.h"
#include "../../third_party/stb_ds.h"
#include "float.h"
#include "../math/Math.h"
#include "Cvars.h"
#include "../render/Renderer.h"
#include "Logger.h"
#include "../input/Input.h"
#include "../service/Service.h"
#include "../input/ControllerState.h"
#include "../input/ControllerStates.h"
#include "../globals/Globals.h"
#include "../utils/IStringArray.h"
#include "../render/DrawTool.h"
#include "../core/Window.h"
#include "../input/InputBindings.h"
#include "../input/Keys.h"
#include "../input/Buttons.h"
#include "../input/MouseState.h"
#include "../input/KeyboardState.h"
#include "MString.h"
#include "inttypes.h"

static Color _mEditorColorDefault = { 255, 255, 255, 127 };
static Color _mEditorColorOne = { 0, 0, 255, 127 };
static Color _mEditorColorTwo = { 255, 0, 0, 127 };
static Color _mEditorColorThree = { 0, 255, 0, 127 };
static Color _mEditorColorFour = { 127, 255, 204, 127 };
static Color _mEditorColorFive = { 255, 127, 0, 127 };
static Color _mEditorColorSix = { 255, 0, 255, 127 };
static Color _mEditorColorSeven = { 127, 127, 127, 127 };
static Color _mEditorColorEight = { 255, 255, 255, 127 };
static bool _mPolled;
static MString* _mTempString;
static uint64_t _mMallocRefs;
static int64_t _mSaveFrames;
static Rectangle* arr_resolutions;

#define MISSING_NUMBER "?"

#define NUMBERS_WITH_ZEROS_HUNDREDS_LEN 1000
#define NUMBERS_WITH_ZEROS_TENS_LEN 100

#define LARGE_CHAR_BUFFER_LEN 8192
static char _mLargeCharBuffer[LARGE_CHAR_BUFFER_LEN];

struct { int32_t key; void** value; }*hm_allocation_arenas;

static void ResetLargeCharBuffer(void)
{
	Utils_memset(_mLargeCharBuffer, 0, LARGE_CHAR_BUFFER_LEN);
}

const char* Utils_GetCurrentUserLanguageCode(void)
{
	return UTILS_ENGLISH_LANGUAGE_CODE;

	//MUTE CRIMSON DX DOES NOT HAVE MULTIPLE LANGUAGES, SO NOT DOING THIS RIGHT NOW...

	/*
#ifdef DEBUG_DEF_FORCED_LANGUAGE

	switch (DEBUG_DEF_FORCED_LANGUAGE)
	{
	case 0:
		return UTILS_ENGLISH_LANGUAGE_CODE;
	case 1:
		return UTILS_FRENCH_LANGUAGE_CODE;
	case 2:
		return UTILS_ITALIAN_LANGUAGE_CODE;
	case 3:
		return UTILS_GERMAN_LANGUAGE_CODE;
	case 4:
		return UTILS_SPANISH_LANGUAGE_CODE;
	}
#endif

	if (!_mHasCachedValidLanguages)
	{
		const std_vector<std_string>& listOfLanguages = OeResourceManagers_StringsTextManager.GetKeyList();
		for (int32_t i = 0; i < listOfLanguages.size(); i += 1)
		{
			_mCachedValidLanguages.push_back(listOfLanguages[i]);
		}
		_mHasCachedValidLanguages = true;
	}

	if (_mCachedValidLanguages.size() <= 1) //No other languages present
	{
		return ENGLISH_LANGUAGE_CODE;
	}

	if (Utils_StringEqualTo(_mCultureLanguage, STR_NOTHING))
	{
		const std_string& platformLanguage = OeService_GetPlatformLanguage();
		if (!Utils_StringEqualTo(platformLanguage, STR_NOTHING))
		{
			_mCultureLanguage = platformLanguage;
		}
		else
		{
			_mCultureLanguage = ENGLISH_LANGUAGE_CODE;
			//WILLNOTDO _mCultureLanguage = CultureInfo.CurrentUICulture.TwoLetterISOLanguageName;
		}
	}

	std_string currentLanguage = OeCvars_Get(OeCvars_USER_LANGUAGE);
	if (currentLanguage == LANGUAGE_SYSTEM)
	{
		currentLanguage = _mCultureLanguage;
	}

	bool isValid = false;
	for (int32_t i = 0; i < _mCachedValidLanguages.size(); i += 1)
	{
		if (_mCachedValidLanguages[i] == currentLanguage)
		{
			isValid = true;
		}
	}

	if (!isValid)
	{
		return ENGLISH_LANGUAGE_CODE;
	}

	return currentLanguage;*/
}

bool Utils_IsCurrentLanguageEnglish(void)
{
	return (Utils_StringEqualTo(Utils_GetCurrentUserLanguageCode(), UTILS_ENGLISH_LANGUAGE_CODE));
}
bool Utils_IsCurrentlyUsingEnglishFont(void)
{
	return true;
}
uint64_t Utils_GetMallocRefs(void)
{
	return _mMallocRefs;
}
int32_t Utils_memcmp(const void* s1, const void* s2, size_t len)
{
	return SDL_memcmp(s1, s2, len);
}
void Utils_memcpy(void* _Dst, const void* _Src, size_t _Size)
{
	SDL_memcpy(_Dst, _Src, _Size);
}
void Utils_memset(void* _Dst, int32_t _Val, size_t _Size)
{
	SDL_memset(_Dst, _Val, _Size);
}
static void AddToAllocationArenas(int32_t allocationArena, void* memoryAllocation)
{
	if (allocationArena == UTILS_ALLOCATION_ARENA_INVALID)
	{
		Logger_LogWarning("Attempted to allocate to invalid allocation arena!");
		return;
	}

	int64_t index = hmgeti(hm_allocation_arenas, allocationArena);
	if (index == -1)
	{
		hmput(hm_allocation_arenas, allocationArena, NULL);
	}

	index = hmgeti(hm_allocation_arenas, allocationArena);
	if (index == -1)
	{
		Logger_LogWarning("Attempted to allocate to get allocation arena!");
		return;
	}

	arrput(hm_allocation_arenas[index].value, memoryAllocation);
}
/*void* Utils_realloc(void* mem, size_t size) //UNUSED
{
	if (mem == NULL)
	{
		_mMallocRefs += 1;
	}
	return SDL_realloc(mem, size);
}*/
/*void* Utils_malloc(size_t size) //UNUSED
{
	_mMallocRefs += 1;
	return SDL_malloc(size);
}*/
/*void* Utils_MallocArena(size_t size, int32_t allocationArena) //UNUSED
{
	void* mallocToReturn = Utils_malloc(size);

	AddToAllocationArenas(allocationArena, mallocToReturn);

	return mallocToReturn;
}*/
void* Utils_calloc(size_t nmemb, size_t size)
{
	_mMallocRefs += 1;
	return SDL_calloc(nmemb, size);
}
void* Utils_CallocArena(size_t nmemb, size_t size, int32_t allocationArena)
{
	void* callocToReturn = Utils_calloc(nmemb, size);

	AddToAllocationArenas(allocationArena, callocToReturn);

	return callocToReturn;
}
void Utils_FreeArena(int32_t allocationArena)
{
	int64_t index = hmgeti(hm_allocation_arenas, allocationArena);
	if (index == -1)
	{
		return;
	}

	int64_t len = arrlen(hm_allocation_arenas[index].value);
	if (len == 0)
	{
		return;
	}

	for (int32_t i = 0; i < len; i += 1)
	{
		Utils_free(hm_allocation_arenas[index].value[i]);
	}
	arrsetlen(hm_allocation_arenas[index].value, 0);
}
void Utils_free(void* mem)
{
	_mMallocRefs -= 1;
	SDL_free(mem);
}
void* Utils_grow(void* mem, size_t oldSize, size_t newSize)
{
	if (newSize == 0) //0 is not a valid size.
	{
		newSize = 1;
	}

	if ((mem != NULL) && (oldSize >= newSize)) //Will never return NULL pointer.
	{
		return mem;
	}

	void* newMemory = Utils_calloc(1, newSize);

	if (mem != NULL) //Copy data if previous exists.
	{
		Utils_memcpy(newMemory, mem, oldSize);
		Utils_free(mem);
	}

	return newMemory;
}
size_t Utils_strlen(const char* str)
{
	return SDL_strlen(str);
}
size_t Utils_strnlen(const char* str, size_t maxlen)
{
	return SDL_strnlen(str, maxlen);
}
size_t Utils_strlcpy(char* dst, const char* src, size_t maxlen)
{
	return SDL_strlcpy(dst, src, maxlen);
}
size_t Utils_strlcat(char* dst, const char* src, size_t maxlen)
{
	return SDL_strlcat(dst, src, maxlen);
}
bool Utils_ParseBooleanFromChar(const char* str)
{
	if ((str[0] == 'T') || (str[0] == 't'))
	{
		return true;
	}
	else
	{
		return false;
	}
}
float Utils_ParseFloat(const char* str)
{
	return (float)Utils_ParseDouble(str);
}
double Utils_ParseDouble(const char* str)
{
	return SDL_strtod(str, NULL);
}
int32_t Utils_ParseInt(const char* str)
{
	return (int32_t)SDL_strtol(str, NULL, 10);
}
int64_t Utils_ParseInt64(const char* str)
{
	return (int64_t)SDL_strtoll(str, NULL, 10);
}
uint32_t Utils_ParseUInt32(const char* str)
{
	return (uint32_t)SDL_strtoul(str, NULL, 10);
}
uint64_t Utils_ParseUInt64(const char* str)
{
	return (uint64_t)SDL_strtoull(str, NULL, 10);
}
bool Utils_ParseBoolean(const char* str)
{
	int32_t value = SDL_atoi(str);
	if (value == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}
bool Utils_StringEqualTo(const char* str1, const char* str2)
{
	if ((str1 == NULL) || (str2 == NULL)) //A NULL pointer is not a valid string!
	{
		return false;
	}

	return (SDL_strcmp(str1, str2) == 0);
}
int32_t Utils_UInt8ToString(uint8_t value, char* buffer, size_t maxlen)
{
	return SDL_snprintf(buffer, maxlen, "%" PRIu8, value);
}
int32_t Utils_UInt16ToString(uint16_t value, char* buffer, size_t maxlen)
{
	return SDL_snprintf(buffer, maxlen, "%" PRIu16, value);
}
int32_t Utils_UInt32ToString(uint32_t value, char* buffer, size_t maxlen)
{
	return SDL_snprintf(buffer, maxlen, "%" PRIu32, value);
}
int32_t Utils_UInt64ToString(uint64_t value, char* buffer, size_t maxlen)
{
	return SDL_snprintf(buffer, maxlen, "%" PRIu64, value);
}
int32_t Utils_Int8ToString(int8_t value, char* buffer, size_t maxlen)
{
	return SDL_snprintf(buffer, maxlen, "%" PRId8, value);
}
int32_t Utils_Int16ToString(int16_t value, char* buffer, size_t maxlen)
{
	return SDL_snprintf(buffer, maxlen, "%" PRId16, value);
}
int32_t Utils_IntToString(int32_t value, char* buffer, size_t maxlen)
{
	return SDL_snprintf(buffer, maxlen, "%" PRId32, value);
}
int32_t Utils_Int64ToString(int64_t value, char* buffer, size_t maxlen)
{
	return SDL_snprintf(buffer, maxlen, "%" PRId64, value);
}
int32_t Utils_FloatToString(float value, char* buffer, size_t maxlen)
{
	return SDL_snprintf(buffer, maxlen, "%.9g", value); //9 FROM FROM FLT_DECIMAL_DIG in float.h
}
int32_t Utils_DoubleToString(double value, char* buffer, size_t maxlen)
{
	return SDL_snprintf(buffer, maxlen, "%.17g", value); //17 FROM DBL_DECIMAL_DIG in float.h
}
char* Utils_UInt64ToStringGlobalBuffer(uint64_t value)
{
	ResetLargeCharBuffer();
	Utils_UInt64ToString(value, _mLargeCharBuffer, LARGE_CHAR_BUFFER_LEN);
	return _mLargeCharBuffer;
}
char* Utils_IntToStringGlobalBuffer(int32_t value)
{
	ResetLargeCharBuffer();
	Utils_IntToString(value, _mLargeCharBuffer, LARGE_CHAR_BUFFER_LEN);
	return _mLargeCharBuffer;
}
char* Utils_FloatToStringGlobalBuffer(float value)
{
	ResetLargeCharBuffer();
	Utils_FloatToString(value, _mLargeCharBuffer, LARGE_CHAR_BUFFER_LEN);
	return _mLargeCharBuffer;
}
char* Utils_DoubleToStringGlobalBuffer(double value)
{
	ResetLargeCharBuffer();
	Utils_DoubleToString(value, _mLargeCharBuffer, LARGE_CHAR_BUFFER_LEN);
	return _mLargeCharBuffer;
}
void Utils_ResetArrayAsBool(bool* values, size_t len, bool valueToSet)
{
	for (int32_t i = 0; i < len; i += 1)
	{
		values[i] = valueToSet;
	}
}
void Utils_ResetArrayAsInt(int32_t* values, size_t len, int32_t valueToSet)
{
	for (int32_t i = 0; i < len; i += 1)
	{
		values[i] = valueToSet;
	}
}
void Utils_ResetArrayAsFloat(float* values, size_t len, float valueToSet)
{
	for (int32_t i = 0; i < len; i += 1)
	{
		values[i] = valueToSet;
	}
}
void Utils_ToggleFullscreenButton(void)
{
	Utils_ToggleFullscreen();

	Utils_SetFullscreenCvar(Window_IsFullscreen());
}
int32_t Utils_StringIndexOfString(const char* findThis, const char* inThis, size_t maxlen)
{
	size_t findThisLen = Utils_strnlen(findThis, maxlen);
	size_t inThisLen = Utils_strnlen(inThis, maxlen);
	if ((findThisLen == 0) || (inThisLen == 0) || (findThisLen > inThisLen)) //Invalid
	{
		return -1;
	}
	else if (findThisLen == inThisLen)
	{
		if (Utils_StringEqualTo(findThis, inThis))
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		int32_t sequenceIndexStart = -1;
		int32_t sequenceCounter = 0;
		for (int32_t i = 0; i < inThisLen; i += 1)
		{
			if (inThis[i] != findThis[sequenceCounter])
			{
				sequenceIndexStart = -1;
				sequenceCounter = 0;
			}
			if (inThis[i] == findThis[sequenceCounter])
			{
				if (sequenceCounter == 0)
				{
					sequenceIndexStart = i;
				}
				sequenceCounter += 1;
				if (sequenceCounter == findThisLen)
				{
					return sequenceIndexStart;
				}
			}
		}
		return -1;
	}
}
int32_t Utils_StringIndexOf(char findThis, const char* strInThis, size_t maxlen, bool findLastIndex)
{
	int32_t len = (int32_t)Utils_strnlen(strInThis, maxlen);
	int32_t loc = -1;
	for (int32_t i = 0; i < len; i += 1)
	{
		if (strInThis[i] == findThis)
		{
			if (!findLastIndex)
			{
				return i;
			}
			else
			{
				loc = i;
			}
		}
	}
	return loc;
}
int32_t Utils_StringFirstIndexOf(char findThis, const char* strInThis, size_t maxlen)
{
	return Utils_StringIndexOf(findThis, strInThis, maxlen, false);
}
int32_t Utils_StringLastIndexOf(char findThis, const char* strInThis, size_t maxlen)
{
	return Utils_StringIndexOf(findThis, strInThis, maxlen, true);
}
double Utils_GetNormalStepLength(void)
{
	return MATH_TICK_60HZ;
}
double Utils_GetInterpolated(double delta, float current, float last)
{
	float diff = current - last;
	double mul = delta / Utils_GetNormalStepLength();
	return last + (diff * mul);
}
Rectangle Utils_GetInternalRectangle(void)
{
	Rectangle rect = { 0, 0, Utils_GetInternalWidth(), Utils_GetInternalHeight() };
	return rect;
}
Rectangle Utils_GetInternalRenderRectangle(void)
{
	Rectangle rect = { 0, 0, Utils_GetInternalRenderWidth(), Utils_GetInternalRenderHeight() };
	return rect;
}
int32_t Utils_GetInternalWidth(void)
{
	return Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_WIDTH);
}
int32_t Utils_GetInternalHeight(void)
{
	return Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_HEIGHT);
}
int32_t Utils_GetInternalRenderWidth(void)
{
	return Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_RENDER_WIDTH);
}
int32_t Utils_GetInternalRenderHeight(void)
{
	return Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_RENDER_HEIGHT);
}
float Utils_GetCurrentHardwareRatio(void)
{
	Rectangle rect = Renderer_GetDrawableSize();
	return (float)(rect.Width) / (float)(rect.Height);
}
float Utils_GetCurrentInternalRatio(void)
{
	return (float)(Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_RENDER_WIDTH)) / (float)(Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_RENDER_HEIGHT));
}
void Utils_SplitString(const char* str, size_t maxlen, char delim, IStringArray* addToThis)
{
	ResetLargeCharBuffer();

	size_t len = Utils_strnlen(str, maxlen);
	int32_t counter = 0;
	for (int32_t i = 0; i < (len + 1); i += 1) //Add +1 to len because...
	{
		if ((str[i] == delim) || (i == len)) //We need to make sure we get the stuff at the end...
		{
			_mLargeCharBuffer[counter] = '\0';
			IStringArray_Add(addToThis, _mLargeCharBuffer);
			Utils_memset(_mLargeCharBuffer, 0, LARGE_CHAR_BUFFER_LEN);
			counter = 0;
		}
		else
		{
			_mLargeCharBuffer[counter] = str[i];
			counter += 1;
		}
	}
}
char Utils_GetCharFromNumber(int32_t val)
{
	if (val == -1)
	{
		return ' ';
	}

	switch (val)
	{
	case 0:
		return '0';
	case 1:
		return '1';
	case 2:
		return '2';
	case 3:
		return '3';
	case 4:
		return '4';
	case 5:
		return '5';
	case 6:
		return '6';
	case 7:
		return '7';
	case 8:
		return '8';
	case 9:
		return '9';
	}
	return '0';
}
int32_t Utils_Get1DArrayPosFor2DArray(int32_t i, int32_t j, int32_t width)
{
	return i + (j * width);
}
bool Utils_CharIsDigit(char c)
{
	switch (c)
	{
	case '1':
		return true;
	case '2':
		return true;
	case '3':
		return true;
	case '4':
		return true;
	case '5':
		return true;
	case '6':
		return true;
	case '7':
		return true;
	case '8':
		return true;
	case '9':
		return true;
	case '0':
		return true;
	}
	return false;
}
bool Utils_ArrContainsInt(int32_t* arr_values, int32_t containsThis)
{
	int64_t len = arrlen(arr_values);
	for (int32_t i = 0; i < len; i += 1)
	{
		if (arr_values[i] == containsThis)
		{
			return true;
		}
	}
	return false;
}
bool Utils_StringContains(const char* str, const char* containsThis)
{
	int32_t len = (int32_t)Utils_strlen(str);
	int32_t indexOf = Utils_StringIndexOfString(containsThis, str, len);
	if (indexOf >= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool Utils_StringEndsWith(const char* str, const char* endsWithThis)
{
	size_t strLen = Utils_strlen(str);
	size_t endsWithThisLen = Utils_strlen(endsWithThis);

	if (strLen < endsWithThisLen)
	{
		return false;
	}

	if (Utils_memcmp(str + (strLen - endsWithThisLen), endsWithThis, endsWithThisLen) == 0)
	{
		return true;
	}
	return false;
}
bool Utils_StringStartsWith(const char* str, const char* startsWithThis)
{
	int32_t strLen = (int32_t)Utils_strlen(str);
	int32_t startsWithThisLen = (int32_t)Utils_strlen(startsWithThis);

	if (strLen < startsWithThisLen)
	{
		return false;
	}

	if (Utils_memcmp(str, startsWithThis, startsWithThisLen) == 0)
	{
		return true;
	}
	return false;
}
int32_t Utils_ParseDirection(const char* s)
{
	int32_t value;
	if (Utils_StringEqualTo(s, "0"))
	{
		value = 0;
	}
	else if (Utils_StringEqualTo(s, "1"))
	{
		value = 1;
	}
	else
	{
		value = -1;
	}
	return value;
}
const char* Utils_GetGlyphType(void)
{
	int32_t controllerType = Cvars_GetAsInt(CVARS_USER_CONTROLLER_TYPE);
	if (Service_PlatformForcesSpecificGlyph())
	{
		const char* strForcedGlyph = ControllerState_PlatformGetForcedSpecificGlyphAsString();
		if (!Utils_StringEqualTo(strForcedGlyph, EE_STR_EMPTY))
		{
			return strForcedGlyph;
		}
		else
		{
			controllerType = Service_PlatformGetForcedSpecificGlyph();
		}
	}

	switch (controllerType)
	{
	case INPUT_CONTROLLER_GLYPH_X:
		return "XONE";
	case INPUT_CONTROLLER_GLYPH_P4:
		return "PS4";
	case INPUT_CONTROLLER_GLYPH_P5:
		return "PS5";
	case INPUT_CONTROLLER_GLYPH_N:
		return "SWITCH";
	}
	return "?";
}
float Utils_GetSubseconds(int32_t val)
{
	return (val % 60) * (100.0f / 60.0f);
}
int32_t Utils_GetMinutes(int32_t val)
{
	int32_t minutes = 0;

	int32_t seconds = val / 60;
	if (seconds >= 60)
	{
		minutes = seconds / 60;
		if (minutes >= 60)
		{
			minutes = minutes % 60;
		}
	}

	return minutes;
}
int32_t Utils_GetHours(int32_t val)
{
	int32_t hours = 0;

	int32_t seconds = val / 60;
	if (seconds >= 60)
	{
		int32_t minutes = seconds / 60;
		if (minutes >= 60)
		{
			hours = minutes / 60;
		}
	}

	return hours;
}
int32_t Utils_GetSeconds(int32_t val)
{
	int32_t seconds = val / 60;

	if (seconds >= 60)
	{
		seconds = seconds % 60;
	}

	return seconds;
}
bool Utils_CheckSave(bool update)
{
	if (_mSaveFrames > 0)
	{
		if (update)
		{
			_mSaveFrames -= 1;
		}
		return true;
	}
	return false;
}
void Utils_JustSaved(void)
{
	if (Globals_IsSavingUserDataDisabled())
	{
		return;
	}

	_mSaveFrames = Cvars_GetAsInt(CVARS_ENGINE_SAVE_ICON_TIME);
}
int32_t Utils_GetAmountOfDigits(int32_t n)
{
	double value = (Math_log10(n) + 1);
	return (int32_t)value;
}
void Utils_GetSplitCSV(const char* str, IStringArray* addToHere)
{
	ResetLargeCharBuffer();
	//std_vector<std_string> strings = {};

	bool startedNewString = true;
	bool isEscaped = false;
	bool checkForNest = false;
	bool isNested = false;
	bool suppressAdd = false;

	size_t strLen = Utils_strlen(str);
	int32_t addCounter = 0;
	for (int32_t i = 0; i < strLen; i += 1)
	{
		suppressAdd = false;

		char currentChar = str[i];

		if (currentChar == '"')
		{
			if (checkForNest)
			{
				isNested = true;
			}
			else if (isNested)
			{
				isNested = false;
			}
			else
			{
				if (isEscaped)
				{
					isEscaped = false;
					suppressAdd = true;
				}
			}

			if (startedNewString)
			{
				isEscaped = true;
				checkForNest = true;
				suppressAdd = true;
			}
		}
		else
		{
			if (startedNewString)
			{
				if (currentChar == ' ')
				{
					suppressAdd = true;
				}
			}

			checkForNest = false;
		}

		startedNewString = false;

		if (currentChar == ',' && !isEscaped)
		{
			IStringArray_Add(addToHere, _mLargeCharBuffer);
			ResetLargeCharBuffer();
			addCounter = 0;
			startedNewString = true;
		}
		else
		{
			if (!suppressAdd)
			{
				_mLargeCharBuffer[addCounter] = currentChar;
				addCounter += 1;
			}
		}
	}

	IStringArray_Add(addToHere, _mLargeCharBuffer);
}
bool Utils_IsStringUnderWidth(const char* str, const char* font, int32_t start, int32_t currentIndex, int32_t width)
{
	MString* testString = NULL;
	MString_AssignSubString(&testString, str, start, currentIndex - start);
	Rectangle bounds = DrawTool_GetBounds(MString_Text(testString), font);
	MString_Dispose(&testString);
	if (bounds.Width < width)
	{
		return true;
	}
	else
	{
		return false;
	}
}
int32_t Utils_GetSpotForNewLine(const char* str, const char* font, int32_t width)
{
	size_t strSize = Utils_strlen(str);

	int32_t start = Utils_StringLastIndexOf('\n', str, strSize);
	if (start == -1)
	{
		start = 0;
	}
	else
	{
		start += 1;
	}

	int32_t last = -1;
	for (int32_t i = start; i < strSize; i += 1)
	{
		if (str[i] != ' ')
		{
			continue;
		}
		if (Utils_IsStringUnderWidth(str, font, start, i, width))
		{
			last = i;
		}
		else
		{
			return last;
		}
	}

	if (!Utils_IsStringUnderWidth(str, font, start, (int32_t)strSize, width))
	{
		return last;
	}

	return -1;
}
void Utils_GetStringWithNewLines(const char* str, const char* font, int32_t width, MString** assignToThis)
{
	MString_AssignString(assignToThis, str);

	if (Utils_StringContains(str, "\n") || Utils_StringContains(str, "\r") || Utils_StringContains(str, "\r\n"))
	{
		return;
	}

	bool isDone = false;
	do
	{
		size_t strLen = Utils_strlen(MString_Text(*assignToThis));
		int32_t loc = Utils_GetSpotForNewLine(MString_Text(*assignToThis), font, width);
		if (loc != -1)
		{
			MString* firstHalf = NULL;
			MString_AssignSubString(&firstHalf, MString_Text(*assignToThis), 0, loc);

			MString* secondHalf = NULL;
			MString_AssignSubString(&secondHalf, MString_Text(*assignToThis), loc + 1, (int32_t)(strLen - loc - 1));

			MString_AssignMString(assignToThis, firstHalf);
			MString_AddAssignString(assignToThis, "\n");
			MString_AddAssignMString(assignToThis, secondHalf);

			MString_Dispose(&firstHalf);
			MString_Dispose(&secondHalf);
		}
		else
		{
			isDone = true;
		}
	} while (!isDone);
}
const char* Utils_GetExtension(bool isBinary)
{
	if (isBinary)
	{
		return UTILS_EXTENSION_BIN;
	}
	else
	{
		return UTILS_EXTENSION_INI;
	}
}
bool Utils_IsBinaryForDebugFlag(void)
{
	if (Globals_IsDebugFileMode())
	{
		return false;
	}
	else
	{
		return true;
	}
}
void Utils_ToggleFullscreen(void)
{
	if (!Window_IsFullscreen())
	{
		Utils_InvokeFullscreenMode();
	}
	else
	{
		Utils_InvokeWindowedMode();
	}
}
void Utils_SetFullscreenCvar(bool value)
{
	Cvars_SetAsBool(CVARS_USER_IS_FULLSCREEN, value);
}
void Utils_InvokeWindowedMode(void)
{
	Utils_SetFullscreenCvar(false);
	Window_UpdateFullscreen();
	Window_SetWindowPositionToCentered();
	Renderer_ApplyChanges();
}
void Utils_InvokeFullscreenMode(void)
{
	Utils_SetFullscreenCvar(true);
	Window_UpdateFullscreen();
	Renderer_ApplyChanges();
}
void Utils_ToggleVsyncButton(void)
{
	Cvars_FlipAsBool(CVARS_USER_IS_VSYNC);

	Renderer_UpdateVsync();
	Renderer_ApplyChanges();
}
Rectangle Utils_GetProposedWindowSize(void)
{
	int32_t windowSizeMul = Cvars_GetAsInt(CVARS_USER_WINDOW_SIZE_MULTIPLE);
	int32_t windowSizeWidth = Cvars_GetAsInt(CVARS_USER_WINDOW_SIZE_WIDTH);
	int32_t windowSizeHeight = Cvars_GetAsInt(CVARS_USER_WINDOW_SIZE_HEIGHT);
	if (windowSizeWidth > 0 && windowSizeHeight > 0)
	{
		return Rectangle_Create(0, 0, windowSizeWidth, windowSizeHeight);
	}
	else
	{
		return Rectangle_Create(0, 0, windowSizeMul * Utils_GetWindowSizeMulWidth(), windowSizeMul * Utils_GetWindowSizeMulHeight());
	}
}
int32_t Utils_GetWindowSizeMulWidth(void)
{
	int32_t width = Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_RENDER_WIDTH);
	int32_t overrideWidth = Cvars_GetAsInt(CVARS_ENGINE_OVERRIDE_INTERNAL_WINDOW_WIDTH);
	if (overrideWidth > 0)
	{
		width = overrideWidth;
	}
	return width;
}
int32_t Utils_GetWindowSizeMulHeight(void)
{
	int32_t height = Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_RENDER_HEIGHT);
	int32_t overrideHeight = Cvars_GetAsInt(CVARS_ENGINE_OVERRIDE_INTERNAL_WINDOW_HEIGHT);
	if (overrideHeight > 0)
	{
		height = overrideHeight;
	}
	return height;
}
static bool CheckResolutionsForDuplicates(const Rectangle* valids, int32_t validsLength, int32_t width, int32_t height)
{
	for (int32_t i = 0; i < validsLength; i += 1)
	{
		const Rectangle* rect = &valids[i];
		if ((rect->Width == width) && (rect->Height == height))
		{
			return false;
		}
	}
	return true;
}
Rectangle* Utils_GetWindowResolutions(bool filterAspectRatio, int32_t* length)
{
	if (arr_resolutions != NULL)
	{
		*length = (int32_t)arrlen(arr_resolutions);
		return arr_resolutions;
	}

	int32_t internalWidth = Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_RENDER_WIDTH);
	int32_t internalHeight = Cvars_GetAsInt(CVARS_ENGINE_INTERNAL_RENDER_HEIGHT);

	Rectangle* arrValids = NULL;
	arrput(arrValids, Rectangle_Create(0, 0, internalWidth, internalHeight));
	if ((internalWidth >= 1280) && (internalHeight >= 720)) //Hack for MCDX
	{
		arrput(arrValids, Rectangle_Create(0, 0, internalWidth /= 2, internalHeight /= 2));
	}

	Rectangle currentBounds = Window_GetDisplayBounds();
	int32_t displayModeBoundsLength = 0;
	Rectangle* displayModeBounds = Window_GetAllDisplayModeBounds(&displayModeBoundsLength);

	float wantedAspectRatio = (16.0f / 9.0f);
	for (int32_t i = 0; i < displayModeBoundsLength; i += 1)
	{
		Rectangle bounds = displayModeBounds[i];
		float aspectRatio = ((float)bounds.Width / (float)bounds.Height);
		if (!filterAspectRatio || (Math_fabsf(wantedAspectRatio - aspectRatio) < .01f))
		{
			int32_t width = bounds.Width;
			int32_t height = bounds.Height;
			if ((width != currentBounds.Width) || (height != currentBounds.Height))
			{
				if (CheckResolutionsForDuplicates(arrValids, (int32_t)arrlen(arrValids), width, height))
				{
					arrput(arrValids, Rectangle_Create(0, 0, width, height));
				}
			}
		}
	}

	int32_t safetyCounter = 0;
	while (arrlen(arrValids) > 0)
	{
		int32_t lowest = INT_MAX;
		int32_t index = -1;
		for (int32_t i = 0; i < arrlen(arrValids); i += 1)
		{
			Rectangle rect = arrValids[i];
			int32_t value = rect.Width * rect.Height;
			if (value < lowest)
			{
				lowest = value;
				index = i;
			}
		}
		arrput(arr_resolutions, arrValids[index]);
		arrdel(arrValids, index);
		safetyCounter += 1;
		if (safetyCounter >= 100000)
		{
			break;
		}
	}

	*length = (int32_t)arrlen(arr_resolutions);
	return arr_resolutions;
}
void ConvertTimeHelper(int32_t time)
{
	if (time < 10)
	{
		MString_AddAssignString(&_mTempString, "0");
		MString_AddAssignString(&_mTempString, Utils_IntToStringGlobalBuffer(time));
	}
	else
	{
		MString_AddAssignString(&_mTempString, Utils_IntToStringGlobalBuffer(time));
	}
}
const char* Utils_ConvertFramesToTimeDisplay(int32_t val)
{
	int32_t subseconds = (int32_t)(Utils_GetSubseconds(val));
	int32_t seconds = Utils_GetSeconds(val);
	int32_t minutes = Utils_GetMinutes(val);
	int32_t hours = Utils_GetHours(val);
	if (hours >= 24)
	{
		hours = 23;
		minutes = 59;
		seconds = 59;
		subseconds = 99;
	}

	MString_AssignString(&_mTempString, "");
	ConvertTimeHelper(hours);
	MString_AddAssignString(&_mTempString, ":");
	ConvertTimeHelper(minutes);
	MString_AddAssignString(&_mTempString, ":");
	ConvertTimeHelper(seconds);
	MString_AddAssignString(&_mTempString, ":");
	ConvertTimeHelper(subseconds);
	return MString_Text(_mTempString);
}
static void BuilderHelper(char* buffer, int32_t index, int32_t val)
{
	if (val < 10)
	{
		buffer[index] = '0';
		buffer[index + 1] = Utils_GetCharFromNumber(val);
	}
	else
	{
		buffer[index] = Utils_GetCharFromNumber(val / 10);
		buffer[index + 1] = Utils_GetCharFromNumber(val % 10);
	}
}
void Utils_UpdateFramesToTimeDisplay(char* buffer, int32_t val)
{
	int32_t subseconds = (int32_t)Utils_GetSubseconds(val);
	int32_t seconds = Utils_GetSeconds(val);
	int32_t minutes = Utils_GetMinutes(val);
	int32_t hours = Utils_GetHours(val);
	if (hours >= 24)
	{
		hours = 23;
		minutes = 59;
		seconds = 59;
		subseconds = 99;
	}

	BuilderHelper(buffer, 0, hours);
	buffer[2] = ':';
	BuilderHelper(buffer, 3, minutes);
	buffer[5] = ':';
	BuilderHelper(buffer, 6, seconds);
	buffer[8] = ':';
	BuilderHelper(buffer, 9, subseconds);
}
void Utils_DeleteBinding(int32_t player, int32_t index, const char* dataName)
{
	InputAction* data = InputBindings_GetActionFromBindings(player, dataName);

	{
		MString* tempString = NULL;
		MString_AssignString(&tempString, "Player #");
		MString_AddAssignInt(&tempString, player);
		MString_AddAssignString(&tempString, " Index #");
		MString_AddAssignInt(&tempString, index);
		MString_AddAssignString(&tempString, " Name: ");
		MString_AddAssignString(&tempString, dataName);
		MString_AddAssignString(&tempString, " has been deleted");
		Logger_LogInformation(MString_Text(tempString));
		MString_Dispose(&tempString);
	}

	InputCheck dummyCheck = { 0 };
	InputChecks_Set(&data->mChecks, index, dummyCheck);
	InputBindings_SaveAllBindings();
}
bool Utils_UpdateBinding(int32_t player, int32_t index, const char* dataName, bool isController, bool isBoth)
{
	InputAction* data = InputBindings_GetActionFromBindings(player, dataName);

	if (Input_IsTildePressed() || Input_IsEscapePressed() || Input_IsDeletePressed() || Input_IsPressedByAnyPlayer(ACTIONLIST_GAME_START)) //Forbidden bindings
	{
		_mPolled = false;
		return true;
	}

	bool inputtedThisFrame = false;
	InputCheck check;
	bool blockMouse = Utils_StringStartsWith(dataName, "GAME_MENU_");
	if (Utils_StringEqualTo(dataName, "GAME_START"))
	{
		blockMouse = true;
	}
	bool isValid = Utils_GetInputCheckForBind(&check, isController, isBoth, blockMouse);

	if (isValid && !_mPolled)
	{
		{
			MString* tempString = NULL;
			MString_AssignString(&tempString, "Player #");
			MString_AddAssignInt(&tempString, player);
			MString_AddAssignString(&tempString, " Index #");
			MString_AddAssignInt(&tempString, index);
			MString_AddAssignString(&tempString, " Name: ");
			MString_AddAssignString(&tempString, dataName);
			MString_AddAssignString(&tempString, " has been set to ");
			MString_AddAssignString(&tempString, InputCheck_GetName(&check));
			Logger_LogInformation(MString_Text(tempString));
			MString_Dispose(&tempString);
		}
		//
		InputChecks_Set(&data->mChecks, index, check);
		inputtedThisFrame = true;
		_mPolled = true;
	}

	if (!inputtedThisFrame && _mPolled)
	{
		InputBindings_SaveAllBindings();
		_mPolled = false;
		return true;
	}

	return false;
}
bool Utils_GetInputCheckForBind(InputCheck* inputCheck, bool isController, bool isBoth, bool blockMouse)
{
	if (!isController || isBoth)
	{
		int32_t keyBind = Utils_GetKeyboardForBind();
		if (keyBind != -1)
		{
			*inputCheck = InputCheck_CreateCheckKey(Keys_GetArray()[keyBind]);
			return true;
		}
		if (!blockMouse)
		{
			int32_t mouseButtonBind = Utils_GetMouseButtonForBind();
			if (mouseButtonBind != -1)
			{
				*inputCheck = InputCheck_CreateCheckMouseButton((MouseButton)mouseButtonBind);
				return true;
			}
		}
	}

	if (isController || isBoth)
	{
		for (int32_t i = 0; i < INPUT_MAXIMUM_PLAYER_COUNT; i += 1)
		{
			ControllerState* controllerState = ControllerStates_GetController(i);

			for (int32_t j = 0; j < AXES_AMOUNT_OF_AXIS; j += 1)
			{
				int32_t releaseDirection = ControllerState_IsAnalogReleased(controllerState, j, 0.5f);
				if (releaseDirection != 0)
				{
					*inputCheck = InputCheck_CreateCheckAxis((Axis)j, releaseDirection);
					return true;
				}
			}

			const Button* buttons = Buttons_GetArray();
			int32_t buttonsLength = Buttons_GetArrayLength();
			for (int32_t j = 0; j < buttonsLength; j += 1)
			{
				Button button = buttons[j];
				if (ControllerState_IsButtonReleased(controllerState, button))
				{
					*inputCheck = InputCheck_CreateCheckButton(button);
					return true;
				}
			}
		}
	}

	Utils_memset(inputCheck, 0, sizeof(InputCheck));
	return false;
}
int32_t Utils_GetKeyboardForBind(void)
{
	int32_t keysLength = Keys_GetArrayLength();
	const Key* keys = Keys_GetArray();
	for (int32_t i = 0; i < keysLength; i += 1) //Keys
	{
		if (KeyboardState_IsKeyReleased(keys[i]))
		{
			return i;
		}
	}
	return -1;
}
int32_t Utils_GetMouseButtonForBind(void)
{
	int32_t len = MOUSEBUTTONS_AMOUNT_OF_MOUSE_BUTTONS;
	for (int32_t i = 0; i < len; i += 1)
	{
		if (MouseState_IsButtonReleased(i))
		{
			return i;
		}
	}
	return -1;
}
const char* Utils_GetStringFromNumberWithZerosHundreds(int32_t val)
{
	if ((val < 0) || (val > (NUMBERS_WITH_ZEROS_HUNDREDS_LEN - 1)))
	{
		return MISSING_NUMBER;
	}
	else
	{
		MString_AssignString(&_mTempString, EE_STR_EMPTY);
		if (val < 10)
		{
			MString_AddAssignInt(&_mTempString, 0);
		}
		if (val < 100)
		{
			MString_AddAssignInt(&_mTempString, 0);
		}
		MString_AddAssignInt(&_mTempString, val);
		return MString_Text(_mTempString);
	}
}
const char* Utils_GetStringFromNumberWithZerosTens(int32_t val)
{
	if ((val < 0) || (val > (NUMBERS_WITH_ZEROS_TENS_LEN - 1)))
	{
		return MISSING_NUMBER;
	}
	else
	{
		MString_AssignString(&_mTempString, EE_STR_EMPTY);
		if (val < 10)
		{
			MString_AddAssignInt(&_mTempString, 0);
		}
		MString_AddAssignInt(&_mTempString, val);
		return MString_Text(_mTempString);
	}
}
Color Utils_GetCollisionColor(int32_t val)
{
	Color color;
	switch (val)
	{
	case 1:
		color = _mEditorColorOne;
		break;
	case 2:
		color = _mEditorColorTwo;
		break;
	case 3:
		color = _mEditorColorThree;
		break;
	case 4:
		color = _mEditorColorFour;
		break;
	case 5:
		color = _mEditorColorFive;
		break;
	case 6:
		color = _mEditorColorSix;
		break;
	case 7:
		color = _mEditorColorSeven;
		break;
	case 8:
		color = _mEditorColorEight;
		break;
	default:
		color = _mEditorColorDefault;
		break;
	}
	return color;
}
int32_t Utils_ConvertFramesToMilliseconds(int32_t val)
{
	if (val < 0)
	{
		return val;
	}

	int32_t subseconds = (int32_t)(Utils_GetSubseconds(val));
	int32_t seconds = Utils_GetSeconds(val);
	int32_t minutes = Utils_GetMinutes(val);
	int32_t hours = Utils_GetHours(val);
	if (hours >= 24)
	{
		hours = 23;
		minutes = 59;
		seconds = 59;
		subseconds = 99;
	}

	int32_t convSubSeconds = subseconds * 10;
	int32_t convSeconds = seconds * 1000;
	int32_t convMinutes = minutes * (1000 * 60);
	int32_t convHours = hours * (1000 * 60 * 60);

	int32_t total = convSubSeconds + convSeconds + convMinutes + convHours;

	return total;
}
void Utils_StringReplaceStr(MString** assignToThis, const char* str, const char* findThis, const char* replaceWithThis)
{
	MString_AssignString(assignToThis, str);

	if (Utils_StringEqualTo(findThis, replaceWithThis)) //INVALID
	{
		return;
	}

	int32_t strLen = (int32_t)Utils_strlen(str);
	int32_t findThisLen = (int32_t)Utils_strnlen(findThis, strLen);

	int safetyCounter = 0;
	while (Utils_StringContains(MString_Text(*assignToThis), findThis))
	{
		if (safetyCounter >= 10000)
		{
			return;
		}
		safetyCounter += 1;

		int indexOf = Utils_StringIndexOfString(findThis, MString_Text(*assignToThis), strLen);
		if (indexOf == -1)
		{
			return;
		}

		MString* firstHalf = NULL;
		MString* secondHalf = NULL;

		MString_AssignSubString(&firstHalf, str, 0, indexOf);
		MString_AssignSubString(&secondHalf, str, indexOf + findThisLen, strLen - (indexOf + findThisLen));

		MString_Combine3(assignToThis, MString_Text(firstHalf), replaceWithThis, MString_Text(secondHalf));

		MString_Dispose(&firstHalf);
		MString_Dispose(&secondHalf);
	}
}
uint8_t Utils_BoolToUInt8(bool value)
{
	if (value)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
bool Utils_IsStringASCII(const char* str, size_t len)
{
	for (int i = 0; i < len; i += 1)
	{
		char c = str[i];
		if ((c < 0) || (c > 127)) //BEGINNING OF ASCII RANGE IS 0, END OF RANGE IS 127
		{
			return false;
		}
	}
	return true;
}
