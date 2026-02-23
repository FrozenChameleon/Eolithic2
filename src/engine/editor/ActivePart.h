#pragma once

#include "../utils/Macros.h"
#include "PartFuncData.h"

typedef struct SpriteBatch SpriteBatch;

typedef enum EditorPartType
{
	EDITORPART_COLLISION = 0,
	EDITORPART_TILE_PICKER = 1,
	EDITORPART_THING = 2,
	EDITORPART_PROP = 3,
	EDITORPART_LINE = 4,
	EDITORPART_CAMERA = 5,
	EDITORPART_META_MAP = 6,
	EDITORPART_DRAW = 7
} EditorPartType;

void ActivePart_Init(void);
int32_t ActivePart_GetCurrentLayer(void);
void ActivePart_Update();
void ActivePart_Draw(SpriteBatch* spriteBatch);
void ActivePart_DrawHud(SpriteBatch* spriteBatch);
PartFuncData ActivePart_GetCurrentPart(void);
void ActivePart_SetPartToCollision();
void ActivePart_SetPartToTile();
void ActivePart_SetPartToThing();
void ActivePart_SetPartToProp();
void ActivePart_SetPartToLine();
void ActivePart_SetPartToCamera();
void ActivePart_SetPartToDraw();
void ActivePart_SetPartToMetaMap();