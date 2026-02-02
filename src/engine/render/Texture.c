/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Texture.h"

#include "../render/Renderer.h"

void Texture_Dispose(Texture* tex)
{
	//UNUSED
}
Texture* Texture_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	return Renderer_GetTextureData(path, BufferReader_GetBuffer(br));
}
int32_t Texture_GetWidth(Texture* tex)
{
	return tex->mBounds.Width;
}
int32_t Texture_GetHeight(Texture* tex)
{
	return tex->mBounds.Height;
}
void* Texture_GetTexture2D(Texture* tex)
{
	return tex->mTextureData;
}