/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "MString.h"

#include "Macros.h"
#include "Utils.h"
#include "Logger.h"
#include "../io/BufferReader.h"
#include "../../third_party/stb_ds.h"

//#define FIND_THE_LEAKS

typedef struct MString
{
	char* text;
	int32_t len;
	int32_t capacity;
} MString;

static char _mEmptyString[1] = "";
static uint64_t _mRefs;

#define STRING_BUFFER_LEN EE_PATH_MAX
#define NUMBER_BUFFER_LEN EE_SAFE_BUFFER_LEN_FOR_DOUBLE

static char _mStringBuffer[STRING_BUFFER_LEN];
static char _mNumberBuffer[NUMBER_BUFFER_LEN];

#ifdef FIND_THE_LEAKS
typedef struct LeakTest
{ 
	MString* key; 
	int32_t value;
} LeakTest;

static LeakTest* _mLeakTest;
static bool _mHasLeakTestBegun;
#endif

static void ClearNumberBuffer(void)
{
	Utils_memset(_mNumberBuffer, 0, NUMBER_BUFFER_LEN * sizeof(char));
}
static void ClearStringBuffer(void)
{
	Utils_memset(_mStringBuffer, 0, STRING_BUFFER_LEN * sizeof(char));
}

//Returns length of src excluding null terminator after copying.
static size_t ClearStringBufferAndThenCopyToIt(const char* copyThis)
{
	ClearStringBuffer();
	return Utils_strlcpy(_mStringBuffer, copyThis, STRING_BUFFER_LEN);
}

static MString* MString_CreateEmpty(int32_t capacity)
{
	if (capacity <= 0)
	{
		capacity = 1;
	}

	_mRefs += 1;
	MString* m_string = (MString*)Utils_calloc(1, sizeof(MString));
#ifdef FIND_THE_LEAKS
	if (_mHasLeakTestBegun)
	{
		hmput(_mLeakTest, mstring, 0);
	}
#endif
	m_string->text = (char*)Utils_calloc(1, capacity);
	m_string->len = 0;
	m_string->capacity = capacity;
	return m_string;
}
static bool CheckDoublePointerSafetyForComparison(MString** str)
{
	if (str == NULL)
	{
		Logger_LogWarning("MString double pointer is not safe...");
		return false;
	}

	if (*str == NULL)
	{
		return false;
	}

	return true;
}
static void CheckAndReplaceNullString(MString** str)
{
	if (str == NULL)
	{
		Logger_LogWarning("Handed NULL MString Double Pointer...");
		return;
	}

	if (*str != NULL)
	{
		return;
	}

	*str = MString_CreateEmpty(1);
}
static void GrowMStringIfNeeded(MString* str, size_t newCapacity)
{
	if (str == NULL)
	{
		Logger_LogWarning("Cannot grow NULL MString!");
		return;
	}

	if (newCapacity == 0) //Cannot ever be a capacity of 0.
	{
		newCapacity = 1;
	}

	if (str->capacity >= newCapacity)
	{
		return;
	}

	str->text = (char*)Utils_grow(str->text, str->capacity, newCapacity);

	str->capacity = (int32_t)newCapacity;
}
static void ClearMString(MString* str)
{
	if (str == NULL)
	{
		Logger_LogWarning("Cannot clear NULL MString!");
		return;
	}

	Utils_memset(str->text, 0, str->capacity);
	str->len = 0;
}

uint64_t MString_GetRefs(void)
{
	return _mRefs;
}
char* MString_Text(const MString* str)
{
	if (str == NULL)
	{
		return _mEmptyString;
	}

	return str->text;
}
char MString_GetLastChar(const MString* str)
{
	if ((str == NULL) || (str->len <= 0))
	{
		return '\0';
	}

	return str->text[str->len - 1];
}
int32_t MString_Length(const MString* str)
{
	if (str == NULL)
	{
		return 0;
	}

	return str->len;
}
int32_t MString_Capacity(const MString* str)
{
	if (str == NULL)
	{
		return 0;
	}

	return str->capacity;
}
bool MString_IsEmpty(const MString* str)
{
	return (str->len <= 0);
}
bool MString_EqualToString(const MString* str, const char* otherStr)
{
	return Utils_StringEqualTo(MString_Text(str), otherStr);
}
bool MString_EqualTo(const MString* str, const MString* otherStr)
{
	return Utils_StringEqualTo(MString_Text(str), MString_Text(otherStr));
}
void MString_AssignEmptyString(MString** str)
{
	MString_AssignString(str, "");
}
void MString_AssignString(MString** str, const char* toThis)
{
	//
	CheckAndReplaceNullString(str);
	//

	MString* currentString = *str;
	size_t newLen = ClearStringBufferAndThenCopyToIt(toThis);
	size_t newCapacity = newLen + 1;
	ClearMString(currentString);
	GrowMStringIfNeeded(currentString, newCapacity);
	currentString->len = (int32_t)Utils_strlcpy(currentString->text, _mStringBuffer, currentString->capacity);
}
void MString_AssignMString(MString** str, MString* toThis)
{
	MString_AssignString(str, MString_Text(toThis));
}
void MString_Clear(MString** str)
{
	MString_AssignString(str, EE_STR_EMPTY);
}
void MString_AssignEmpty(MString** str, int32_t capacity)
{
	CheckAndReplaceNullString(str);

	MString* currentString = *str;
	ClearMString(currentString);
	GrowMStringIfNeeded(currentString, capacity);
}
void MString_AssignSubString(MString** str, const char* fromThis, int32_t startIndex, int32_t length)
{
	CheckAndReplaceNullString(str);

	if ((length + 1) > STRING_BUFFER_LEN)
	{
		Logger_LogWarning("SubString is too large for MString!");
		return;
	}

	MString* currentString = *str;
	ClearStringBuffer(); //Manually copying to string buffer.
	size_t newLen = Utils_strlcpy(_mStringBuffer, fromThis + startIndex, length + 1); //
	size_t newCapacity = newLen + 1;
	ClearMString(currentString);
	GrowMStringIfNeeded(currentString, newCapacity);
	currentString->len = (int32_t)Utils_strlcpy(currentString->text, _mStringBuffer, currentString->capacity);
}
void MString_AddAssignBool(MString** str, bool addThisBool)
{
	MString_AddAssignInt(str, addThisBool ? 1 : 0);
}
void MString_AddAssignInt8(MString** str, int8_t addThisInt8)
{
	ClearNumberBuffer();
	Utils_Int8ToString(addThisInt8, _mNumberBuffer, NUMBER_BUFFER_LEN);
	MString_AddAssignString(str, _mNumberBuffer);
}
void MString_AddAssignInt16(MString** str, int16_t addThisInt16)
{
	ClearNumberBuffer();
	Utils_Int16ToString(addThisInt16, _mNumberBuffer, NUMBER_BUFFER_LEN);
	MString_AddAssignString(str, _mNumberBuffer);
}
void MString_AddAssignInt(MString** str, int32_t addThisInt)
{
	ClearNumberBuffer();
	Utils_IntToString(addThisInt, _mNumberBuffer, NUMBER_BUFFER_LEN);
	MString_AddAssignString(str, _mNumberBuffer);
}
void MString_AddAssignInt64(MString** str, int64_t addThisInt64)
{
	ClearNumberBuffer();
	Utils_Int64ToString(addThisInt64, _mNumberBuffer, NUMBER_BUFFER_LEN);
	MString_AddAssignString(str, _mNumberBuffer);
}
void MString_AddAssignUInt8(MString** str, uint8_t addThisUInt8)
{
	ClearNumberBuffer();
	Utils_UInt8ToString(addThisUInt8, _mNumberBuffer, NUMBER_BUFFER_LEN);
	MString_AddAssignString(str, _mNumberBuffer);
}
void MString_AddAssignUInt16(MString** str, uint16_t addThisUInt16)
{
	ClearNumberBuffer();
	Utils_UInt16ToString(addThisUInt16, _mNumberBuffer, NUMBER_BUFFER_LEN);
	MString_AddAssignString(str, _mNumberBuffer);
}
void MString_AddAssignUInt32(MString** str, uint32_t addThisUInt32)
{
	ClearNumberBuffer();
	Utils_UInt32ToString(addThisUInt32, _mNumberBuffer, NUMBER_BUFFER_LEN);
	MString_AddAssignString(str, _mNumberBuffer);
}
void MString_AddAssignUInt64(MString** str, uint64_t addThisUInt64)
{
	ClearNumberBuffer();
	Utils_UInt64ToString(addThisUInt64, _mNumberBuffer, NUMBER_BUFFER_LEN);
	MString_AddAssignString(str, _mNumberBuffer);
}
void MString_AddAssignFloat(MString** str, float addThisFloat)
{
	ClearNumberBuffer();
	Utils_FloatToString(addThisFloat, _mNumberBuffer, NUMBER_BUFFER_LEN);
	MString_AddAssignString(str, _mNumberBuffer);
}
void MString_AddAssignDouble(MString** str, double addThisDouble)
{
	ClearNumberBuffer();
	Utils_DoubleToString(addThisDouble, _mNumberBuffer, NUMBER_BUFFER_LEN);
	MString_AddAssignString(str, _mNumberBuffer);
}
void MString_AddAssignChar(MString** str, char addThisChar)
{
	CheckAndReplaceNullString(str);

	char tempString[2];
	tempString[0] = addThisChar;
	tempString[1] = '\0';
	MString_AddAssignString(str, tempString);
}
void MString_AddAssignMString(MString** str, const MString* addThisStr)
{
	MString_AddAssignString(str, MString_Text(addThisStr));
}
void MString_AddAssignString(MString** str, const char* addThisStr)
{
	CheckAndReplaceNullString(str);

	MString* currentString = *str;
	size_t addThisStrLen = ClearStringBufferAndThenCopyToIt(addThisStr);
	size_t newLen = currentString->len + addThisStrLen;
	size_t newCapacity = newLen + 1;
	GrowMStringIfNeeded(currentString, newCapacity);
	currentString->len = (int32_t)Utils_strlcat(currentString->text, _mStringBuffer, newCapacity);
}
void MString_Truncate(MString** str, int32_t newLength)
{
	CheckAndReplaceNullString(str);

	MString* actualStr = *str;

	if (newLength >= actualStr->len)
	{
		return;
	}

	for (int32_t i = 0; i < actualStr->capacity; i += 1)
	{
		if (i >= newLength)
		{
			actualStr->text[i] = '\0';
		}
	}
	actualStr->len = newLength;
}
void MString_Dispose(MString** str)
{
	if (!CheckDoublePointerSafetyForComparison(str))
	{
		return;
	}

#ifdef FIND_THE_LEAKS
	if (_mHasLeakTestBegun)
	{
		hmdel(_mLeakTest, *str);
	}
#endif

	_mRefs -= 1;

	Utils_free((*str)->text);
	Utils_free((*str));
	*str = NULL;
}
void MString_Read(MString** str, BufferReader* br)
{
	uint8_t newStringLength = BufferReader_ReadJustTheStringLength(br);
	int32_t newStrCapacity = newStringLength + 1;
	MString_AssignEmpty(str, newStrCapacity);
	(*str)->len = newStringLength;
	BufferReader_ReadJustTheStringData(br, newStringLength, MString_Text(*str), newStrCapacity);
}

void MString_Combine2(MString** str, const char* str1, const char* str2)
{
	MString_AssignString(str, str1);
	MString_AddAssignString(str, str2);
}
void MString_Combine3(MString** str, const char* str1, const char* str2, const char* str3)
{
	MString_Combine2(str, str1, str2);
	MString_AddAssignString(str, str3);
}
void MString_Combine4(MString** str, const char* str1, const char* str2, const char* str3, const char* str4)
{
	MString_Combine3(str, str1, str2, str3);
	MString_AddAssignString(str, str4);
}
void MString_Combine5(MString** str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5)
{
	MString_Combine4(str, str1, str2, str3, str4);
	MString_AddAssignString(str, str5);
}
void MString_Combine6(MString** str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6)
{
	MString_Combine5(str, str1, str2, str3, str4, str5);
	MString_AddAssignString(str, str6);
}
void MString_Combine7(MString** str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7)
{
	MString_Combine6(str, str1, str2, str3, str4, str5, str6);
	MString_AddAssignString(str, str7);
}
void MString_Combine8(MString** str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8)
{
	MString_Combine7(str, str1, str2, str3, str4, str5, str6, str7);
	MString_AddAssignString(str, str8);
}
void MString_DebugPrintLeakInfo(void)
{
#ifdef FIND_THE_LEAKS
	_mHasLeakTestBegun = true;
	int64_t len = hmlen(_mLeakTest);
	for (int32_t i = 0; i < len; i += 1)
	{
		Logger_printf(MString_Text(_mLeakTest[i].key));
		Logger_printf("\n");
	}
#endif
}
