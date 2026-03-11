#pragma once

#include "../utils/Macros.h"
#include "../systems/SystemSimple.h"

typedef struct ShakeFOverTimeData
{
    int mFramesToShake;
    int mType;
    int mEntityNumber;
	char mState[EE_FILENAME_MAX];
    float mMinX;
    float mMaxX;
    float mMinY;
    float mMaxY;
	bool mIslockedToInteger;
    bool mIsRandomDirection;
} ShakeFOverTimeData;

#define SHAKEFOVERTIME_TYPE_ENTITY_SINGLE_STATE 0
#define SHAKEFOVERTIME_TYPE_ENTITY_UNIVERSAL 1
#define SHAKEFOVERTIME_TYPE_CAMERA 2

System* ShakeFOverTime_CreateSystem();

void ShakeFOverTime_UpdateRoutine(Entity owner, ShakeFOverTimeData* data);
void ShakeFOverTime_Create(int type, Entity entityToShake, const char* state, float minX, float maxX, float minY, float maxY,
    bool isLockedToInteger, bool isRandomDirection, int framesToShake);