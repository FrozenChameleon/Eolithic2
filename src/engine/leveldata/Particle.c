/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "Particle.h"
#include "../utils/Utils.h"
#include "../io/BufferReader.h"
#include "../io/DynamicByteBuffer.h"

#define VERSION_PARTICLE 7

void Particle_Read(Particle* p, BufferReader* br)
{
	if (!BufferReader_ReadMagicNumber(br))
	{
		return;
	}
	BufferReader_ReadVersionNumber(br);

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
void Particle_Write(Particle* p, DynamicByteBuffer* dbb)
{
	DynamicByteBuffer_WriteMagicNumber(dbb);
	DynamicByteBuffer_WriteVersionNumber(dbb, VERSION_PARTICLE);

	DynamicByteBuffer_WriteI32(dbb, p->mExtraPasses);
	DynamicByteBuffer_WriteBoolean(dbb, p->mIsBlendStateAdditive);

	DynamicByteBuffer_WriteI32(dbb, p->mMiscTTLMin);
	DynamicByteBuffer_WriteI32(dbb, p->mMiscTTLMax);
	DynamicByteBuffer_WriteBoolean(dbb, p->mMiscDoesDieOffscreen);

	DynamicByteBuffer_WriteFloat(dbb, p->mOffsetMinimum.X);
	DynamicByteBuffer_WriteFloat(dbb, p->mOffsetMinimum.Y);
	DynamicByteBuffer_WriteFloat(dbb, p->mOffsetMaximum.X);
	DynamicByteBuffer_WriteFloat(dbb, p->mOffsetMaximum.Y);
	DynamicByteBuffer_WriteBoolean(dbb, p->mOffsetIsNegativeX);
	DynamicByteBuffer_WriteBoolean(dbb, p->mOffsetIsNegativeY);

	DynamicByteBuffer_WriteBoolean(dbb, p->mFlickerIsFlicker);
	DynamicByteBuffer_WriteI32(dbb, p->mFlickerMinimumStart);
	DynamicByteBuffer_WriteI32(dbb, p->mFlickerMaximumStart);
	DynamicByteBuffer_WriteI32(dbb, p->mFlickerMinimumSpeed);
	DynamicByteBuffer_WriteI32(dbb, p->mFlickerMaximumSpeed);

	DynamicByteBuffer_WriteFloat(dbb, p->mConstantMinimum.X);
	DynamicByteBuffer_WriteFloat(dbb, p->mConstantMinimum.Y);
	DynamicByteBuffer_WriteFloat(dbb, p->mConstantMaximum.X);
	DynamicByteBuffer_WriteFloat(dbb, p->mConstantMaximum.Y);
	DynamicByteBuffer_WriteBoolean(dbb, p->mConstantIsNegativeX);
	DynamicByteBuffer_WriteBoolean(dbb, p->mConstantIsNegativeY);
	DynamicByteBuffer_WriteBoolean(dbb, p->mConstantIsFluctuating);
	DynamicByteBuffer_WriteI32(dbb, p->mConstantFluctuateIntervalMin);
	DynamicByteBuffer_WriteI32(dbb, p->mConstantFluctuateIntervalMax);

	DynamicByteBuffer_WriteFloat(dbb, p->mCurveTotalMinimum.X);
	DynamicByteBuffer_WriteFloat(dbb, p->mCurveTotalMinimum.Y);
	DynamicByteBuffer_WriteFloat(dbb, p->mCurveTotalMaximum.X);
	DynamicByteBuffer_WriteFloat(dbb, p->mCurveTotalMaximum.Y);
	DynamicByteBuffer_WriteFloat(dbb, p->mCurveMinimumIncreaseRate.X);
	DynamicByteBuffer_WriteFloat(dbb, p->mCurveMinimumIncreaseRate.Y);
	DynamicByteBuffer_WriteFloat(dbb, p->mCurveMaximumIncreaseRate.X);
	DynamicByteBuffer_WriteFloat(dbb, p->mCurveMaximumIncreaseRate.Y);
	DynamicByteBuffer_WriteBoolean(dbb, p->mCurveIsOppositeX);
	DynamicByteBuffer_WriteBoolean(dbb, p->mCurveIsOppositeY);
	DynamicByteBuffer_WriteBoolean(dbb, p->mCurveIsNegativeX);
	DynamicByteBuffer_WriteBoolean(dbb, p->mCurveIsNegativeY);
	DynamicByteBuffer_WriteBoolean(dbb, p->mCurveIsFluctuating);
	DynamicByteBuffer_WriteBoolean(dbb, p->mCurveFluctuateKeepSpeed);
	DynamicByteBuffer_WriteBoolean(dbb, p->mCurveFluctuateResetRate);
	DynamicByteBuffer_WriteBoolean(dbb, p->mCurveFluctuateResetTotal);
	DynamicByteBuffer_WriteI32(dbb, p->mCurveFluctuateIntervalMin);
	DynamicByteBuffer_WriteI32(dbb, p->mCurveFluctuateIntervalMax);

	DynamicByteBuffer_WriteI32(dbb, p->mTextureDepth);
	DynamicByteBuffer_WriteString(dbb, p->mTextureName, EE_FILENAME_MAX);
	DynamicByteBuffer_WriteI32(dbb, p->mTextureFlipSpeed);
	DynamicByteBuffer_WriteBoolean(dbb, p->mTextureIsAnimation);
	DynamicByteBuffer_WriteI32(dbb, p->mScaler);
	DynamicByteBuffer_WriteI32(dbb, p->mTextureLoopPoint);
	DynamicByteBuffer_WriteBoolean(dbb, p->mTextureDoesAnimationStop);

	DynamicByteBuffer_WriteI32(dbb, p->mPreviewAmount);
	DynamicByteBuffer_WriteI32(dbb, p->mPreviewInterval);
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