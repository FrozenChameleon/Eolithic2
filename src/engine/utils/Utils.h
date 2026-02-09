/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "Macros.h"
#include "../math/Rectangle.h"
#include "../math/Point.h"
#include "../math/Vector2.h"
#include "../render/Color.h"

typedef struct IStringArray IStringArray;
typedef struct MString MString;
typedef struct InputCheck InputCheck;

#define UTILS_ENGLISH_LANGUAGE_CODE "en"
#define UTILS_FRENCH_LANGUAGE_CODE "fr"
#define UTILS_ITALIAN_LANGUAGE_CODE "it"
#define UTILS_GERMAN_LANGUAGE_CODE "de"
#define UTILS_SPANISH_LANGUAGE_CODE "es"

#define UTILS_EXTENSION_BIN ".bin"
#define UTILS_EXTENSION_INI ".ini"

typedef enum UtilsRoundingMode
{
    UTILS_ROUNDING_MODE_FLOOR = 0,
    UTILS_ROUNDING_MODE_CEIL = 1,
    UTILS_ROUNDING_MODE_ROUND = 2
} UtilsRoundingMode;

typedef enum UtilsAllocationArena
{
    UTILS_ALLOCATION_ARENA_INVALID = 0,
    UTILS_ALLOCATION_ARENA_JUST_THIS_FRAME = 1,
    UTILS_ALLOCATION_ARENA_JUST_THIS_LEVEL = 2,
    UTILS_ALLOCATION_ARENA_MOVIE_PLAYER = 3
} UtilsAllocationArena;

const char* Utils_GetCurrentUserLanguageCode(void);
bool Utils_IsCurrentLanguageEnglish(void);
bool Utils_IsCurrentlyUsingEnglishFont(void);
uint64_t Utils_GetMallocRefs(void);
int32_t Utils_memcmp(const void* s1, const void* s2, size_t len);
void Utils_memcpy(void* _Dst, const void* _Src, size_t _Size);
void Utils_memset(void* _Dst, int32_t _Val, size_t _Size);
//void* Utils_realloc(void* mem, size_t size); //UNUSED
//void* Utils_malloc(size_t size); //UNUSED
//void* Utils_MallocArena(size_t size, int32_t allocationArena); //UNUSED
void* Utils_calloc(size_t nmemb, size_t size);
void* Utils_CallocArena(size_t nmemb, size_t size, UtilsAllocationArena allocationArena);
void Utils_FreeArena(UtilsAllocationArena allocationArena);
void Utils_free(void* mem);
//Grow mem to newSize if bigger than oldSize, or if mem is NULL. On growth, returns new zero initialized mem. 0 is not valid for newSize, it will be forced to 1.
void* Utils_grow(void* mem, size_t oldSize, size_t newSize);
size_t Utils_strlen(const char* str);
size_t Utils_strnlen(const char* str, size_t maxlen);
//Returns length of src excluding null terminator after copying.
size_t Utils_strlcpy(char* dst, const char* src, size_t maxlen);
//Returns length of src + dst excluding null terminator after appending.
size_t Utils_strlcat(char* dst, const char* src, size_t maxlen);
bool Utils_ParseBooleanFromChar(const char* str);
float Utils_ParseFloat(const char* str); //(float)strtod
double Utils_ParseDouble(const char* str); //strtod
int32_t Utils_ParseInt(const char* str); //(int32_t)strtol
int64_t Utils_ParseInt64(const char* str); //strtoll
uint32_t Utils_ParseUInt32(const char* str); //strtoul
uint64_t Utils_ParseUInt64(const char* str); //strtoull
bool Utils_ParseBoolean(const char* str);
bool Utils_StringEqualTo(const char* str1, const char* str2);
int32_t Utils_UInt8ToString(uint8_t value, char* buffer, size_t maxlen);
int32_t Utils_UInt16ToString(uint16_t value, char* buffer, size_t maxlen);
int32_t Utils_UInt32ToString(uint32_t value, char* buffer, size_t maxlen);
int32_t Utils_UInt64ToString(uint64_t value, char* buffer, size_t maxlen);
int32_t Utils_Int8ToString(int8_t value, char* buffer, size_t maxlen);
int32_t Utils_Int16ToString(int16_t value, char* buffer, size_t maxlen);
int32_t Utils_IntToString(int32_t value, char* buffer, size_t maxlen);
int32_t Utils_Int64ToString(int64_t value, char* buffer, size_t maxlen);
int32_t Utils_FloatToString(float value, char* buffer, size_t maxlen);
int32_t Utils_DoubleToString(double value, char* buffer, size_t maxlen);
char* Utils_UInt64ToStringGlobalBuffer(uint64_t value);
char* Utils_IntToStringGlobalBuffer(int32_t value);
char* Utils_FloatToStringGlobalBuffer(float value);
char* Utils_DoubleToStringGlobalBuffer(double value);
void Utils_ResetArrayAsBool(bool* values, size_t len, bool valueToSet);
void Utils_ResetArrayAsInt(int32_t* values, size_t len, int32_t valueToSet);
void Utils_ResetArrayAsFloat(float* values, size_t len, float valueToSet);
void Utils_ToggleFullscreenButton(void);
int32_t Utils_StringIndexOfString(const char* findThis, const char* inThis, size_t maxlen);
int32_t Utils_StringIndexOf(char findThis, const char* strInThis, size_t maxlen, bool findLastIndex);
int32_t Utils_StringFirstIndexOf(char findThis, const char* strInThis, size_t maxlen);
int32_t Utils_StringLastIndexOf(char findThis, const char* strInThis, size_t maxlen);
double Utils_GetNormalStepLength(void);
double Utils_GetInterpolated(double delta, float current, float last);
Rectangle Utils_GetInternalRectangle(void);
Rectangle Utils_GetInternalRenderRectangle(void);
int32_t Utils_GetInternalWidth(void);
int32_t Utils_GetInternalHeight(void);
int32_t Utils_GetInternalRenderWidth(void);
int32_t Utils_GetInternalRenderHeight(void);
float Utils_GetCurrentHardwareRatio(void);
float Utils_GetCurrentInternalRatio(void);
void Utils_SplitString(const char* str, size_t maxlen, char delim, IStringArray* addToThis);
char Utils_GetCharFromNumber(int32_t val);
int32_t Utils_Get1DArrayPosFor2DArray(int32_t i, int32_t j, int32_t width);
bool Utils_CharIsDigit(char c);
bool Utils_ArrContainsInt(int32_t* arr_values, int32_t containsThis);
bool Utils_StringContains(const char* str, const char* containsThis);
bool Utils_StringEndsWith(const char* str, const char* endsWithThis);
bool Utils_StringStartsWith(const char* str, const char* startsWithThis);
int32_t Utils_ParseDirection(const char* s);
const char* Utils_GetGlyphType(void);
void Utils_UpdateFramesToTimeDisplay(char* buffer, int32_t val);
float Utils_GetSubseconds(int32_t val);
int32_t Utils_GetMinutes(int32_t val);
int32_t Utils_GetHours(int32_t val);
int32_t Utils_GetSeconds(int32_t val);
bool Utils_CheckSave(bool update);
void Utils_JustSaved(void);
int32_t Utils_GetAmountOfDigits(int32_t n);
void Utils_GetSplitCSV(const char* str, IStringArray* addToHere);
bool Utils_IsStringUnderWidth(const char* str, const char* font, int32_t start, int32_t currentIndex, int32_t width);
int32_t Utils_GetSpotForNewLine(const char* str, const char* font, int32_t width);
void Utils_GetStringWithNewLines(const char* str, const char* font, int32_t width, MString** assignToThis);
const char* Utils_GetExtension(bool isBinary);
bool Utils_IsBinaryForDebugFlag(void);
void Utils_ToggleFullscreen(void);
void Utils_SetFullscreenCvar(bool value);
void Utils_InvokeWindowedMode(void);
void Utils_InvokeFullscreenMode(void);
void Utils_ToggleVsyncButton(void);
Rectangle Utils_GetProposedWindowSize(void);
int32_t Utils_GetWindowSizeMulWidth(void);
int32_t Utils_GetWindowSizeMulHeight(void);
Rectangle* Utils_GetWindowResolutions(bool filterAspectRatio, int32_t* length);
const char* Utils_ConvertFramesToTimeDisplay(int32_t val);
void Utils_DeleteBinding(int32_t player, int32_t index, const char* dataName);
bool Utils_UpdateBinding(int32_t player, int32_t index, const char* dataName, bool isController, bool isBoth);
bool Utils_GetInputCheckForBind(InputCheck* inputCheck, bool isController, bool isBoth, bool blockMouse);
int32_t Utils_GetKeyboardForBind(void);
int32_t Utils_GetMouseButtonForBind(void);
const char* Utils_GetStringFromNumberWithZerosHundreds(int32_t val);
const char* Utils_GetStringFromNumberWithZerosTens(int32_t val);
Color Utils_GetCollisionColor(int32_t val);
int32_t Utils_ConvertFramesToMilliseconds(int32_t val);
void Utils_StringReplaceStr(MString** assignToThis, const char* str, const char* findThis, const char* replaceWithThis);
uint8_t Utils_BoolToUInt8(bool value);
bool Utils_IsStringASCII(const char* str, size_t len);
int32_t Utils_AlignToTileGridInt(int32_t value, UtilsRoundingMode roundingMode, bool returnPixelCoordinates);
Point Utils_AlignToTileGridPoint(Point value, UtilsRoundingMode roundingMode, bool returnPixelCoordinates);
int32_t Utils_AlignToTileGridFloat(float value, UtilsRoundingMode roundingMode, bool returnPixelCoordinates);
Point Utils_AlignToTileGridVector2(Vector2 value, UtilsRoundingMode roundingMode, bool returnPixelCoordinates);
int32_t Utils_AlignToTileGridDouble(double value, UtilsRoundingMode roundingMode, bool returnPixelCoordinates);