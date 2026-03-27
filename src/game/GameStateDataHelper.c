//THIS CODE IS AUTOMATICALLY GENERATED, DO NOT EDIT!

#include "../engine/gamestate/GameStateDataHelper.h"
#include "ComponentType.h"

#include "Player.h"
#include "../engine/collision/Body.h"
#include "../engine/collision/CollisionEngine.h"
#include "../engine/components/BounceData.h"
#include "../engine/components/Camera.h"
#include "../engine/components/CollisionImprintData.h"
#include "../engine/components/DrawActor.h"
#include "../engine/components/FakePosition.h"
#include "../engine/components/FloatyMovementData.h"
#include "../engine/components/FreezeEntityTillOnScreen.h"
#include "../engine/components/GridPosition.h"
#include "../engine/components/IdleCircleData.h"
#include "../engine/components/InitialPosition.h"
#include "../engine/components/LevelDataStub.h"
#include "../engine/components/LevelFrameCounter.h"
#include "../engine/components/Name.h"
#include "../engine/components/Nodes.h"
#include "../engine/components/ParentNumber.h"
#include "../engine/components/PlayerNumber.h"
#include "../engine/components/ShakeCamera.h"
#include "../engine/components/SpecialMcDeathEffect.h"
#include "../engine/components/SpecialMcFinalBossEffect.h"
#include "../engine/components/Step.h"
#include "../engine/components/StepCounter.h"
#include "../engine/components/StepTimer.h"
#include "../engine/components/StringSettings.h"
#include "../engine/render/DrawRenderInfo.h"
#include "../engine/render/DrawStateInfo.h"
#include "../engine/leveldata/ParticleInstance.h"
#include "../engine/components/TagIsActor.h"
#include "../engine/components/TagIsComplete.h"
#include "../engine/components/TagIsDrawDisabled.h"
#include "../engine/components/TagIsInPlay.h"
#include "../engine/components/TagIsPlayer.h"
#include "../engine/components/TagIsUpdateDisabled.h"
#include "../engine/components/ShakeFOverTime.h"

#define GSD_INITIAL_SIZE 1

void GameStateDataHelper_InitAllComponentPacks(GameStateData* gsd)
{
    ComponentPack_Init(C_Player, &gsd->mComponentPacks[C_Player], sizeof(Body), 4);
    ComponentPack_Init(C_Body, &gsd->mComponentPacks[C_Body], sizeof(Body), 1024);
    ComponentPack_Init(C_CollisionEngine, &gsd->mComponentPacks[C_CollisionEngine], sizeof(CollisionEngine), 1);
    ComponentPack_Init(C_BounceData, &gsd->mComponentPacks[C_BounceData], sizeof(BounceData), 1);
    ComponentPack_Init(C_Camera, &gsd->mComponentPacks[C_Camera], sizeof(Camera), 1);
    ComponentPack_Init(C_CollisionImprintData, &gsd->mComponentPacks[C_CollisionImprintData], sizeof(CollisionImprintData), 512);
    ComponentPack_Init(C_DrawActor, &gsd->mComponentPacks[C_DrawActor], sizeof(DrawActor), 256);
    ComponentPack_Init(C_FakePosition, &gsd->mComponentPacks[C_FakePosition], sizeof(FakePosition), 1024);
    ComponentPack_Init(C_FloatyMovementData, &gsd->mComponentPacks[C_FloatyMovementData], sizeof(FloatyMovementData), 4);
    ComponentPack_Init(C_FreezeEntityTillOnScreen, &gsd->mComponentPacks[C_FreezeEntityTillOnScreen], sizeof(FreezeEntityTillOnScreen), 256);
    ComponentPack_Init(C_GridPosition, &gsd->mComponentPacks[C_GridPosition], sizeof(GridPosition), 1024);
    ComponentPack_Init(C_IdleCircleData, &gsd->mComponentPacks[C_IdleCircleData], sizeof(IdleCircleData), 4);
    ComponentPack_Init(C_InitialPosition, &gsd->mComponentPacks[C_InitialPosition], sizeof(InitialPosition), 1024);
    ComponentPack_Init(C_LevelDataStub, &gsd->mComponentPacks[C_LevelDataStub], sizeof(LevelDataStub), 1);
    ComponentPack_Init(C_LevelFrameCounter, &gsd->mComponentPacks[C_LevelFrameCounter], sizeof(LevelFrameCounter), 1);
    ComponentPack_Init(C_Name, &gsd->mComponentPacks[C_Name], sizeof(Name), 1024);
    ComponentPack_Init(C_Nodes, &gsd->mComponentPacks[C_Nodes], sizeof(Nodes), 1024);
    ComponentPack_Init(C_ParentNumber, &gsd->mComponentPacks[C_ParentNumber], sizeof(ParentNumber), 1024);
    ComponentPack_Init(C_PlayerNumber, &gsd->mComponentPacks[C_PlayerNumber], sizeof(PlayerNumber), 1);
    ComponentPack_Init(C_ShakeCamera, &gsd->mComponentPacks[C_ShakeCamera], sizeof(ShakeCamera), 8);
    ComponentPack_Init(C_SpecialMcDeathEffect, &gsd->mComponentPacks[C_SpecialMcDeathEffect], sizeof(SpecialMcDeathEffect), 1);
    ComponentPack_Init(C_SpecialMcFinalBossEffect, &gsd->mComponentPacks[C_SpecialMcFinalBossEffect], sizeof(SpecialMcFinalBossEffect), 1);
    ComponentPack_Init(C_Step, &gsd->mComponentPacks[C_Step], sizeof(Step), 32);
    ComponentPack_Init(C_StepCounter, &gsd->mComponentPacks[C_StepCounter], sizeof(StepCounter), 32);
    ComponentPack_Init(C_StepTimer, &gsd->mComponentPacks[C_StepTimer], sizeof(StepTimer), 32);
    ComponentPack_Init(C_StringSettings, &gsd->mComponentPacks[C_StringSettings], sizeof(StringSettings), 1024);
    ComponentPack_Init(C_DrawRenderInfo, &gsd->mComponentPacks[C_DrawRenderInfo], sizeof(DrawRenderInfo), 2048);
    ComponentPack_Init(C_DrawStateInfo, &gsd->mComponentPacks[C_DrawStateInfo], sizeof(DrawStateInfo), 256);
    ComponentPack_Init(C_ParticleInstance, &gsd->mComponentPacks[C_ParticleInstance], sizeof(ParticleInstance), 4096);
    ComponentPack_Init(C_TagIsActor, &gsd->mComponentPacks[C_TagIsActor], sizeof(TagIsActor), 1024);
    ComponentPack_Init(C_TagIsComplete, &gsd->mComponentPacks[C_TagIsComplete], sizeof(TagIsComplete), 64);
    ComponentPack_Init(C_TagIsDrawDisabled, &gsd->mComponentPacks[C_TagIsDrawDisabled], sizeof(TagIsDrawDisabled), 128);
    ComponentPack_Init(C_TagIsInPlay, &gsd->mComponentPacks[C_TagIsInPlay], sizeof(TagIsInPlay), 1024);
    ComponentPack_Init(C_TagIsPlayer, &gsd->mComponentPacks[C_TagIsPlayer], sizeof(TagIsPlayer), 1);
    ComponentPack_Init(C_TagIsUpdateDisabled, &gsd->mComponentPacks[C_TagIsUpdateDisabled], sizeof(TagIsUpdateDisabled), 256);
    ComponentPack_Init(C_ShakeFOverTimeData, &gsd->mComponentPacks[C_ShakeFOverTimeData], sizeof(ShakeFOverTimeData), 16);
}
