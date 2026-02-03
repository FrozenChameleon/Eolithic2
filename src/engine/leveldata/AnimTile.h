/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "../render/Animation.h"
#include "../render/Sheet.h"
#include "../utils/MString.h"

typedef struct BufferReader BufferReader;
typedef struct DynamicByteBuffer DynamicByteBuffer;

typedef struct AnimTileGraphicsState
{
	bool mIsSetup;
	Animation mAnimation;
} AnimTileGraphicsState;

typedef struct AnimTile
{
	bool mIsAdditive;
	int32_t mScaler;
	bool mIsFlipX;
	bool mIsFlipY;
	int32_t mFlipSpeed;
	float mRotation;
	char mTextureName[EE_FILENAME_MAX];
	char mTilesetFilter[EE_FILENAME_MAX];
	AnimTileGraphicsState mGraphics;
} AnimTile;

void AnimTile_Read(AnimTile* at, BufferReader* br);
void AnimTile_Write(AnimTile* at, DynamicByteBuffer* dbb);
AnimTile* AnimTile_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);
void AnimTile_Dispose(AnimTile* at);
Animation* AnimTile_GetAnimation(AnimTile* at);
void AnimTile_UpdateResource(AnimTile* at);
void AnimTile_Draw(AnimTile* at, SpriteBatch* spriteBatch, Color color, int32_t depth, int32_t x, int32_t y, float rotation, bool flipX, bool flipY);
Sheet* AnimTile_GetAnimationSheet(AnimTile* at);
void AnimTile_LoadAnimation(AnimTile* at);