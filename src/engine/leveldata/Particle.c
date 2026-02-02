/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Particle.h"
#include "../utils/Utils.h"

void Particle_Read(Particle* p, BufferReader* br)
{
	p->mExtraPasses = BufferReader_ReadI32(br);
	p->mIsBlendStateAdditive = BufferReader_ReadBoolean(br);

	p->mMiscTTLMin = BufferReader_ReadI32(br);
	p->mMiscTTLMax = BufferReader_ReadI32(br);
	p->mMiscDoesDieOffscreen = BufferReader_ReadBoolean(br);

	p->mOffsetMinimum.X = BufferReader_ReadFloat(br);
	p->mOffsetMinimum.Y = BufferReader_ReadFloat(br);
	p->mOffsetMaximum.X = BufferReader_ReadFloat(br);
	p->mOffsetMaximum.Y = BufferReader_ReadFloat(br);
	p->mOffsetIsNegativeX = BufferReader_ReadBoolean(br);
	p->mOffsetIsNegativeY = BufferReader_ReadBoolean(br);

	p->mFlickerIsFlicker = BufferReader_ReadBoolean(br);
	p->mFlickerMinimumStart = BufferReader_ReadI32(br);
	p->mFlickerMaximumStart = BufferReader_ReadI32(br);
	p->mFlickerMinimumSpeed = BufferReader_ReadI32(br);
	p->mFlickerMaximumSpeed = BufferReader_ReadI32(br);

	p->mConstantMinimum.X = BufferReader_ReadFloat(br);
	p->mConstantMinimum.Y = BufferReader_ReadFloat(br);
	p->mConstantMaximum.X = BufferReader_ReadFloat(br);
	p->mConstantMaximum.Y = BufferReader_ReadFloat(br);
	p->mConstantIsNegativeX = BufferReader_ReadBoolean(br);
	p->mConstantIsNegativeY = BufferReader_ReadBoolean(br);
	p->mConstantIsFluctuating = BufferReader_ReadBoolean(br);
	p->mConstantFluctuateIntervalMin = BufferReader_ReadI32(br);
	p->mConstantFluctuateIntervalMax = BufferReader_ReadI32(br);

	p->mCurveTotalMinimum.X = BufferReader_ReadFloat(br);
	p->mCurveTotalMinimum.Y = BufferReader_ReadFloat(br);
	p->mCurveTotalMaximum.X = BufferReader_ReadFloat(br);
	p->mCurveTotalMaximum.Y = BufferReader_ReadFloat(br);
	p->mCurveMinimumIncreaseRate.X = BufferReader_ReadFloat(br);
	p->mCurveMinimumIncreaseRate.Y = BufferReader_ReadFloat(br);
	p->mCurveMaximumIncreaseRate.X = BufferReader_ReadFloat(br);
	p->mCurveMaximumIncreaseRate.Y = BufferReader_ReadFloat(br);
	p->mCurveIsOppositeX = BufferReader_ReadBoolean(br);
	p->mCurveIsOppositeY = BufferReader_ReadBoolean(br);
	p->mCurveIsNegativeX = BufferReader_ReadBoolean(br);
	p->mCurveIsNegativeY = BufferReader_ReadBoolean(br);
	p->mCurveIsFluctuating = BufferReader_ReadBoolean(br);
	p->mCurveFluctuateKeepSpeed = BufferReader_ReadBoolean(br);
	p->mCurveFluctuateResetRate = BufferReader_ReadBoolean(br);
	p->mCurveFluctuateResetTotal = BufferReader_ReadBoolean(br);
	p->mCurveFluctuateIntervalMin = BufferReader_ReadI32(br);
	p->mCurveFluctuateIntervalMax = BufferReader_ReadI32(br);

	p->mTextureDepth = BufferReader_ReadI32(br);
	BufferReader_ReadString(br, p->mTextureName, EE_FILENAME_MAX);
	p->mTextureFlipSpeed = BufferReader_ReadI32(br);
	p->mTextureIsAnimation = BufferReader_ReadBoolean(br);
	p->mScaler = BufferReader_ReadI32(br);
	p->mTextureLoopPoint = BufferReader_ReadI32(br);
	p->mTextureDoesAnimationStop = BufferReader_ReadBoolean(br);

	p->mPreviewAmount = BufferReader_ReadI32(br);
	p->mPreviewInterval = BufferReader_ReadI32(br);
}

Particle* Particle_FromStream(const char* path, const char* filenameWithoutExtension, BufferReader* br)
{
	Particle* p = (Particle*)Utils_calloc(1, sizeof(Particle));
	Particle_Read(p, br);
	return p;
}

void Particle_Dispose(Particle* p)
{
	Utils_free(p);
}
