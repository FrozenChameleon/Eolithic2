/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

 /* Derived from code by Ethan Lee (Copyright 2009-2024).
  * Released under the Microsoft Public License.
  * See fna.LICENSE for details.

  * Derived from code by the Mono.Xna Team (Copyright 2006).
  * Released under the MIT License.
  * See monoxna.LICENSE for details.
  */

#pragma once

#include "stdint.h"
#include "stdbool.h"

#pragma once

#include "../render/Color.h"
#include "../math/Rectangle.h"
#include "../math/Vector2.h"
#include "../math/Matrix.h"
#include "../render/Texture.h"
#include "../render/ShaderProgram.h"
#include "SpriteSortMode.h"
#include "BlendState.h"
#include "SpriteEffects.h"
#include "../io/FixedByteBuffer.h"
#include "SpriteBatch.h"
#include "../font/BmFont.h"

typedef struct TTFontState
{
	Vector2 mPosition;
	Color mColor;
	float mScaleFactor;
} TTFontState;

typedef struct PositionFloat3
{
	float X;
	float Y;
	float Z;
} PositionFloat3;

typedef struct ColorFloat4
{
	float R;
	float G;
	float B;
	float A;
} ColorFloat4;

typedef struct TextureCoordinateFloat2
{
	float U;
	float V;
} TextureCoordinateFloat2;

typedef struct VertexPositionColorTexture
{
	PositionFloat3 Position;
	ColorFloat4 Color;
	TextureCoordinateFloat2 TextureCoordinate;
} VertexPositionColorTexture;

typedef struct VertexPositionColorTexture4
{
	PositionFloat3 Position0;
	PositionFloat3 Position1;
	PositionFloat3 Position2;
	PositionFloat3 Position3;
	ColorFloat4 Color0;
	ColorFloat4 Color1;
	ColorFloat4 Color2;
	ColorFloat4 Color3;
	TextureCoordinateFloat2 TextureCoordinate0;
	TextureCoordinateFloat2 TextureCoordinate1;
	TextureCoordinateFloat2 TextureCoordinate2;
	TextureCoordinateFloat2 TextureCoordinate3;
} VertexPositionColorTexture4;

typedef enum ShaderType
{
	RENDERER_SHADER_TYPE_SPRITE_EFFECT = 0,
	RENDERER_SHADER_TYPE_WHITE_HIT_FLASH = 1,
	RENDERER_SHADER_TYPE_MULTI_COLOR_REPLACE = 2,
	RENDERER_SHADER_TYPE_ALPHA_TEST = 3
} ShaderType;

typedef enum DrawType
{
	RENDERER_TYPE_SHEET = 1,
	RENDERER_TYPE_STRING = 2,
	RENDERER_TYPE_STRING_BUILDER = 3,
	RENDERER_TYPE_TILE_LAYER = 3,
	RENDERER_TYPE_MANY_RECTANGLE = 4
} DrawType;

typedef enum DrawMode
{
	RENDERER_DRAWMODE_ASPECT = 0,
	RENDERER_DRAWMODE_SUPERSAMPLE = 1,
	RENDERER_DRAWMODE_PIXELPERFECT = 2,
	RENDERER_DRAWMODE_END_OF_NORMAL_OPTIONS = 2,
	RENDERER_DRAWMODE_STRETCH = 3
} DrawMode;

void Renderer_SetGlobalShaderProgram(ShaderProgram* value);
ShaderProgram* Renderer_GetGlobalShaderProgram(void);
void Renderer_INTERNAL_SetCurrentShaderProgram(ShaderProgram* value);
ShaderProgram* Renderer_INTERNAL_GetCurrentShaderProgram(void);
void Renderer_INTERNAL_SetCurrentBlendState(BlendState value);
BlendState Renderer_INTERNAL_GetCurrentBlendState(void);
void Renderer_INTERNAL_SetCurrentCameraPosition(Vector2 value);
Vector2 Renderer_INTERNAL_GetCurrentCameraPosition(void);
void Renderer_INTERNAL_SetCurrentDepth(int32_t value);
int32_t Renderer_INTERNAL_GetCurrentDepth(void);
Rectangle Renderer_GetWantedBackBufferBounds(void);
int32_t Renderer_Init(void* deviceWindowHandle);
int32_t Renderer_InitSpriteBatch(void);
Texture* Renderer_GetTextureData(const char* path, FixedByteBuffer* blob);
Texture* Renderer_GetNewTextureData(const char* path, int32_t width, int32_t height, bool clearTexture);
void Renderer_UpdateTextureData(Texture* texture, int32_t x, int32_t y, int32_t w, int32_t h, int32_t level, void* data, int32_t dataLength);
SpriteEffects Renderer_GetEffects(bool flipX, bool flipY);
void* Renderer_CreateSurface(FixedByteBuffer* blob);
void Renderer_BeforeCommit(void);
void Renderer_AfterCommit(void);
void Renderer_FlushBatch(void);
void Renderer_EnableDepthBufferWrite(void);
void Renderer_DisableDepthBufferWrite(void);
void Renderer_LogInfo(const char* msg);
void Renderer_LogWarning(const char* msg);
void Renderer_LogError(const char* msg);
void Renderer_SetupVerticesForTTFont(VertexPositionColorTexture* vertices, Color fontColor, int32_t pos, const float* verts, const float* tcoords, const unsigned int* colors, int32_t nverts);
const TTFontState* Renderer_GetTTFontState(void);
void Renderer_SetupTTFontState(const TTFontState* fontState);
void Renderer_DrawTtText(Texture* texture, const float* verts, const float* tcoords, const unsigned int* colors, int32_t nverts);
void Renderer_SetupVerticesFromVPCT4(VertexPositionColorTexture* vertices, int32_t pos, const VertexPositionColorTexture4* sprite);
void Renderer_DrawVertexPositionColorTexture4(Texture* texture, const VertexPositionColorTexture4* sprite);
void Renderer_GenerateVertexInfo(Texture* texture, float sourceX, float sourceY, float sourceW, float sourceH,
	float destinationX, float destinationY, float destinationW, float destinationH,
	Color color, float originX, float originY, float rotationSin, float rotationCos, float depth, uint8_t effects);
void Renderer_PushSprite(Texture* texture, float sourceX, float sourceY, float sourceW, float sourceH,
	float destinationX, float destinationY, float destinationW, float destinationH,
	Color color, float originX, float originY, float rotationSin, float rotationCos, float depth, uint8_t effects);
void Renderer_Draw(Texture* texture, Rectangle destinationRectangle, Color color);
void Renderer_Draw2(Texture* texture, Vector2 position, Rectangle sourceRectangle, Color color);
void Renderer_Draw3(Texture* texture, Rectangle destinationRectangle, Rectangle sourceRectangle, Color color,
	float rotation, Vector2 origin, SpriteEffects effects, float layerDepth);
void Renderer_Draw4(Texture* texture, Vector2 position, Rectangle sourceRectangle, Color color, float rotation, Vector2 origin,
	Vector2 scale, SpriteEffects effects, float layerDepth);
void Renderer_Draw5(Texture* texture, Vector2 position, Rectangle sourceRectangle, Color color, float rotation,
	Vector2 origin, float scale, SpriteEffects effects, float layerDepth);
void Renderer_DrawSheet(RenderCommandSheet* draw, double delta);
void Renderer_DrawManyRectangles(RenderCommandManyRectangle* draw);
Rectangle Renderer_RenderBmFont(bool drawTheText, BmFont* bmf, const char* text, Color color, Vector2 position);
void Renderer_DrawString(RenderCommandString* draw, double delta);
void Renderer_DrawTiles(RenderCommandTileLayer* draw);
void Renderer_Commit(SpriteBatch* render, Vector2 cameraOffset, double delta);
void Renderer_BeforeRender(void);
void Renderer_AfterRender(void);
void Renderer_Render(double delta);
uint16_t* Renderer_GetDefaultIndexBufferData(void);
bool Renderer_IsVsync(void);
void Renderer_UpdateVsync(void);
void Renderer_UpdateViewport(void);
void Renderer_UpdateScissor(void);
void Renderer_ApplyChanges(void);
void Renderer_ResetBackBuffer(void);
Rectangle Renderer_GetDrawableSize(void);
void Renderer_UpdateDisplayDimensions(void);
Point Renderer_GetScreenOffset(void);
Vector2 Renderer_GetScreenScale(void);
Rectangle Renderer_GetScreenBounds(void);
void Renderer_SetupCommit(double delta);
void Renderer_SetupRenderState(void);
int32_t Renderer_GetFPS(void);
int32_t Renderer_GetRenderTargetScale(void);
