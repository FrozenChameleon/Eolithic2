/* Eolithic2
 * Copyright 2025-2026 Patrick Derosby
 * Released under the zlib License.
 * See eolithic2.LICENSE for details.
 */
 
//THIS CODE IS AUTOMATICALLY GENERATED, DO NOT EDIT!

#pragma once

#define COMPONENT_TYPE_UNUSED_COMPONENT_RANGE 20000
#define COMPONENT_TYPE_LEN 377

typedef enum ComponentType
{
    C_Player = 1,
    C_Body = 85,
    C_CollisionEngine = 86,
    C_BounceData = 87,
    C_Camera = 88,
    C_CollisionImprintData = 89,
    C_DrawActor = 90,
    C_FakePosition = 91,
    C_FloatyMovementData = 92,
    C_FreezeEntityTillOnScreen = 93,
    C_GridPosition = 94,
    C_IdleCircleData = 95,
    C_InitialPosition = 96,
    C_LevelDataStub = 97,
    C_LevelFrameCounter = 98,
    C_Name = 99,
    C_Nodes = 100,
    C_ParentNumber = 101,
    C_PlayerNumber = 102,
    C_ShakeCamera = 103,
    C_SpecialMcDeathEffect = 104,
    C_SpecialMcFinalBossEffect = 105,
    C_Step = 106,
    C_StepCounter = 107,
    C_StepTimer = 108,
    C_StringSettings = 109,
    C_DrawRenderInfo = 110,
    C_DrawStateInfo = 111,
    C_ParticleInstance = 112,
    C_TagIsActor = 113,
    C_TagIsComplete = 114,
    C_TagIsDrawDisabled = 115,
    C_TagIsInPlay = 116,
    C_TagIsPlayer = 117,
    C_TagIsUpdateDisabled = 118,
    C_ShakeFOverTimeData = 119,
    C_MirrorEffect = 20000,
    C_SceneCameraData = 20001,
    C_TagIsIgnoringTransitions = 20002,
    C_TagIsExtraBody = 20003,
    C_TagIsEnemy = 20004,
    C_TagIsBlockingLineOfSight = 20005,
    C_StunFrames = 20006,
    C_Random32 = 20007,
    C_NodeMovingKit = 20008,
    C_MoveGetter = 20009,
    C_Move = 20010,
    C_IntTag = 20011,
    C_HitFlashKit = 20012,
    C_DrawLineOfSight = 20013,
} ComponentType;

const char* ComponentType_GetComponentName(ComponentType ctype);