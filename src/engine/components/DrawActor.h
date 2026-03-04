/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../math/Vector2.h"
#include "../render/Color.h"

typedef struct ShaderProgram ShaderProgram;

typedef struct DrawActor
{
	int32_t mExtraPasses;
	char mDefaultDrawState[EE_FILENAME_MAX];
	char mDefaultDrawPhase[EE_FILENAME_MAX];
	int32_t mUniversalDepthOverride;
	float mRotation;
	float mOnScreenCheckMul;
	bool mIsFlipX;
	bool mIsFlipY;
	bool mIsBlendStateAdditive;
	bool mIsOnScreen;
	bool mIsUpdateDisabled;
	Vector2 mRenderPosition;
	Vector2 mLastRenderPosition;
	Vector2 mUniversalNudge;
	Vector2 mScale;
	Vector2 mOffset;
	Color mTintColor;
	ShaderProgram* mShaderProgram;
} DrawActor;

