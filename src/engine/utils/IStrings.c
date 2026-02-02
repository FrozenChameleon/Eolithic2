/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "IStrings.h"

#include "../utils/Utils.h"
#include "../../third_party/stb_ds.h"

typedef struct IStrings
{
	struct { const char* key; int32_t value; } *sh_values;
} IStrings;

static uint64_t _mRefs;

IStrings* IStrings_Create(void)
{
	_mRefs += 1;
	IStrings* is = (IStrings*)Utils_calloc(1, sizeof(IStrings));
	sh_new_arena(is->sh_values);
	return is;
}
void IStrings_Dispose(IStrings* is)
{
	_mRefs -= 1;
	shfree(is->sh_values);
	Utils_free(is);
}
void IStrings_Clear(IStrings* is)
{
	if (shlen(is->sh_values) > 0)
	{
		shfree(is->sh_values);
		sh_new_arena(is->sh_values);
	}
}
const char* IStrings_Get(IStrings* is, const char* str)
{
	shput(is->sh_values, str, 0);
	ptrdiff_t index = shgeti(is->sh_values, str);
	return is->sh_values[index].key;
}
size_t IStrings_Length(IStrings* is)
{
	return shlen(is->sh_values);
}
const char* IStrings_GetByIndex(IStrings* is, int32_t index)
{
	return is->sh_values[index].key;
}
uint64_t IStrings_GetRefs(void)
{
	return _mRefs;
}