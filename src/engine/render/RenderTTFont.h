/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../io/FixedByteBuffer.h"
#include "../font/FontMapData.h"
#include "../render/Color.h"
#include "../math/Rectangle.h"

void RenderTTFont_Create(const char* name, FixedByteBuffer* data);
void RenderTTFont_Draw(const FontMapData* replacement, const char* text, Color color, Vector2 position);
Rectangle RenderTTFont_GetBounds(const FontMapData* replacement, const char* text);
