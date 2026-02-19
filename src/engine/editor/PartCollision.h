#pragma once

#include "PartFuncData.h"

void PartCollision_CreateWindows();
void PartCollision_UpdateHelper();
const char* PartCollision_GetStatus();
void PartCollision_DrawHud(SpriteBatch* spriteBatch);
void PartCollision_Draw(SpriteBatch* spriteBatch);
void PartCollision_JustChangedToThisPart();
void PartCollision_Update(double deltaTime);
PartFuncData PartCollision_GetFuncData();

