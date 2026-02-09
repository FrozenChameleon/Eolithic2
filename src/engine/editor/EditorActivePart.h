#pragma once

#include "../utils/Macros.h"

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

int32_t EditorActivePart_GetCurrentLayer(void);
void EditorActivePart_Draw(SpriteBatch* spriteBatch);
void EditorActivePart_DrawHud(SpriteBatch* spriteBatch);
