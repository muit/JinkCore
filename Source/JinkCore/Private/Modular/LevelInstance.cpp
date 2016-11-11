// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "LevelInstance.h"

//Set instance counter to 0
int32 ALevelInstance::InstanceCount = 0;

ALevelInstance::ALevelInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	bShouldBeLoaded = true;
	bShouldBeVisible = true;
	bShouldBlockOnLoad = false;
	bInitiallyLoaded = true;
	bInitiallyVisible = true;

	InstanceIndex = -1;

	AssignedLevel = nullptr;
	bRegisteredInWorld = false;
}

void ALevelInstance::BeginPlay()
{
	Super::BeginPlay();
	LoadLevel();
	RegistryLevelInWorld();
}

void ALevelInstance::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UnloadLevel();
}

bool ALevelInstance::LoadLevel()
{
	if (LevelAsset.IsNull())
		return false;

	UWorld* const World = GetWorld();
	if (!World)
		return false;

	ULevelStreamingKismet* StreamingLevel = NewObject<ULevelStreamingKismet>((UObject*)GetTransientPackage(), ULevelStreamingKismet::StaticClass());
	if (!StreamingLevel)
	{
		return false;
	}

	//Full Name
	FString FullPackageName = LevelAsset.GetLongPackageName();
	//Long Package Name
	FString LongLevelPackageName = FPackageName::FilenameToLongPackageName(FullPackageName);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Ensure unique names to gain ability to have multiple instances of same level.
	// Use the InstanceIndex as a counter
	FString UniqueLevelPackageName = LongLevelPackageName + "_" + FString::FromInt(InstanceCount);
	// Increment counter
	InstanceIndex = InstanceCount;
	InstanceCount++;
	//Set the level
	StreamingLevel->SetWorldAssetByPackageName(FName(*UniqueLevelPackageName));
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	StreamingLevel->PackageNameToLoad = FName(*FullPackageName);

	if (!FPackageName::DoesPackageExist(StreamingLevel->PackageNameToLoad.ToString(), NULL, &FullPackageName)) {
		return false;
	}



	//Map package to load
	StreamingLevel->PackageNameToLoad = FName(*LongLevelPackageName);

	AssignedLevel = StreamingLevel;
	bRegisteredInWorld = false;

	UE_LOG(JinkCore, Warning, TEXT("JinkCore: Loaded Instanced Level"));
	return true;
}

bool ALevelInstance::RegistryLevelInWorld()
{
	if (UWorld* World = GetWorld()) {
		if (IsLoaded() && !IsRegisteredInWorld()) {
			if (World->IsPlayInEditor()) {
				FWorldContext WorldContext = GEngine->GetWorldContextFromWorldChecked(World);
				AssignedLevel->RenameForPIE(WorldContext.PIEInstance);
			}

			//Setup the level
			AssignedLevel->LevelColor = FColor::MakeRandomColor();
			AssignedLevel->bShouldBeLoaded = bShouldBeLoaded;
			AssignedLevel->bShouldBeVisible = bShouldBeVisible;
			AssignedLevel->bShouldBlockOnLoad = bShouldBlockOnLoad;
			AssignedLevel->bInitiallyLoaded = bInitiallyLoaded;
			AssignedLevel->bInitiallyVisible = bInitiallyVisible;

			//Apply Transform
			AssignedLevel->LevelTransform = GetActorTransform();

			//Registry the level
			World->StreamingLevels.Add(AssignedLevel);
			bRegisteredInWorld = true;
			return true;
		}
	}
	return false;
}

void ALevelInstance::UnloadLevel() {
	if (UWorld* World = GetWorld()) {
		World->StreamingLevels.Remove(AssignedLevel);
		UE_LOG(JinkCore, Warning, TEXT("JinkCore: Registered Instanced Level"));
	}
}