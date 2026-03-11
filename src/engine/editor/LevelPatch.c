#include "LevelPatch.h"

#include "../leveldata/LevelData.h"
#include "../leveldata/Tile.h"
#include "../utils/Utils.h"
#include "../utils/Logger.h"
#include "EditorGlobals.h"

typedef struct LevelPatch
{
    Tile* new_tile;
    Tile* old_tile;
	Tile* src;
	uint64_t operationCounter;
	bool isReady;
} LevelPatch;

bool LevelPatch_IsReady(LevelPatch* lp)
{
	return lp->isReady;
}
LevelPatch* LevelPatch_Create(Tile* src)
{
	if (src == NULL)
    {
        Logger_Log(LOGGER_INFORMATION, "Given invalid data for level patch creation");
        return NULL;
    }

	LevelPatch* lp = (LevelPatch*)Utils_calloc(1, sizeof(LevelPatch));
	lp->src = src;
	lp->operationCounter = EditorGlobals_GetOperationCounter();;
    lp->new_tile = (Tile*)Utils_calloc(1, sizeof(Tile));
	lp->old_tile = (Tile*)Utils_calloc(1, sizeof(Tile));
    Tile_CopyTo(lp->old_tile, lp->src, false);
	return lp;
}
LevelPatchStatus LevelPatch_Finish(LevelPatch* lp)
{
	if ((lp == NULL) || (lp->src == NULL) || lp->isReady)
	{
		Logger_Log(LOGGER_INFORMATION, "Given invalid data for level patch finishing");
		return LEVEL_PATCH_STATUS_INVALID;
	}

    Tile_CopyTo(lp->new_tile, lp->src, false);
	if (Tile_EqualTo(lp->new_tile, lp->old_tile))
	{
		return LEVEL_PATCH_STATUS_INVALID_BECAUSE_SAME;
	}

    lp->isReady = true;

	return LEVEL_PATCH_STATUS_VALID;
}
void LevelPatch_Undo(LevelPatch* lp)
{
    if (!lp->isReady)
    {
        Logger_Log(LOGGER_INFORMATION, "Patch not ready for undo!");
		return;
    }

    Tile_CopyTo(lp->src, lp->old_tile, false);
}
void LevelPatch_Redo(LevelPatch* lp)
{
    if (!lp->isReady)
    {
        Logger_Log(LOGGER_INFORMATION, "Patch not ready for redo!");
        return;
    }

    Tile_CopyTo(lp->src, lp->new_tile, false);
}
uint64_t LevelPatch_GetOperationCounter(LevelPatch* lp)
{
	return lp->operationCounter;
}

/*
bool LevelPatch_EqualTo(LevelPatch* value1, LevelPatch* value2)
{
    if (!Rectangle_EqualTo(&value1->bounds, &value2->bounds))
    {
		return false;
    }

    if (_mTiles.GetLength(0) != patch._mTiles.GetLength(0) || _mTiles.GetLength(1) != patch._mTiles.GetLength(1))
    {
        return false;
    }

    for (int i = 0; i < _mTiles.GetLength(0); i += 1)
    {
        for (int j = 0; j < _mTiles.GetLength(1); j += 1)
        {
            if (!_mTiles[i, j].IsEqualTo(patch._mTiles[i, j]))
            {
                return false;
            }
        }
    }

    return true;
}

void LevelPatch_Apply(LevelData* levelData, bool applyOldTilesInstead)
{
    int counterX = 0;
    int counterY = 0;

    for (int i = _mBeginX; i < _mBeginX + _mTiles.GetLength(0); i += 1)
    {
        for (int j = _mBeginY; j < _mBeginY + _mTiles.GetLength(1); j += 1)
        {
            levelData.SetTile(i, j, _mTiles[counterX, counterY]);
            counterY += 1;
        }
        counterY = 0;
        counterX += 1;
    }

    _mBeginX = -1;
    _mBeginY = -1;
    _mTiles = null;
}
*/