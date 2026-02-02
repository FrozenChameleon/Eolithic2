/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "DatInfo.h"
#include "../utils/MString.h"
#include "BufferReader.h"

typedef struct DatReader DatReader;

FixedByteBuffer* DatReader_GetMemoryStream(DatReader* dr, int64_t length);
bool DatReader_HasInit(const DatReader* dr);
bool DatReader_HasNext(const DatReader* dr);
void DatReader_NextFilePath(MString** assignToThis, DatReader* dr);
BufferReader* DatReader_NextStream(DatReader* dr, bool doNotReturnStream);
DatReader* DatReader_Create(const char* path);
void DatReader_Dispose(DatReader* dr);