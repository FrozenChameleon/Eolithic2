/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "IStringMap.h"

#include "../io/BufferReader.h"
#include "../utils/MString.h"
#include "../utils/Utils.h"
#include "../../third_party/stb_ds.h"
#include "IStrings.h"

static uint64_t _mRefs;

typedef struct IStringMap
{
	IStrings* mStrings;
	struct { const char* key; const char* value; } *sh_values;
} IStringMap;

IStringMap* IStringMap_Create(void)
{
	_mRefs += 1;
	IStringMap* sa = (IStringMap*)Utils_calloc(1, sizeof(IStringMap));
	sa->mStrings = IStrings_Create();
	return sa;
}
void IStringMap_Dispose(IStringMap* sm)
{
	_mRefs -= 1;
	shfree(sm->sh_values);
	IStrings_Dispose(sm->mStrings);
	Utils_free(sm);
}
void IStringMap_Clear(IStringMap* sm)
{
	shfree(sm->sh_values);
	sm->sh_values = NULL;
}
void IStringMap_Add(IStringMap* sm, const char* key, const char* value)
{
	const char* internedKey = IStrings_Get(sm->mStrings, key);
	const char* internedValue = IStrings_Get(sm->mStrings, value);
	shput(sm->sh_values, internedKey, internedValue);
}
const char* IStringMap_Get(IStringMap* sm, const char* key)
{
	return shget(sm->sh_values, key);
}
bool IStringMap_Contains(IStringMap* sm, const char* key)
{
	int64_t index = shgeti(sm->sh_values, key);
	if (index < 0)
	{
		return false;
	}
	return true;
}
int64_t IStringMap_Length(IStringMap* sm)
{
	return shlen(sm->sh_values);
}
const char* IStringMap_GetKeyByIndex(IStringMap* sm, int32_t index)
{
	return sm->sh_values[index].key;
}
const char* IStringMap_GetValueByIndex(IStringMap* sm, int32_t index)
{
	return sm->sh_values[index].value;
}
uint64_t IStringMap_GetRefs(void)
{
	return _mRefs;
}
void IStringMap_Load(IStringMap* sm, BufferReader* br)
{
	while (BufferReader_HasNext(br))
	{
		MString* key = NULL;
		MString* value = NULL;
		BufferReader_ReadMString(&key, br);
		BufferReader_ReadMString(&value, br);
		IStringMap_Add(sm, MString_Text(key), MString_Text(value));
		MString_Dispose(&key);
		MString_Dispose(&value);
	}
}
