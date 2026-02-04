/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#include "LevelCameraData.h"

#include "limits.h"
#include "float.h"
#include "../render/Color.h"
#include "../io/BufferReader.h"
//#include "MetaMapChunk.h"
#include "../render/SpriteBatch.h"
#include "../math/PointRectangle.h"

#define DEFAULT_STR_NOT_SET "NOT_SET"

PointRectangle* LevelCameraData_GetActiveBoundsPointRectangle(LevelCameraData* lcd)
{
	if (LevelCameraData_HasBounds(lcd))
	{
		return &lcd->mVolumeBounds;
	}
	else
	{
		return &lcd->mVolumeTrigger;
	}
}
bool LevelCameraData_HasBounds(LevelCameraData* lcd)
{
	return !PointRectangle_IsEmpty(&lcd->mVolumeBounds);
}
void LevelCameraData_Read(int32_t version, LevelCameraData* lcd, BufferReader* br)
{
	PointRectangle_Read(&lcd->mVolumeTrigger, br);
	PointRectangle_Read(&lcd->mVolumeBounds, br);
	lcd->mDirectionX =  BufferReader_ReadI32(br);
	lcd->mDirectionY = BufferReader_ReadI32(br);
	lcd->mIsPersistent = BufferReader_ReadBoolean(br);
	lcd->mIsDualX = BufferReader_ReadBoolean(br);
	lcd->mIsDualY = BufferReader_ReadBoolean(br);
	lcd->mIsUpFrozen = BufferReader_ReadBoolean(br);
	lcd->mIsRightFrozen = BufferReader_ReadBoolean(br);
	lcd->mIsDownFrozen = BufferReader_ReadBoolean(br);
	lcd->mIsLeftFrozen = BufferReader_ReadBoolean(br);
	BufferReader_ReadString(br, lcd->mScript, EE_FILENAME_MAX);
	lcd->mId = BufferReader_ReadI32(br);
	lcd->mIsTransition = BufferReader_ReadBoolean(br);
	lcd->mNumberTransitionTo = BufferReader_ReadI32(br);
	lcd->mNumberTransitionFrom = BufferReader_ReadI32(br);
	lcd->mIsTransitionX = BufferReader_ReadBoolean(br);
	lcd->mIsTransitionY = BufferReader_ReadBoolean(br);
	for (int32_t i = 0; i < LEVELCAMERADATA_STRING_DATA_LIMIT; i += 1)
	{
		BufferReader_ReadString(br, lcd->mStringData[i], EE_FILENAME_MAX);
	}
}
void LevelCameraData_Write(LevelCameraData* lcd, DynamicByteBuffer* dbb)
{
	PointRectangle_Write(&lcd->mVolumeTrigger, dbb);
	PointRectangle_Write(&lcd->mVolumeBounds, dbb);
	DynamicByteBuffer_WriteI32(dbb, lcd->mDirectionX);
	DynamicByteBuffer_WriteI32(dbb, lcd->mDirectionY);
	DynamicByteBuffer_WriteBoolean(dbb, lcd->mIsPersistent);
	DynamicByteBuffer_WriteBoolean(dbb, lcd->mIsDualX);
	DynamicByteBuffer_WriteBoolean(dbb, lcd->mIsDualY);
	DynamicByteBuffer_WriteBoolean(dbb, lcd->mIsUpFrozen);
	DynamicByteBuffer_WriteBoolean(dbb, lcd->mIsRightFrozen);
	DynamicByteBuffer_WriteBoolean(dbb, lcd->mIsDownFrozen);
	DynamicByteBuffer_WriteBoolean(dbb, lcd->mIsLeftFrozen);
	DynamicByteBuffer_WriteString(dbb, lcd->mScript, EE_FILENAME_MAX);
	DynamicByteBuffer_WriteI32(dbb, lcd->mId); //v2
	DynamicByteBuffer_WriteBoolean(dbb, lcd->mIsTransition);
	DynamicByteBuffer_WriteI32(dbb, lcd->mNumberTransitionTo);
	DynamicByteBuffer_WriteI32(dbb, lcd->mNumberTransitionFrom);
	DynamicByteBuffer_WriteBoolean(dbb, lcd->mIsTransitionX);
	DynamicByteBuffer_WriteBoolean(dbb, lcd->mIsTransitionY);
	for (int32_t i = 0; i < LEVELCAMERADATA_STRING_DATA_LIMIT; i += 1) //v4
	{
		DynamicByteBuffer_WriteString(dbb, lcd->mStringData[i], EE_FILENAME_MAX);
	}
}
