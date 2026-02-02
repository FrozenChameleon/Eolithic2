/* EolithicEngine
 * Copyright 2025 Patrick Derosby
 * Released under the zlib License.
 * See LICENSE for details.
 */
 
//THIS CODE IS AUTOMATICALLY GENERATED, DO NOT EDIT!

#pragma once

#include "Resource.h"
#include "../utils/Macros.h"
#include "../io/BufferReader.h"
%strIncludePath%

Resource* %strResourceManager%_GetResource(const char* filenameWithoutExtension);
%strResource%* %strResourceManager%_GetResourceData(const char* filenameWithoutExtension);
Resource* %strResourceManager%_LoadAssetFromStreamAndCreateResource(BufferReader* br, const char* filenameWithoutExtension, const char* path);
const char* %strResourceManager%_GetDatFileName();
void %strResourceManager%_LoadAllFromDat();
void %strResourceManager%_Dispose(const char* filenameWithoutExtension);
void %strResourceManager%_DisposeAll();
int64_t %strResourceManager%_Length();
Resource* %strResourceManager%_GetResourceByIndex(int index);
%strResource%* %strResourceManager%_GetResourceDataByIndex(int index);