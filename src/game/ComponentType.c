//THIS CODE IS AUTOMATICALLY GENERATED, DO NOT EDIT!

#include "ComponentType.h"

const char* ComponentType_GetComponentName(ComponentType ctype)
{
    switch (ctype)
    {
    case C_Player:
		return "Player";
    case C_Body:
        return "Body";
    case C_CollisionEngine:
        return "CollisionEngine";
    case C_BounceData:
        return "BounceData";
    case C_Camera:
        return "Camera";
    case C_CollisionImprintData:
        return "CollisionImprintData";
    case C_DrawActor:
        return "DrawActor";
    case C_FakePosition:
        return "FakePosition";
    case C_FloatyMovementData:
        return "FloatyMovementData";
    case C_FreezeEntityTillOnScreen:
        return "FreezeEntityTillOnScreen";
    case C_GridPosition:
        return "GridPosition";
    case C_IdleCircleData:
        return "IdleCircleData";
    case C_InitialPosition:
        return "InitialPosition";
    case C_LevelDataStub:
        return "LevelDataStub";
    case C_LevelFrameCounter:
        return "LevelFrameCounter";
    case C_Name:
        return "Name";
    case C_Nodes:
        return "Nodes";
    case C_ParentNumber:
        return "ParentNumber";
    case C_PlayerNumber:
        return "PlayerNumber";
    case C_ShakeCamera:
        return "ShakeCamera";
    case C_SpecialMcDeathEffect:
        return "SpecialMcDeathEffect";
    case C_SpecialMcFinalBossEffect:
        return "SpecialMcFinalBossEffect";
    case C_Step:
        return "Step";
    case C_StepCounter:
        return "StepCounter";
    case C_StepTimer:
        return "StepTimer";
    case C_StringSettings:
        return "StringSettings";
    case C_DrawRenderInfo:
        return "DrawRenderInfo";
    case C_DrawStateInfo:
        return "DrawStateInfo";
    case C_ParticleInstance:
        return "ParticleInstance";
    case C_TagIsActor:
        return "TagIsActor";
    case C_TagIsComplete:
        return "TagIsComplete";
    case C_TagIsDrawDisabled:
        return "TagIsDrawDisabled";
    case C_TagIsInPlay:
        return "TagIsInPlay";
    case C_TagIsPlayer:
        return "TagIsPlayer";
    case C_TagIsUpdateDisabled:
        return "TagIsUpdateDisabled";
    case C_ShakeFOverTimeData:
        return "ShakeFOverTimeData";
    default:
        return "NOT_A_VALID_COMPONENT";
    }
}
