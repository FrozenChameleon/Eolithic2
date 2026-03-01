/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "IStringArray.h"

#include "IStrings.h"
#include "../utils/Utils.h"
#include "../utils/Logger.h"
#include "../../third_party/stb_ds.h"

typedef struct IStringArray
{
	struct { int64_t key; const char* value; } *hm_values;
	IStrings* strings;
	bool isJustThisFrame;
} IStringArray;

static uint64_t _mRefs;
static IStringArray** arr_just_this_frame;

IStringArray* IStringArray_CreateForJustThisFrame()
{
	IStringArray* sa = IStringArray_Create();
	sa->isJustThisFrame = true;
	arrput(arr_just_this_frame, sa);
	return sa;
}
IStringArray* IStringArray_Create(void)
{
	_mRefs += 1;
	IStringArray* sa = (IStringArray*)Utils_calloc(1, sizeof(IStringArray));
	sa->strings = IStrings_Create();
	return sa;
}
static void DisposeHelper(IStringArray* sa, bool checkForIsJustThisFrame)
{
	if (sa == NULL)
	{
		Logger_LogInformation("Attempted to dispose NULL IStringArray");
		return;
	}

	if (checkForIsJustThisFrame && sa->isJustThisFrame)
	{
		Logger_LogWarning("Attempted to dispose just-this-frame IStringArray in incorrect way!");
		return;
	}

	_mRefs -= 1;
	hmfree(sa->hm_values);
	IStrings_Dispose(sa->strings);
	Utils_free(sa);
}

void IStringArray_Dispose(IStringArray* sa)
{
	DisposeHelper(sa, true);
}
void IStringArray_Clear(IStringArray* sa)
{
	if (hmlen(sa->hm_values) > 0)
	{
		hmfree(sa->hm_values);
		sa->hm_values = NULL;
	}
	IStrings_Clear(sa->strings);
}
void IStringArray_Add(IStringArray* sa, const char* str)
{
	const char* internedString = IStrings_Get(sa->strings, str);
	int64_t len = hmlen(sa->hm_values);
	hmput(sa->hm_values, len, internedString);
}
bool IStringArray_Contains(IStringArray* sa, const char* containsThis)
{
	int64_t len = IStringArray_Length(sa);
	for (int32_t i = 0; i < len; i += 1)
	{
		const char* tempStr = IStringArray_Get(sa, i);
		if (Utils_StringEqualTo(tempStr, containsThis))
		{
			return true;
		}
	}
	return false;
}
int64_t IStringArray_Length(const IStringArray* sa)
{
	if (sa == NULL)
	{
		return 0;
	}

	return hmlen(sa->hm_values);
}
const char* IStringArray_Get(IStringArray* sa, int32_t index)
{
	if (sa == NULL)
	{
		return NULL;
	}

	return sa->hm_values[index].value;
}
uint64_t IStringArray_GetRefs(void)
{
	return _mRefs;
}
void IStringArray_DisposeJustThisFrameAllocations()
{
	for (int i = 0; i < arrlen(arr_just_this_frame); i += 1)
	{
		DisposeHelper(arr_just_this_frame[i], false);
	}
	arrsetlen(arr_just_this_frame, 0);
}
