// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "LevelInstance.h"
#include "LevelInstanceBounds.h"

ULevelInstance::ULevelInstance(const FObjectInitializer & ObjectInitializer)
	: Super(ObjectInitializer)
{
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
		ALevelInstanceBounds* Bounds = InstancedLevel->SpawnActor<ALevelInstanceBounds>();
		Bounds->LevelInstance = this;
	}
}