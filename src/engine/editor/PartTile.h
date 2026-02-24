#pragma once

#include "../utils/Macros.h"
#include "../math/Rectangle.h"
#include "PartFuncData.h"

typedef struct SpriteBatch SpriteBatch;
typedef struct DrawTile DrawTile;

PartFuncData PartTile_GetFuncData();
void PartTile_CreateWindows();
void PartTile_AutoTilerPass();
void PartTile_DrawSingleSelectionSelectedTiles(SpriteBatch* spriteBatch);
void PartTile_BucketFill(DrawTile* givenDraw, int x, int y);
void PartTile_Update();
void PartTile_UpdateHelper();
void PartTile_Draw(SpriteBatch* spriteBatch);
void PartTile_DrawHud(SpriteBatch* spriteBatch);
void PartTile_DrawHudHelper(SpriteBatch* spriteBatch);
void PartTile_OperationHelper(bool reset, bool resetAll, bool grabTile);
void PartTile_Operation(Rectangle selection, bool resetTile, bool resetAll, bool grabTile);
const char* PartTile_GetStatus();
void PartTile_JustChangedToThisPart();