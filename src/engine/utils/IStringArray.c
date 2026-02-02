/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "IStringArray.h"

#include "../utils/Utils.h"
#include "../../third_party/stb_ds.h"
#include "IStrings.h"
#include "../utils/Logger.h"

static uint64_t _mRefs;

typedef struct IStringArray
{
	struct { int64_t key; const char* value; } *hm_values;
	IStrings* mStrings;
} IStringArray;

IStringArray* IStringArray_Create(void)
{
	_mRefs += 1;
	IStringArray* sa = (IStringArray*)Utils_calloc(1, sizeof(IStringArray));
	sa->mStrings = IStrings_Create();
	return sa;
}
void IStringArray_Dispose(IStringArray* sa)
{
	if (sa == NULL)
	{
		Logger_LogInformation("Attempted to dispose NULL IStringArray");
		return;
	}

	_mRefs -= 1;
	hmfree(sa->hm_values);
	IStrings_Dispose(sa->mStrings);
	Utils_free(sa);
}
void IStringArray_Clear(IStringArray* sa)
{
	if (hmlen(sa->hm_values) > 0)
	{
		hmfree(sa->hm_values);
		sa->hm_values = NULL;
	}
	IStrings_Clear(sa->mStrings);
}
void IStringArray_Add(IStringArray* sa, const char* str)
{
	const char* internedString = IStrings_Get(sa->mStrings, str);
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
int64_t IStringArray_Length(IStringArray* sa)
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
