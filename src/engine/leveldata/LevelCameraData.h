/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */

#pragma once

#include "MetaMapChunk.h"
#include "../math/PointRectangle.h"
#include "../utils/MString.h"

#define LEVELCAMERADATA_STRING_DATA_LIMIT 10

typedef struct SpriteBatch SpriteBatch;
typedef struct BufferReader BufferReader;
typedef struct DynamicByteBuffer DynamicByteBuffer;

typedef struct LevelCameraData
{
	MetaMapChunk* mAttachedChunk;
	PointRectangle mVolumeTrigger;
	PointRectangle mVolumeBounds;
	int32_t mDirectionX;
	int32_t mDirectionY;
	bool mIsPersistent;
	bool mIsDualX;
	bool mIsDualY;
	bool mIsUpFrozen;
	bool mIsRightFrozen;
	bool mIsDownFrozen;
	bool mIsLeftFrozen;
	char mScript[EE_FILENAME_MAX];
	int32_t mId;
	bool mIsTransition;
	int32_t mNumberTransitionTo;
	int32_t mNumberTransitionFrom;
	bool mIsTransitionX;
	bool mIsTransitionY;
	char mStringData[LEVELCAMERADATA_STRING_DATA_LIMIT][EE_FILENAME_MAX];
} LevelCameraData;

bool LevelCameraData_HasBounds(LevelCameraData* lcd);
PointRectangle* LevelCameraData_GetActiveBoundsPointRectangle(LevelCameraData* lcd);
void LevelCameraData_Read(int32_t version, LevelCameraData* lcd, BufferReader* br);
void LevelCameraData_Write(LevelCameraData* lcd, DynamicByteBuffer* dbb);