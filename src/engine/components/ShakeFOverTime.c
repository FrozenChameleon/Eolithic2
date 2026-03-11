#include "ShakeFOverTime.h"

#include "../core/Func.h"
#include "../utils/Utils.h"
#include "../utils/Logger.h"

System* ShakeFOverTime_CreateSystem()
{
	SystemSimple* ss = SystemSimple_Create(C_ShakeFOverTimeData);
    ss->_mUpdateRoutine = (SystemSimple_UpdateFunc)ShakeFOverTime_UpdateRoutine;
	return SystemSimple_CreateSystem(ss);
}

void ShakeFOverTime_UpdateRoutine(Entity owner, ShakeFOverTimeData* data)
{
    Entity entity = ENTITY_NOTHING;
    if ((data->mType == SHAKEFOVERTIME_TYPE_ENTITY_SINGLE_STATE) || (data->mType == SHAKEFOVERTIME_TYPE_ENTITY_UNIVERSAL))
    {
        entity = Get_Entity(data->mEntityNumber);
        if (entity == ENTITY_NOTHING)
        {
            Do_SetComplete(owner);
            return;
        }
    }

    if (data->mFramesToShake <= 0)
    {
        switch (data->mType)
        {
        case SHAKEFOVERTIME_TYPE_ENTITY_SINGLE_STATE:
            Do_ClearShake2(entity, data->mState);
            break;
        case SHAKEFOVERTIME_TYPE_ENTITY_UNIVERSAL:
            Do_ClearShake(entity);
            break;
        }

        Do_SetComplete(owner);
        return;
    }

    switch (data->mType)
    {
    case SHAKEFOVERTIME_TYPE_ENTITY_SINGLE_STATE:
        INTERNAL_Do_ShakeStateF(entity, data->mState, data->mMinX, data->mMaxX, data->mMinY, data->mMaxY, data->mIslockedToInteger, data->mIsRandomDirection);
        break;
    case SHAKEFOVERTIME_TYPE_ENTITY_UNIVERSAL:
        INTERNAL_Do_ShakeF(entity, data->mMinX, data->mMaxX, data->mMinY, data->mMaxY, data->mIslockedToInteger, data->mIsRandomDirection);
        break;
    case SHAKEFOVERTIME_TYPE_CAMERA:
        INTERNAL_Do_ShakeCameraF(data->mMinX, data->mMaxX, data->mMinY, data->mMaxY, data->mIslockedToInteger, data->mIsRandomDirection);
        break;
    }

    data->mFramesToShake -= 1;
}

void ShakeFOverTime_Create(int type, Entity entityToShake, const char* state, float minX, float maxX, float minY, float maxY, 
    bool isLockedToInteger, bool isRandomDirection, int framesToShake)
{
    if (framesToShake <= 0)
    {
        Logger_Log(LOGGER_WARNING, "Invalid frames to shake for ShakeFOverTime");
        return;
    }

    Entity entityToUse = Do_BuildNewEntityWithName("ShakeFOverTime");

	ShakeFOverTimeData* data = (ShakeFOverTimeData*)Get_Component(C_ShakeFOverTimeData, entityToUse);
	Utils_memset(data, 0, sizeof(ShakeFOverTimeData)); //Reset this no matter what...
    data->mType = type;
    data->mEntityNumber = entityToShake;
	Utils_strlcpy(data->mState, state, EE_FILENAME_MAX);
    data->mMinX = minX;
    data->mMaxX = maxX;
    data->mMinY = minY;
    data->mMaxY = maxY;
    data->mIslockedToInteger = isLockedToInteger;
    data->mIsRandomDirection = isRandomDirection;
    data->mFramesToShake = framesToShake;
}
