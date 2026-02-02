/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"

enum
{
	FIXED_LIST_EIGHT_INT_VALUES_LEN = 8
};

typedef struct FixedListEightInt
{
	int32_t _mCount;
	int32_t _mValues[FIXED_LIST_EIGHT_INT_VALUES_LEN];
} FixedListEightInt;

void FixedListEightInt_AddAll(FixedListEightInt* from, FixedListEightInt* to);
int32_t FixedListEightInt_Count(const FixedListEightInt* flei);
void FixedListEightInt_Clear(FixedListEightInt* flei);
bool FixedListEightInt_Contains(const FixedListEightInt* flei, int32_t value);
int32_t FixedListEightInt_Get(const FixedListEightInt* flei, int32_t i);
void FixedListEightInt_Set(FixedListEightInt* flei, int32_t i, int32_t value);
void FixedListEightInt_Add(FixedListEightInt* flei, int32_t value);