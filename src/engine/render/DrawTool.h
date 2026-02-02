/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Point.h"
#include "../math/Rectangle.h"
#include "../math/Vector2.h"
#include "../render/Color.h"

typedef struct Texture Texture;
typedef struct SpriteBatch SpriteBatch;
typedef struct MString MString;

Texture* DrawTool_GetSinglePixel(void);
void DrawTool_DrawRectangle(SpriteBatch* spriteBatch, Color color, int32_t depth, int32_t x, int32_t y, int32_t width, int32_t height, float rotation, bool isCenter);
void DrawTool_DrawRectangle2(SpriteBatch* spriteBatch, Color color, int32_t depth, Rectangle destinationRect, float rotation, bool isCenter);
void DrawTool_DrawRectangleHollow(SpriteBatch* spriteBatch, Color color, int32_t depth, int32_t x, int32_t y, int32_t width, int32_t height, float rotation, bool isCenter, int32_t thickness);
void DrawTool_DrawRectangleHollow2(SpriteBatch* spriteBatch, Color color, int32_t depth, Rectangle destinationRect, float rotation, bool isCenter, int32_t thickness);
Rectangle DrawTool_GetBounds(const char* str, const char* font);
void DrawTool_DrawLine(SpriteBatch* spriteBatch, Color color, int32_t depth, int32_t delay, int32_t offset, int32_t size, Point begin, Point end);
void DrawTool_DrawLine2(SpriteBatch* spriteBatch, Color color, int32_t depth, int32_t delay, int32_t offset, int32_t size, Vector2 begin, Vector2 end);
void DrawTool_DrawLine3(SpriteBatch* spriteBatch, Color color, int32_t depth, int32_t delay, int32_t offset, int32_t size, float x1, float y1, float x2, float y2);
void DrawTool_DrawDebugInfoHelper(SpriteBatch* spriteBatch, const char* strToDraw, const char* font, Color color, int32_t depth, Vector2 textDiff, int32_t* textOff);
void DrawTool_DrawDebugInfoHelperMString(SpriteBatch* spriteBatch, MString* strToDraw, const char* font, Color color, int32_t depth, Vector2 textDiff, int32_t* textOff);
void DrawTool_DrawDebugRefCounts(SpriteBatch* spriteBatch, const char* font, Color color, int32_t depth, Vector2 textDiff, int32_t* textOff);