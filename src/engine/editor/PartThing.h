#pragma once

#include "PartFuncData.h"

typedef struct SpriteBatch SpriteBatch;

PartFuncData PartThing_GetFuncData();
void PartThing_CreateWindows();
void PartThing_Update();
void PartThing_DrawHud(SpriteBatch* spriteBatch);
void PartThing_Draw(SpriteBatch* spriteBatch);
void PartThing_JustChangedToThisPart();


