#pragma once

#include "PartFuncData.h"

#define PART_PROP_FRAMES_DRAG_TIME 7
#define KEY_WINDOW_PROP_SELECTOR "Prop Mode"
#define KEY_WINDOW_PROP_SELECTOR_PREVIEW "Preview Prop"

typedef struct SpriteBatch SpriteBatch;

PartFuncData PartProp_GetFuncData();
void PartProp_CreateWindows();
void PartProp_Update();
void PartProp_DrawHud(SpriteBatch* spriteBatch);
void PartProp_Draw(SpriteBatch* spriteBatch);
void PartProp_JustChangedToThisPart();
void PartProp_JustLoadedMap();