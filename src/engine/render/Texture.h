/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Rectangle.h"
#include "../io/BufferReader.h"
#include "../utils/MString.h"

typedef struct Texture
{
	MString* mPath;
	void* mTextureData;
	//void* mSurfaceData;
	Rectangle mBounds;
} Texture;

void Texture_Dispose(Texture* tex);
Texture* Texture_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);
int32_t Texture_GetWidth(Texture* tex);
int32_t Texture_GetHeight(Texture* tex);
void* Texture_GetTexture2D(Texture* tex);