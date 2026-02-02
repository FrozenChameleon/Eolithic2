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

#ifdef RENDER_DUMMY

#include "Renderer.h"

void Renderer_BeforeRender(void)
{
}
void Renderer_AfterRender(void)
{
}
void Renderer_DrawTtText(Texture* texture, const float* verts, const float* tcoords, const unsigned int* colors, int32_t nverts)
{
}
void Renderer_DrawVertexPositionColorTexture4(Texture* texture, const VertexPositionColorTexture4* sprite)
{
}
Texture* Renderer_GetTextureData(const char* path, FixedByteBuffer* blob)
{
	return Renderer_GetNewTextureData(path, 16, 16, false);
}
Texture* Renderer_GetNewTextureData(const char* path, int32_t width, int32_t height, bool clearTexture)
{
	Texture* tex = Utils_calloc(1, sizeof(Texture));
	MString_AssignString(&tex->mPath, path);
	tex->mBounds.Width = width;
	tex->mBounds.Height = height;
	tex->mTextureData = NULL;
	return tex;
}
void Renderer_UpdateTextureData(Texture* texture, int32_t x, int32_t y, int32_t w, int32_t h, int32_t level, void* data, int32_t dataLength)
{
}
int32_t Renderer_Init(void* deviceWindowHandle)
{
	return 0;
}
void Renderer_BeforeCommit(void)
{
}
void Renderer_AfterCommit(void)
{
}
void Renderer_FlushBatch(void)
{
}
void Renderer_EnableDepthBufferWrite(void)
{
}
void Renderer_DisableDepthBufferWrite(void)
{
}
void Renderer_UpdateVsync(void)
{
}
void Renderer_ApplyChanges(void)
{
}
void Renderer_UpdateViewport(void)
{
}
void Renderer_UpdateScissor(void)
{
}
void Renderer_ResetBackBuffer(void)
{
}
Rectangle Renderer_GetDrawableSize(void)
{
	return Rectangle_Empty;
}

#endif

typedef int compiler_warning_compliance;