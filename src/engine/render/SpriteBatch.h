/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "RenderStream.h"
#include "Texture.h"
#include "ShaderProgram.h"

#define SPRITEBATCH_RENDER_STREAMS_LEN 201

typedef struct SpriteBatch
{
	RenderStream _mRenderStreams[SPRITEBATCH_RENDER_STREAMS_LEN];
	bool _mHasInit;
} SpriteBatch;

void SpriteBatch_DisposePinnedStrings(void);
void SpriteBatch_Init(SpriteBatch* sb);
void SpriteBatch_Clear(SpriteBatch* sb);
RenderCommandSheet* SpriteBatch_Draw(SpriteBatch* sb, Texture* texture, Color color, int32_t depth, ShaderProgram* program, Vector2 position,
	Rectangle sourceRectangle, Vector2 scale, float rotation, bool flipX, bool flipY, Vector2 origin);
RenderCommandSheet* SpriteBatch_DrawInterpolated(SpriteBatch* sb, Texture* texture, Color color, int32_t depth, ShaderProgram* program, Vector2 position, Vector2 lastPosition,
	Rectangle sourceRectangle, Vector2 scale, float rotation, bool flipX, bool flipY, Vector2 origin);

RenderCommandManyRectangle* SpriteBatch_DrawManyRectangle(SpriteBatch* sb, int32_t depth, DrawRectangle* manyRectangles);
RenderCommandTileLayer* SpriteBatch_DrawLayer(SpriteBatch* sb, Texture* tileset, Color color, Tile** tileData, Rectangle tileDataBounds, int32_t depth, int32_t layer,
	int32_t x1, int32_t x2, int32_t y1, int32_t y2);

RenderCommandString* SpriteBatch_DrawString(SpriteBatch* sb, const char* font, const char* str, Color color, int32_t depth, Vector2 position);
RenderCommandString* SpriteBatch_DrawString2(SpriteBatch* sb, const char* font, const char* str, Color color, int32_t depth, Vector2 position,
	int32_t alignmentX, int32_t alignmentY);
RenderCommandString* SpriteBatch_DrawString3(SpriteBatch* sb, const char* font, const char* str, Color color, int32_t depth, Vector2 position,
	int32_t alignmentX, int32_t alignmentY, bool isLockedToInt);

RenderCommandSheet* SpriteBatch_DrawRectangle(SpriteBatch* sb, Texture* texture, Color color, int32_t depth, ShaderProgram* program, Rectangle destinationRectangle, Rectangle sourceRectangle,
	float rotation, bool flipX, bool flipY, Vector2 origin);