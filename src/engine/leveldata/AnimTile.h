/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "../render/Animation.h"
#include "../render/Sheet.h"
#include "../io/BufferReader.h"
#include "../utils/MString.h"

typedef struct AnimTile
{
	bool _mIsSetup;
	int32_t _mWrapSpeedCounter;
	Point _mWrapOffset;
	Animation _mAnimation;
	Sheet* _mWrapSheet;

	bool mIsAdditive;
	int32_t mScaler;
	bool mIsFlipX;
	bool mIsFlipY;
	int32_t mFlipSpeed;
	float mRotation;
	char mTextureName[EE_FILENAME_MAX];
	char mTilesetFilter[EE_FILENAME_MAX];
	bool mIsWrap;
	char mWrapTextureName[EE_FILENAME_MAX];
	bool mIsWrapX;
	int32_t mWrapSpeedX;
	bool mIsWrapY;
	int32_t mWrapSpeedY;
	int32_t mWrapSpeedDelay;
} AnimTile;

void AnimTile_Read(AnimTile* at, BufferReader* br);
AnimTile* AnimTile_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);
void AnimTile_Dispose(AnimTile* at);
Animation* AnimTile_GetAnimation(AnimTile* at);
void AnimTile_UpdateResource(AnimTile* at);
void AnimTile_Draw(AnimTile* at, SpriteBatch* spriteBatch, Color color, int32_t depth, int32_t x, int32_t y, float rotation, bool flipX, bool flipY);
Sheet* AnimTile_GetAnimationSheet(AnimTile* at);
void AnimTile_LoadAnimation(AnimTile* at);