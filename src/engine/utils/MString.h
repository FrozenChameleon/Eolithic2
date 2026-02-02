/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"

typedef struct MString MString;
typedef struct BufferReader BufferReader;

uint64_t MString_GetRefs(void);
char* MString_Text(const MString* str);
int32_t MString_Length(const MString* str);
int32_t MString_Capacity(const MString* str);
bool MString_IsEmpty(const MString* str);
char MString_GetLastChar(const MString* str);
bool MString_EqualToString(const MString* str, const char* otherStr);
bool MString_EqualTo(const MString* str, const MString* otherStr);
void MString_AssignEmptyString(MString** str);
void MString_AssignString(MString** str, const char* toThis);
void MString_AssignMString(MString** str, MString* toThis);
void MString_Clear(MString** str);
void MString_AssignEmpty(MString** str, int32_t capacity);
void MString_AssignSubString(MString** str, const char* fromThis, int32_t startIndex, int32_t length);
void MString_AddAssignBool(MString** str, bool addThisBool);
void MString_AddAssignInt8(MString** str, int8_t addThisInt8);
void MString_AddAssignInt16(MString** str, int16_t addThisInt16);
void MString_AddAssignInt(MString** str, int32_t addThisInt);
void MString_AddAssignInt64(MString** str, int64_t addThisInt64);
void MString_AddAssignUInt8(MString** str, uint8_t addThisUInt8);
void MString_AddAssignUInt16(MString** str, uint16_t addThisUInt16);
void MString_AddAssignUInt32(MString** str, uint32_t addThisUInt32);
void MString_AddAssignUInt64(MString** str, uint64_t addThisUInt64);
void MString_AddAssignFloat(MString** str, float addThisFloat);
void MString_AddAssignDouble(MString** str, double addThisDouble);
void MString_AddAssignChar(MString** str, char addThisChar);
void MString_AddAssignMString(MString** str, const MString* addThisStr);
void MString_AddAssignString(MString** str, const char* addThisStr);
void MString_Truncate(MString** str, int32_t newLength);
void MString_Dispose(MString** str);
void MString_Read(MString** str, BufferReader* br);
void MString_Combine2(MString** str, const char* str1, const char* str2);
void MString_Combine3(MString** str, const char* str1, const char* str2, const char* str3);
void MString_Combine4(MString** str, const char* str1, const char* str2, const char* str3, const char* str4);
void MString_Combine5(MString** str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5);
void MString_Combine6(MString** str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6);
void MString_Combine7(MString** str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7);
void MString_Combine8(MString** str, const char* str1, const char* str2, const char* str3, const char* str4, const char* str5, const char* str6, const char* str7, const char* str8);
void MString_DebugPrintLeakInfo(void);