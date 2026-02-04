/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "Prop.h"
#include "../math/Vector2.h"
#include "../utils/MString.h"

typedef struct BufferReader BufferReader;
typedef struct DynamicByteBuffer DynamicByteBuffer;
typedef struct SpriteBatch SpriteBatch;

typedef struct PropInstance
{
	Prop* INTERNAL_mCachedPropData;
	int32_t mDepth;
	float mScale;
	float mRotation;
	bool mFlipX;
	bool mFlipY;
	char mName[EE_FILENAME_MAX];
	Vector2 mOffset;
	Vector2 mDrawOffset;
} PropInstance;

void PropInstance_Init(PropInstance* prop);
void PropInstance_Write(PropInstance* prop, DynamicByteBuffer* dbb);
void PropInstance_Read(int32_t version, PropInstance* prop, BufferReader* reader);
void PropInstance_Draw(PropInstance* prop, SpriteBatch* spriteBatch, Vector2 position);
void PropInstance_Draw2(PropInstance* prop, SpriteBatch* spriteBatch, Vector2 position, bool drawInfo);
void PropInstance_Draw3(PropInstance* prop, SpriteBatch* spriteBatch, int32_t depth, Vector2 position);
void PropInstance_Draw4(PropInstance* prop, SpriteBatch* spriteBatch, int32_t depth, Vector2 position, bool drawInfo);
Prop* PropInstance_GetPropData(PropInstance* prop);
bool PropInstance_IsPropValid(PropInstance* prop);
Rectangle PropInstance_GetRectangle(PropInstance* prop, Vector2 position);
bool PropInstance_IsPropActuallyTouched(PropInstance* prop, Point relativeMouse);
bool PropInstance_IsEqualTo(PropInstance* prop, PropInstance* instance);