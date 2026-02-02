/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "LevelDataStub.h"
#include "../resources/Resource.h"
#include "../leveldata/LevelData.h"

LevelData* LevelDataStubFunc_GetLevelData(const LevelDataStub* data);
Resource* LevelDataStubFunc_GetLevelDataResource(const LevelDataStub* data);