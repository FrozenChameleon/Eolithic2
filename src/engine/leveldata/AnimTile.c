/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "AnimTile.h"

#include "../utils/Macros.h"
#include "../utils/Utils.h"
#include "../io/File.h"
#include "../render/SpriteBatch.h"
#include "../render/Sheet.h"
#include "../render/BlendState.h"

#define TILE_SIZE GLOBAL_DEF_TILE_SIZE

const static int32_t VERSION = 3;

void AnimTile_Read(AnimTile* at, BufferReader* br)
{
	at->mIsAdditive = BufferReader_ReadBoolean(br);
	at->mScaler = BufferReader_ReadI32(br);
	at->mIsFlipX = BufferReader_ReadBoolean(br);
	at->mIsFlipY = BufferReader_ReadBoolean(br);
	at->mFlipSpeed = BufferReader_ReadI32(br);
	at->mRotation = BufferReader_ReadFloat(br);
	BufferReader_ReadString(br, at->mTextureName, EE_FILENAME_MAX);
	BufferReader_ReadString(br, at->mTilesetFilter, EE_FILENAME_MAX);
	at->mIsWrap = BufferReader_ReadBoolean(br);
	BufferReader_ReadString(br, at->mWrapTextureName, EE_FILENAME_MAX);
	at->mIsWrapX = BufferReader_ReadBoolean(br);
	at->mWrapSpeedX = BufferReader_ReadI32(br);
	at->mIsWrapY = BufferReader_ReadBoolean(br);
	at->mWrapSpeedY = BufferReader_ReadI32(br);
	at->mWrapSpeedDelay = BufferReader_ReadI32(br);
}

AnimTile* AnimTile_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	AnimTile* at = (AnimTile*)Utils_calloc(1, sizeof(AnimTile));
	AnimTile_Read(at, br);
	return at;
}
void AnimTile_Dispose(AnimTile* at)
{
	Utils_free(at);
}
Animation* AnimTile_GetAnimation(AnimTile* at)
{
	return &at->_mAnimation;
}
void AnimTile_UpdateResource(AnimTile* at)
{
	if (!at->_mIsSetup)
	{
		AnimTile_LoadAnimation(at);
		at->_mIsSetup = true;
	}

	Animation_Update(&at->_mAnimation);

	if (at->_mWrapSpeedCounter >= at->mWrapSpeedDelay)
	{
		if (at->mIsWrapX)
		{
			at->_mWrapOffset.X += at->mWrapSpeedX;
		}
		else
		{
			at->_mWrapOffset.X = 0;
		}

		if (at->mIsWrapY)
		{
			at->_mWrapOffset.Y += at->mWrapSpeedY;
		}
		else
		{
			at->_mWrapOffset.Y = 0;
		}

		at->_mWrapSpeedCounter = 0;
	}
	else
	{
		at->_mWrapSpeedCounter += 1;
	}
}
void AnimTile_Draw(AnimTile* at, SpriteBatch* spriteBatch, Color color, int32_t depth, int32_t x, int32_t y, float rotation, bool flipX, bool flipY)
{
	int32_t scaler = at->mScaler;

	bool flippingX = flipX ? !at->mIsFlipX : at->mIsFlipX;
	bool flippingY = flipY ? !at->mIsFlipY : at->mIsFlipY;

	if (at->mIsWrap)
	{
		/*Sheet_DrawSourceRect(_mWrapSheet, spriteBatch, color, depth, true, nullptr, Vector2(x + (TILE_SIZE / 2), y + (TILE_SIZE / 2)),
			Rectangle(0 + _mWrapOffset.X, 0 + _mWrapOffset.Y, TILE_SIZE, TILE_SIZE),
			scaler, mRotation + rotation, flippingX, flippingY);*/ //UNUSED
	}
	else
	{
		RenderCommandSheet* instance = Sheet_Draw4(AnimTile_GetAnimationSheet(at), spriteBatch, color, depth, true, true, NULL,
			Vector2_Create((float)(x + (TILE_SIZE / 2)), (float)(y + (TILE_SIZE / 2))),
			Vector2_Create2((float)scaler), at->mRotation + rotation, flippingX, flippingY);
		if (at->mIsAdditive)
		{
			instance->mBlendState = BLENDSTATE_ADDITIVE;
		}
	}
}
Sheet* AnimTile_GetAnimationSheet(AnimTile* at)
{
	return Animation_GetCurrentSheet(&at->_mAnimation);
}
void AnimTile_LoadAnimation(AnimTile* at)
{
	if (at->mIsWrap)
	{
		if (!Utils_StringEqualTo(at->mWrapTextureName, EE_STR_NOT_SET))
		{
			//at->_mWrapSheet = new OeSheet(OeFile::Combine("data", "gfx", "special", "anim", mWrapTextureName + ".png")); //UNUSED
		}
	}
	else
	{
		if (!Utils_StringEqualTo(at->mTextureName, EE_STR_NOT_SET))
		{
			Animation_Init(&at->_mAnimation, at->mTextureName, at->mFlipSpeed);
		}
	}
}