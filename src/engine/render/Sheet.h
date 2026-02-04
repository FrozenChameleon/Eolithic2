/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "Color.h"
#include "RenderCommands.h"
#include "../math/Vector2.h"
#include "../math/Rectangle.h"
#include "../resources/Resource.h"
#include "../utils/IStringArray.h"

typedef struct DrawInstance DrawInstance;
typedef struct SpriteBatch SpriteBatch;
typedef struct ShaderProgram ShaderProgram;

typedef struct Sheet
{
	char mSheetName[EE_FILENAME_MAX];
	char mUnderlyingTextureName[EE_FILENAME_MAX];
	Rectangle mRectangle;
	Resource* mTextureResource;
} Sheet;

Sheet* Sheet_GetDefaultSheet(void);
Sheet* Sheet_GetSheet(const char* name);
void Sheet_BuildSheets(void);
bool Sheet_HasSheet(const char* name);
void Sheet_GetListOfSheetNames(IStringArray* addToThis);
Resource* Sheet_GetTextureResource(Sheet* sheet);
Texture* Sheet_GetTexture(Sheet* sheet);

RenderCommandSheet* Sheet_Draw(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY, ShaderProgram* program,
	Vector2 position);
 RenderCommandSheet* Sheet_Draw2(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY, ShaderProgram* program,
	Vector2 position, Vector2 scale);
 RenderCommandSheet* Sheet_Draw3(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY, ShaderProgram* program,
	Vector2 position, Vector2 scale, float rotation);
 RenderCommandSheet* Sheet_Draw4(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY, ShaderProgram* program,
	Vector2 position, Vector2 scale, float rotation, bool flipX, bool flipY);
 RenderCommandSheet* Sheet_Draw5(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY, ShaderProgram* program,
	Vector2 position, Vector2 scale, float rotation, bool flipX, bool flipY, Vector2 origin);

RenderCommandSheet* Sheet_DrawInterpolated(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY, ShaderProgram* program,
	Vector2 position, Vector2 lastPosition);
RenderCommandSheet* Sheet_DrawInterpolated2(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY, ShaderProgram* program,
	Vector2 position, Vector2 lastPosition, Vector2 scale);
RenderCommandSheet* Sheet_DrawInterpolated3(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY, ShaderProgram* program,
	Vector2 position, Vector2 lastPosition, Vector2 scale, float rotation);
RenderCommandSheet* Sheet_DrawInterpolated4(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY, ShaderProgram* program,
	Vector2 position, Vector2 lastPosition, Vector2 scale, float rotation, bool flipX, bool flipY);
RenderCommandSheet* Sheet_DrawInterpolated5(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenterX, bool isCenterY, ShaderProgram* program,
	Vector2 position, Vector2 lastPosition, Vector2 scale, float rotation, bool flipX, bool flipY, Vector2 origin);

RenderCommandSheet* Sheet_DrawDestinationRect(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenter, ShaderProgram* program,
	Rectangle destinationRect, float rotation, bool flipX, bool flipY, Vector2 origin);
RenderCommandSheet* Sheet_DrawSourceRect(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenter, ShaderProgram* program,
	Vector2 position, Rectangle sourceRectangle, float scale, float rotation, bool flipX, bool flipY);
RenderCommandSheet* Sheet_DrawSourceRectInterpolated(Sheet* sheet, SpriteBatch* spriteBatch, Color color, int32_t depth, bool isCenter, ShaderProgram* program,
	Vector2 position, Vector2 lastPosition, Rectangle sourceRectangle, float scale, float rotation, bool flipX, bool flipY);