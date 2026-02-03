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
#include "../io/BufferReader.h"
#include "../io/DynamicByteBuffer.h"

#define TILE_SIZE GLOBAL_DEF_TILE_SIZE

#define VERSION_ANIMTILE 7

void AnimTile_Read(AnimTile* at, BufferReader* br)
{
	if (!BufferReader_ReadMagicNumber(br))
	{
		return;
	}
	BufferReader_ReadVersionNumber(br);

	at->mIsAdditive = BufferReader_ReadBoolean(br);
	at->mScaler = BufferReader_ReadI32(br);
	at->mIsFlipX = BufferReader_ReadBoolean(br);
	at->mIsFlipY = BufferReader_ReadBoolean(br);
	at->mFlipSpeed = BufferReader_ReadI32(br);
	at->mRotation = BufferReader_ReadFloat(br);
	BufferReader_ReadString(br, at->mTextureName, EE_FILENAME_MAX);
	BufferReader_ReadString(br, at->mTilesetFilter, EE_FILENAME_MAX);
	BufferReader_ReadBoolean(br); //UNUSED
	char dummy[EE_FILENAME_MAX];
	BufferReader_ReadString(br, dummy, EE_FILENAME_MAX); //UNUSED
	BufferReader_ReadBoolean(br); //UNUSED
	BufferReader_ReadI32(br); //UNUSED
	BufferReader_ReadBoolean(br); //UNUSED
	BufferReader_ReadI32(br); //UNUSED
	BufferReader_ReadI32(br); //UNUSED
}
void AnimTile_Write(AnimTile* at, DynamicByteBuffer* dbb)
{
	DynamicByteBuffer_WriteMagicNumber(dbb);
	DynamicByteBuffer_WriteVersionNumber(dbb, VERSION_ANIMTILE);

	DynamicByteBuffer_WriteBoolean(dbb, at->mIsAdditive);
	DynamicByteBuffer_WriteI32(dbb, at->mScaler);
	DynamicByteBuffer_WriteBoolean(dbb, at->mIsFlipX);
	DynamicByteBuffer_WriteBoolean(dbb, at->mIsFlipY);
	DynamicByteBuffer_WriteI32(dbb, at->mFlipSpeed);
	DynamicByteBuffer_WriteFloat(dbb, at->mRotation);
	DynamicByteBuffer_WriteString(dbb, at->mTextureName, EE_FILENAME_MAX);
	DynamicByteBuffer_WriteString(dbb, at->mTilesetFilter, EE_FILENAME_MAX);
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
	return &at->mGraphics.mAnimation;
}
void AnimTile_UpdateResource(AnimTile* at)
{
	if (!at->mGraphics.mIsSetup)
	{
		AnimTile_LoadAnimation(at);
		at->mGraphics.mIsSetup = true;
	}

	Animation_Update(&at->mGraphics.mAnimation);
}
void AnimTile_Draw(AnimTile* at, SpriteBatch* spriteBatch, Color color, int32_t depth, int32_t x, int32_t y, float rotation, bool flipX, bool flipY)
{
	int32_t scaler = at->mScaler;

	bool flippingX = flipX ? !at->mIsFlipX : at->mIsFlipX;
	bool flippingY = flipY ? !at->mIsFlipY : at->mIsFlipY;

	RenderCommandSheet* instance = Sheet_Draw4(AnimTile_GetAnimationSheet(at), spriteBatch, color, depth, true, true, NULL,
		Vector2_Create((float)(x + (TILE_SIZE / 2)), (float)(y + (TILE_SIZE / 2))),
		Vector2_Create2((float)scaler), at->mRotation + rotation, flippingX, flippingY);
	if (at->mIsAdditive)
	{
		instance->mBlendState = BLENDSTATE_ADDITIVE;
	}
}
Sheet* AnimTile_GetAnimationSheet(AnimTile* at)
{
	return Animation_GetCurrentSheet(&at->mGraphics.mAnimation);
}
void AnimTile_LoadAnimation(AnimTile * at)
{
	if (!Utils_StringEqualTo(at->mTextureName, EE_STR_NOT_SET))
	{
		Animation_Init(&at->mGraphics.mAnimation, at->mTextureName, at->mFlipSpeed);
	}
}