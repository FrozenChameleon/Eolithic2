/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Strings.h"

#include "../core/Game.h"
#include "../core/GameHelper.h"
#include "../utils/Logger.h"
#include "../utils/Utils.h"
#include "../utils/IStringMap.h"
#include "../../third_party/stb_ds.h"

typedef struct MappingInfo
{
	const char* mKey;
	bool mDoNotReplace;
} MappingInfo;

static IStringMap* _mMap;
static MappingInfo* arr_info_list;

static bool _mHasInit;

void Strings_Init(void)
{
	if (_mHasInit)
	{
		return;
	}

	_mMap = IStringMap_Create();

	GameHelper_AddStrings();

	_mHasInit = true;
}

const char* Strings_Get(const char* key)
{
	if (!IStringMap_Contains(_mMap, key))
	{
		return key;
	}

	return IStringMap_Get(_mMap, key);
}
void Strings_Add(const char* key, const char* val)
{
	Strings_AddDoNotReplace(key, val, false);
}
void Strings_AddDoNotReplace(const char* key, const char* val, bool doNotReplace)
{
	if (IStringMap_Contains(_mMap, key))
	{
		Logger_LogError("String map already contains string");
		return;
	}

	IStringMap_Add(_mMap, key, val);

	if (doNotReplace)
	{
		MappingInfo info = { 0 };
		info.mKey = key;
		info.mDoNotReplace = true;
		arrput(arr_info_list, info);
	}
}
bool Strings_Contains(const char* key)
{
	return IStringMap_Contains(_mMap, key);
}
