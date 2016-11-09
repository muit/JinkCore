// Copyright 2015-2016 Piperift. All Rights Reserved.

#include "JinkCorePrivatePCH.h"
#include "LevelInstance.h"

//Set instance counter to 0
int32 ALevelInstance::InstanceIndex = 0;

ALevelInstance::ALevelInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Our root component
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = SceneComponent;
}

void ALevelInstance::BeginPlay()
{
	Super::BeginPlay();
	LoadLevelInstance();
	
}

// Called every frame
void ALevelInstance::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}


bool ALevelInstance::LoadLevelInstance()
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
	FString UniqueLevelPackageName = LongLevelPackageName + "_" + FString::FromInt(InstanceIndex);
	// Increment counter
	InstanceIndex++;
	StreamingLevel->SetWorldAssetByPackageName(FName(*UniqueLevelPackageName));
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (World->IsPlayInEditor()) {
		FWorldContext WorldContext = GEngine->GetWorldContextFromWorldChecked(World);
		StreamingLevel->RenameForPIE(WorldContext.PIEInstance);
	}

	StreamingLevel->LevelColor = FColor::MakeRandomColor();
	StreamingLevel->bShouldBeLoaded = true;
	StreamingLevel->bShouldBeVisible = true;
	StreamingLevel->bShouldBlockOnLoad = false;
	StreamingLevel->bInitiallyLoaded = true;
	StreamingLevel->bInitiallyVisible = true;

	//Apply Transform
	StreamingLevel->LevelTransform = GetActorTransform();
	StreamingLevel->PackageNameToLoad = FName(*FullPackageName);

	if (!FPackageName::DoesPackageExist(StreamingLevel->PackageNameToLoad.ToString(), NULL, &FullPackageName)) {
		return false;
	}

	//Map package to load
	StreamingLevel->PackageNameToLoad = FName(*LongLevelPackageName);

	// Add the new level to the world
	World->StreamingLevels.Add(StreamingLevel);

	UE_LOG(JinkCore, Warning, TEXT("JinkCore: Loaded Instanced Level"));
	return true;
}

