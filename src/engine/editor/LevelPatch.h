#pragma once

#include "../utils/Macros.h"
#include "../math/Rectangle.h"

typedef enum LevelPatchStatus
{
	LEVEL_PATCH_STATUS_VALID = 0,
	LEVEL_PATCH_STATUS_INVALID = 1,
	LEVEL_PATCH_STATUS_INVALID_BECAUSE_SAME = 2,
} LevelPatchStatus;

typedef struct LevelData LevelData;
typedef struct Tile Tile;
typedef struct LevelPatch LevelPatch;

bool LevelPatch_IsReady(LevelPatch* lp);
LevelPatch* LevelPatch_Create(Tile* src);
LevelPatchStatus LevelPatch_Finish(LevelPatch* lp);
void LevelPatch_Undo(LevelPatch* lp);
void LevelPatch_Redo(LevelPatch* lp);
uint64_t LevelPatch_GetOperationCounter(LevelPatch* lp);