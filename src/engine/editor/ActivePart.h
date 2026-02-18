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
void ActivePart_Update(double deltaTime);
void ActivePart_Draw(SpriteBatch* spriteBatch);
void ActivePart_DrawHud(SpriteBatch* spriteBatch);
PartFuncData ActivePart_GetCurrentPart(void);