/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "FixedListEightInt.h"

#include "../utils/Utils.h"
#include "../utils/Logger.h"

void FixedListEightInt_AddAll(FixedListEightInt* from, FixedListEightInt* to)
{
	for (int32_t i = 0; i < from->_mCount; i += 1)
	{
		FixedListEightInt_Add(to, FixedListEightInt_Get(from, i));
	}
}
int32_t FixedListEightInt_Count(const FixedListEightInt* flei)
{
	return flei->_mCount;
}
void FixedListEightInt_Clear(FixedListEightInt* flei)
{
	Utils_memset(flei, 0, sizeof(FixedListEightInt));
}
bool FixedListEightInt_Contains(const FixedListEightInt* flei, int32_t value)
{
	for (int32_t i = 0; i < flei->_mCount; i += 1)
	{
		if (flei->_mValues[i] == value)
		{
			return true;
		}
	}
	return false;
}
int32_t FixedListEightInt_Get(const FixedListEightInt* flei, int32_t i)
{
	if (i < flei->_mCount)
	{
		return flei->_mValues[i];
	}
	else
	{
		Logger_LogWarning("Fixed list eight int32_t invalid get");
		return -1;
	}
}
void FixedListEightInt_Set(FixedListEightInt* flei, int32_t i, int32_t value)
{
	if (i < flei->_mCount)
	{
		flei->_mValues[i] = value;
	}
	else
	{
		Logger_LogWarning("Fixed list eight int32_t invalid set");
	}
}
void FixedListEightInt_Add(FixedListEightInt* flei, int32_t value)
{
	if (flei->_mCount < FIXED_LIST_EIGHT_INT_VALUES_LEN)
	{
		flei->_mValues[flei->_mCount] = value;
		flei->_mCount += 1;
	}
	else
	{
		Logger_LogWarning("Fixed list eight int32_t over limit");
	}
}