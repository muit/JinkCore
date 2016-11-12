// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "LevelInstance.h"

int32 ALevelInstance::InstanceIdCount = 0;

ALevelInstance::ALevelInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer) {
    /** Volume Settings */
    bSolid = true;
    Color = FColor::Blue;
    Color.A = 0.6f;
    //** End Volume     */


    //** Level Settings */
    bRegisterOnBeginPlay = true;
    bShouldBeLoaded = true;
    bShouldBeVisible = true;
    bShouldBlockOnLoad = false;
    bInitiallyLoaded = true;
    bInitiallyVisible = true;
    //** End Level      */

    InstanceId = -1;
    AssignedLevel = nullptr;
}


void ALevelInstance::BeginPlay()
{
    Super::BeginPlay();
    if (bRegisterOnBeginPlay) {
        SpawnLevel();
    }
}

bool ALevelInstance::SpawnLevel()
{
    if (IsRegistered() || LevelAsset.IsNull())
        return false;

    UWorld* const World = GEngine->GetWorldFromContextObject(this, false);
    if (!World) {
        return false;
    }

    // Check whether requested map exists, this could be very slow if LevelName is a short package name
    FString LevelName = LevelAsset.GetLongPackageName();
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
    ULevelStreamingKismet* StreamingLevel = NewObject<ULevelStreamingKismet>(World, ULevelStreamingKismet::StaticClass(), NAME_None, RF_Transient, NULL);
    StreamingLevel->SetWorldAssetByPackageName(FName(*UniqueLevelPackageName));
    StreamingLevel->LevelColor = FColor::MakeRandomColor();
    StreamingLevel->bShouldBeLoaded = bShouldBeLoaded;
    StreamingLevel->bShouldBeVisible = bShouldBeVisible;
    StreamingLevel->bShouldBlockOnLoad = bShouldBlockOnLoad;
    StreamingLevel->bInitiallyLoaded = bInitiallyLoaded;
    StreamingLevel->bInitiallyVisible = bInitiallyVisible;

    // Transform
    StreamingLevel->LevelTransform = GetActorTransform();
    // Map to Load
    StreamingLevel->PackageNameToLoad = FName(*LongPackageName);

    // Add the new level to world.
    World->StreamingLevels.Add(StreamingLevel);

    //Save level reference
    AssignedLevel = StreamingLevel;
    return true;
}

bool ALevelInstance::LoadLevel()
{
    if (!IsRegistered())
        return false;
    AssignedLevel->bShouldBeLoaded = true;
    return true;
}

void ALevelInstance::SetLevelVisibility(bool NewVisibility)
{
    if (IsRegistered()) {
        AssignedLevel->bShouldBeVisible = NewVisibility;
    }
}

void ALevelInstance::UnloadLevel()
{
    if (IsRegistered()) {
        AssignedLevel->bShouldBeLoaded = false;
    }
}

void ALevelInstance::RemoveLevel() {
    if (IsRegistered()) {

    }
}

FString ALevelInstance::GetUniqueName()
{
    if (!IsRegistered()) {
        return TEXT("None");
    }
    return TEXT("");
}
