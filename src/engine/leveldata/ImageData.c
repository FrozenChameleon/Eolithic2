/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "ImageData.h"

#include "../math/Vectors.h"
#include "../utils/Utils.h"
#include "../io/BufferReader.h"
#include "../io/DynamicByteBuffer.h"

void ImageData_Init(ImageData* id)
{
	Utils_memset(id, 0, sizeof(ImageData));

	id->mOrigin = Vectors_NegativeOne;
	id->mDepth = 80;
	id->mScaler = 1;
}

void ImageData_Read(ImageData* id, BufferReader* br)
{
	BufferReader_ReadString(br, id->mImage, EE_FILENAME_MAX);
	id->mIsAdditive = BufferReader_ReadBoolean(br);
	id->mOrigin.X = BufferReader_ReadFloat(br);
	id->mOrigin.Y = BufferReader_ReadFloat(br);
	id->mOffset.X = BufferReader_ReadI32(br);
	id->mOffset.Y = BufferReader_ReadI32(br);
	id->mAnimationLoopPoint = BufferReader_ReadI32(br);
	id->mCanAnimate = BufferReader_ReadBoolean(br);
	id->mAnimationStops = BufferReader_ReadBoolean(br);
	id->mAnimationBlanks = BufferReader_ReadBoolean(br);
	id->mFlipSpeed = BufferReader_ReadI32(br);
	id->mDepth = BufferReader_ReadI32(br);
	id->mInitialRotation = BufferReader_ReadFloat(br);
	id->mScaler = BufferReader_ReadI32(br);
}
void ImageData_Write(ImageData* id, DynamicByteBuffer* dbb)
{
	DynamicByteBuffer_WriteString(dbb, id->mImage, EE_FILENAME_MAX);
	DynamicByteBuffer_WriteBoolean(dbb, id->mIsAdditive);
	DynamicByteBuffer_WriteFloat(dbb, id->mOrigin.X);
	DynamicByteBuffer_WriteFloat(dbb, id->mOrigin.Y);
	DynamicByteBuffer_WriteI32(dbb, id->mOffset.X);
	DynamicByteBuffer_WriteI32(dbb, id->mOffset.Y);
	DynamicByteBuffer_WriteI32(dbb, id->mAnimationLoopPoint);
	DynamicByteBuffer_WriteBoolean(dbb, id->mCanAnimate);
	DynamicByteBuffer_WriteBoolean(dbb, id->mAnimationStops);
	DynamicByteBuffer_WriteBoolean(dbb, id->mAnimationBlanks);
	DynamicByteBuffer_WriteI32(dbb, id->mFlipSpeed);
	DynamicByteBuffer_WriteI32(dbb, id->mDepth);
	DynamicByteBuffer_WriteFloat(dbb, id->mInitialRotation);
	DynamicByteBuffer_WriteI32(dbb, id->mScaler);
}
const char* ImageData_ToString(ImageData* id)
{
	return id->mImage;
}
bool ImageData_IsOriginSet(ImageData* id)
{
	if (Vector2_EqualTo(id->mOrigin, Vectors_NegativeOne))
	{
		return false;
	}
	return true;
}
