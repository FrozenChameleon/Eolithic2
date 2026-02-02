/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "stdint.h"
#include "../render/Color.h"
#include "../math/Vector2.h"
#include "../math/Rectangle.h"

typedef struct Texture Texture;
typedef struct ShaderProgram ShaderProgram;
typedef struct DrawRectangle DrawRectangle;
typedef struct Tile Tile;
typedef struct BmFont BmFont;

typedef struct RenderCommandSheet
{
	uint8_t mType;
	uint8_t mDepth;
	bool mFlipX;
	bool mFlipY;
	bool mIsRectangle;
	bool mIsInterpolated;
	uint16_t mPadding1;
	int32_t mBlendState;
	int32_t mExtraPasses;
	float mRotation;
	Color mColor;
	Vector2 mPosition;
	Vector2 mLastPosition;
	Vector2 mOrigin;
	Vector2 mScale;
	Rectangle mSourceRectangle;
	Rectangle mDestinationRectangle;
	Texture* mTexture;
	ShaderProgram* mShaderProgram;
} RenderCommandSheet;

typedef struct RenderCommandManyRectangle
{
	uint8_t mType;
	uint8_t mDepth;
	uint16_t mPadding1;
	DrawRectangle* mManyRectangles;
	Texture* mTexture;
} RenderCommandManyRectangle;

typedef struct RenderCommandTileLayer
{
	uint8_t mType;
	uint8_t mDepth;
	uint8_t mLayer;
	uint8_t mPadding1;
	int32_t mX1;
	int32_t mX2;
	int32_t mY1;
	int32_t mY2;
	Color mColor;
	Rectangle mTileDataBounds;
	Tile** mTileData;
	Texture* mTexture;
} RenderCommandTileLayer;

typedef struct RenderCommandString
{
	uint8_t mType;
	uint8_t mDepth;
	bool mIsInterpolated;
	bool mIsLockedToInt;
	bool mDoNotReplaceFont;
	uint8_t mPadding1;
	uint16_t mPadding2;
	int16_t mAlignmentX;
	int16_t mAlignmentY;
	Vector2 mPosition;
	Vector2 mLastPosition;
	Color mColor;
	BmFont* mFont;
	const char* mString;
} RenderCommandString;