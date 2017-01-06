// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"

#include "LevelInstanceBounds.h"

#include "LevelInstance.h"

int32 ULevelInstance::InstanceIdCount = 0;

ULevelInstance::ULevelInstance(const FObjectInitializer & ObjectInitializer)
    : Super(ObjectInitializer)
{
    //** Level Settings */
    bShouldBeLoaded = true;
    bShouldBeVisible = true;
    bShouldBlockOnLoad = false;
    bInitiallyLoaded = true;
    bInitiallyVisible = true;
    //** End Level      */
}

void ULevelInstance::SetupBounds() {
    if (InstancedLevel) {
        //Get all instance bounds actors
        TArray<AActor*> BoundsActors;
        UWorld* IWorld = InstancedLevel.LoadSynchronous();
        if (IWorld) {
            for (TActorIterator<AActor> It(IWorld, TSubclassOf<ALevelInstanceBounds>()); It; ++It) {
                AActor* Actor = *It;
                if (!Actor->IsPendingKill()) {
                    BoundsActors.Add(Actor);
                }
            }
        }
        //Destroy them
        for (auto ActorIt = BoundsActors.CreateConstIterator(); ActorIt; ++ActorIt) {
            if (AActor* Actor = *ActorIt) {
                Actor->Destroy();
            }
        }

        //Spawn a level instance bounds actor
        ALevelInstanceBounds* BoundsActor = InstancedLevel->SpawnActor<ALevelInstanceBounds>();
        BoundsActor->LevelInstance = this;
    }
}
