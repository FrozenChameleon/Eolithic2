/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "stdbool.h"
#include "../../defs/GlobalDefs.h"
#include "../../defs/DebugDefs.h"
#include "../../defs/GameDefs.h"
#include "../../defs/EditorDefs.h"

#define UNUSED(x) (void)(x)

#define EE_SAFE_BUFFER_LEN_FOR_INT 20
#define EE_SAFE_BUFFER_LEN_FOR_FLOAT 50
#define EE_SAFE_BUFFER_LEN_FOR_DOUBLE 100
#define EE_FILENAME_MAX 200
#define EE_PATH_MAX 4096
#define EE_STR_NOT_SET "NOT_SET"
#define EE_STR_EMPTY ""
#define ENTITY_NOTHING 0
#define EE_MAGIC_NUMBER 0x0309
#define TILE_SIZE GLOBAL_DEF_TILE_SIZE
#define HALF_TILE_SIZE GLOBAL_DEF_HALF_TILE_SIZE

typedef void (*EE_ACTION0)();
typedef void (*EE_ACTION1)(void* userData);

typedef int32_t Entity;
typedef int32_t ResourceID;