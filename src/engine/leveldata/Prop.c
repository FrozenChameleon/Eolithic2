/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Prop.h"

#include "../utils/Macros.h"
#include "../render/SpriteBatch.h"
#include "../io/File.h"
#include "../utils/Utils.h"
#include "../render/Color.h"
#include "../render/Sheet.h"
#include "../io/BufferReader.h"
#include "../render/DrawTool.h"
#include "../globals/Align.h"
#include "../utils/Logger.h"
#include "../render/BlendState.h"
#include "../io/DynamicByteBuffer.h"

#define TILE_SIZE GLOBAL_DEF_TILE_SIZE

#define VERSION_PROP 7

void Prop_Init(Prop* p)
{
	Utils_memset(p, 0, sizeof(Prop));

	p->mGraphics.mSheet = Sheet_GetDefaultSheet();
	p->mScaler = 1;
	Utils_strlcpy(p->mTextureName, EE_STR_EMPTY, EE_FILENAME_MAX);
	Utils_strlcpy(p->mTilesetFilter, EE_STR_EMPTY, EE_FILENAME_MAX);
}

Sheet* Prop_GetSheet(Prop* p)
{
	if (p->mIsAnimation)
	{
		return Animation_GetCurrentSheet(&p->mGraphics.mAnimation);
	}
	else
	{
		return p->mGraphics.mSheet;
	}
}
void Prop_Draw(Prop* p, SpriteBatch* spriteBatch, int32_t depth, Point position, float scale, float rotation, bool flipX, bool flipY, bool showInfo)
{
	Sheet* sheet = Prop_GetSheet(p);
	Rectangle rect = sheet->mRectangle;

	float scaler = p->mScaler * scale;
	float scaledWidth = (rect.Width * scaler);
	float scaledHeight = (rect.Height * scaler);

	if (showInfo)
	{
		DrawTool_DrawRectangle2(spriteBatch, COLOR_GREEN, 200, Rectangle_Create(position.X, position.Y, TILE_SIZE / 4, TILE_SIZE / 4), 0, false);
		SpriteBatch_DrawString2(spriteBatch, "editor", Utils_IntToStringGlobalBuffer(depth), COLOR_YELLOW, 200,
			Vector2_Create(position.X + (scaledWidth / 2), position.Y + (scaledHeight / 2)), ALIGN_CENTER, ALIGN_CENTER);
	}
	else
	{
		RenderCommandSheet* instance = Sheet_Draw5(sheet, spriteBatch, COLOR_WHITE, depth, false, false, NULL,
			Vector2_Create(position.X + (scaledWidth / 2) - (rect.Width / 2.0f), position.Y + (scaledHeight / 2) - (rect.Height / 2.0f)),
			Vector2_Create2(scaler), rotation, flipX, flipY,
			Vector2_DivFloat(Vector2_Create((float)rect.Width, (float)rect.Height), 2.0f));
		if (p->mIsAdditive)
		{
			instance->mBlendState = BLENDSTATE_ADDITIVE;
		}
	}
}
void Prop_UpdateResource(Prop* p)
{
	if (!p->mGraphics.mIsSetup)
	{
		Prop_LoadDrawing(p);
		p->mGraphics.mIsSetup = true;
	}

	if (p->mIsAnimation)
	{
		Animation_Update(&p->mGraphics.mAnimation);
	}
}
void Prop_LoadDrawing(Prop* p)
{
	if (p->mIsAnimation)
	{
		Animation_Init(&p->mGraphics.mAnimation, p->mTextureName, p->mFlipSpeed);
	}
	else
	{
		p->mGraphics.mSheet = Sheet_GetSheet(p->mTextureName);
	}
}
Rectangle Prop_GetRectangle(Prop* p)
{
	if (!p->mIsAnimation)
	{
		return p->mGraphics.mSheet->mRectangle;
	}
	else
	{
		return Animation_GetCurrentSheet(&p->mGraphics.mAnimation)->mRectangle;
	}
}
const char* Prop_ToString(Prop* p)
{
	return p->mTextureName;
}
Animation* Prop_GetAnimation(Prop* p)
{
	return &p->mGraphics.mAnimation;
}
void Prop_Read(Prop* p, BufferReader* br)
{
	if (!BufferReader_ReadMagicNumber(br))
	{
		return;
	}
	BufferReader_ReadVersionNumber(br);

	p->mIsAdditive = BufferReader_ReadBoolean(br);
	p->mScaler = BufferReader_ReadI32(br);
	p->mFlipSpeed = BufferReader_ReadI32(br);
	p->mIsAnimation = BufferReader_ReadBoolean(br);
	BufferReader_ReadString(br, p->mTextureName, EE_FILENAME_MAX);
	BufferReader_ReadString(br, p->mTilesetFilter, EE_FILENAME_MAX);
}
void Prop_Write(Prop* p, DynamicByteBuffer* dbb)
{
	DynamicByteBuffer_WriteMagicNumber(dbb);
	DynamicByteBuffer_WriteVersionNumber(dbb, VERSION_PROP);

	DynamicByteBuffer_WriteBoolean(dbb, p->mIsAdditive);
	DynamicByteBuffer_WriteI32(dbb, p->mScaler);
	DynamicByteBuffer_WriteI32(dbb, p->mFlipSpeed);
	DynamicByteBuffer_WriteBoolean(dbb, p->mIsAnimation);
	DynamicByteBuffer_WriteString(dbb, p->mTextureName, EE_FILENAME_MAX);
	DynamicByteBuffer_WriteString(dbb, p->mTilesetFilter, EE_FILENAME_MAX);
}
IStringArray* Prop_GetDirectories(void)
{
	return NULL;
}
Prop* Prop_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	Prop* p = (Prop*)Utils_calloc(1, sizeof(Prop));
	Prop_Init(p);
	Prop_Read(p, br);
	return p;
}
void Prop_Dispose(Prop* prop)
{
	Utils_free(prop);
}