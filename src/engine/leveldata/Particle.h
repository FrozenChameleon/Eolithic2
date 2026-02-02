/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "../utils/Macros.h"
#include "../math/Vector2.h"
#include "../utils/MString.h"
#include "../io/BufferReader.h"

typedef struct Particle
{
	bool mIsBlendStateAdditive;
	int32_t mExtraPasses;

	int32_t mMiscTTLMin;
	int32_t mMiscTTLMax;
	bool mMiscDoesDieOffscreen;

	Vector2 mOffsetMinimum;
	Vector2 mOffsetMaximum;
	bool mOffsetIsNegativeX;
	bool mOffsetIsNegativeY;

	bool mFlickerIsFlicker;
	int32_t mFlickerMinimumStart;
	int32_t mFlickerMaximumStart;
	int32_t mFlickerMinimumSpeed;
	int32_t mFlickerMaximumSpeed;

	Vector2 mConstantMinimum;
	Vector2 mConstantMaximum;
	bool mConstantIsNegativeX;
	bool mConstantIsNegativeY;
	bool mConstantIsFluctuating;
	int32_t mConstantFluctuateIntervalMin;
	int32_t mConstantFluctuateIntervalMax;

	Vector2 mCurveTotalMinimum;
	Vector2 mCurveTotalMaximum;
	Vector2 mCurveMinimumIncreaseRate;
	Vector2 mCurveMaximumIncreaseRate;
	bool mCurveIsOppositeX;
	bool mCurveIsOppositeY;
	bool mCurveIsNegativeX;
	bool mCurveIsNegativeY;
	bool mCurveIsFluctuating;
	bool mCurveFluctuateKeepSpeed;
	bool mCurveFluctuateResetRate;
	bool mCurveFluctuateResetTotal;
	int32_t mCurveFluctuateIntervalMin;
	int32_t mCurveFluctuateIntervalMax;

	int32_t mTextureDepth;
	char mTextureName[EE_FILENAME_MAX];
	int32_t mTextureFlipSpeed;
	bool mTextureIsAnimation;
	int32_t mScaler;
	int32_t mTextureLoopPoint;
	bool mTextureDoesAnimationStop;

	int32_t mPreviewAmount;
	int32_t mPreviewInterval;
} Particle;

void Particle_Read(Particle* p, BufferReader* br);
Particle* Particle_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br);
void Particle_Dispose(Particle* p);
