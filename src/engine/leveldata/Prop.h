/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Rectangle.h"
#include "../render/Animation.h"
#include "../resources/Resource.h"

typedef struct SpriteBatch SpriteBatch;
typedef struct Sheet Sheet;
typedef struct MString MString;
typedef struct BufferReader BufferReader;
typedef struct DynamicByteBuffer DynamicByteBuffer;

typedef struct PropGraphicsState
{
	bool mIsSetup;
	Animation mAnimation;
	Sheet* mSheet;
} PropGraphicsState;

typedef struct Prop
{
	bool mIsAdditive;
	int32_t mScaler;
	int32_t mFlipSpeed;
	bool mIsAnimation;
	char mTextureName[EE_FILENAME_MAX];
	char mTilesetFilter[EE_FILENAME_MAX];
	PropGraphicsState mGraphics;
} Prop;

void Prop_Init(Prop* p);
Sheet* Prop_GetSheet(Prop* p);
void Prop_Draw(Prop* p, SpriteBatch* spriteBatch, int32_t depth, Point position, float scale, float rotation, bool flipX, bool flipY, bool showInfo);
void Prop_UpdateResource(Prop* p);
void Prop_LoadDrawing(Prop* p);
Rectangle Prop_GetRectangle(Prop* p);
const char* Prop_ToString(Prop* p);
Animation* Prop_GetAnimation(Prop* p);
void Prop_Read(Prop* p, BufferReader* br);
void Prop_Write(Prop* p, DynamicByteBuffer* dbb);
IStringArray* Prop_GetDirectories(void);
Prop* Prop_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);
void Prop_Dispose(Prop* prop);