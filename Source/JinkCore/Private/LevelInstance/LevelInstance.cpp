// Copyright 2015-2017 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "LevelInstance.h"
#include "LevelInstanceBounds.h"

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

    InstanceId = -1;
    StreamingLevel = nullptr;
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



int32 ULevelInstance::InstanceIdCount = 0;

bool ULevelInstance::SpawnLevel(FTransform Transform)
{
	if (IsRegistered() || InstancedLevel.IsNull())
		return false;

	UWorld* const World = GEngine->GetWorldFromContextObject(this, false);
	if (!World) {
		return false;
	}

	// Check whether requested map exists, this could be very slow if LevelName is a short package name
	FString LevelName = InstancedLevel.GetLongPackageName();
	FString LongPackageName = FPackageName::FilenameToLongPackageName(LevelName);

	// Create Unique Name for sub-level package
	const FString ShortPackageName = FPackageName::GetShortName(LongPackageName);
	const FString PackagePath = FPackageName::GetLongPackagePath(LongPackageName);
	FString UniqueLevelPackageName = PackagePath + TEXT("/") + World->StreamingLevelsPrefix + ShortPackageName;
	UniqueLevelPackageName += TEXT("_LevelInstance_") + FString::FromInt(InstanceIdCount);
	// Increment Id counter
	InstanceId = InstanceIdCount;
	InstanceIdCount++;

	// Setup streaming level object that will load specified map
	ULevelStreamingKismet* NewStreamingLevel = NewObject<ULevelStreamingKismet>(World, ULevelStreamingKismet::StaticClass(), NAME_None, RF_Transient, NULL);
	NewStreamingLevel->SetWorldAssetByPackageName(FName(*UniqueLevelPackageName));
	NewStreamingLevel->LevelColor = FColor::MakeRandomColor();
	NewStreamingLevel->bShouldBeLoaded = bShouldBeLoaded;
	NewStreamingLevel->bShouldBeVisible = bShouldBeVisible;
	NewStreamingLevel->bShouldBlockOnLoad = bShouldBlockOnLoad;
	NewStreamingLevel->bInitiallyLoaded = bInitiallyLoaded;
	NewStreamingLevel->bInitiallyVisible = bInitiallyVisible;

	// Transform
	NewStreamingLevel->LevelTransform = Transform;
	// Map to Load
	NewStreamingLevel->PackageNameToLoad = FName(*LongPackageName);

	// Add the new level to world.
	World->StreamingLevels.Add(NewStreamingLevel);

	//Save level reference
	StreamingLevel = NewStreamingLevel;
	return true;
}

bool ULevelInstance::LoadLevel()
{
	if (!IsRegistered())
		return false;
	StreamingLevel->bShouldBeLoaded = true;
	return true;
}

void ULevelInstance::SetLevelVisibility(bool NewVisibility)
{
	if (IsRegistered()) {
		StreamingLevel->bShouldBeVisible = NewVisibility;
	}
}

void ULevelInstance::UnloadLevel()
{
    if (IsRegistered()) {
        StreamingLevel->bShouldBeLoaded = false;
    }
}

void ULevelInstance::RemoveLevel() {
    if (IsRegistered()) {
        //Find Example at: LevelCollectionModel.cpp:568
    }
}

FString ULevelInstance::GetUniqueName()
{
    if (!IsRegistered()) {
        return TEXT("None");
    }
    return TEXT("");
}
